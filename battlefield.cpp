#include "battlefield.h"
#include "robots.h"
#include <iostream>
#include <mutex>
#include <climits>
using namespace std;

Battlefield::Battlefield(int w, int h)
    : width(w), height(h), gen(rd()), xDist(1, w), yDist(1, h) {}

shared_ptr<Robot> Battlefield::findRobotAt(int x, int y) {
    for (auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y && robot->alive()) {
            return robot;
        }
    }
    return nullptr;
}

int Battlefield::getWidth() const {
    return width;
}

int Battlefield::getHeight() const {
    return height;
}

bool Battlefield::isRobotAt(int x, int y) const {
    for (const auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y) {
            return true;  // Robot found at the position
        }
    }
    return false;  // No robot at the position
}

void Battlefield::addRobot(shared_ptr<Robot> robot) {
    robots.push_back(robot);
}

bool Battlefield::isEnemyAt(int x, int y) const {
    for (const auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y && robot->alive()) {
            return true;
        }
    }
    return false;
}

bool Battlefield::isEmpty() const {
    return robots.empty() && respawnQueue.empty();
}

int Battlefield::countAliveRobots() const {
    int count = 0;
    for (const auto& robot : robots) {
        if (robot->alive()) count++;
    }
    return count;
}

Robot* Battlefield::getAliveRobot() const {
    for (const auto& robot : robots) {
        if (robot->alive()) return robot.get();
    }
    return nullptr;
}

int Battlefield::countLiveRobot() const {
    int count = 0;
    for (const auto& robot : robots) {
        if (!robot->alive()){
            count+= robot->getLives();
        }
    }
    return count;
}

void Battlefield::addToRespawn(shared_ptr<Robot> robot) {
        lock_guard<mutex> lock(respawnMutex);
        respawnQueue.push(robot);
}

bool Battlefield::checkAttackHit(shared_ptr<Robot> attacker, 
                               shared_ptr<Robot> target) {
    if (auto hideBot = dynamic_cast<HideBot*>(target.get())) {
        return !hideBot->isHidden();
    }
    return true;
}

void Battlefield::addLandmine(int x, int y) {
        landmines.emplace(x, y);
}

bool Battlefield::checkLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
        landmines.erase(it); 
        if (rand() % 100 < 50) { 
            cout << "Landmine triggered at (" << x << "," << y << ")!\n";
            return true;
        }
    }
    return false;
}

void Battlefield::simulateTurn() {
    processRespawn();
    bool simulation = true;

    // Shuffle robots for random turn order
    // shuffle(robots.begin(), robots.end(), gen);

    // Print order robot
    vector<shared_ptr<Robot>> copy = robots;  
    copy.erase( // Store the robot which are alive in this turn
        remove_if(copy.begin(), copy.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive();
            }),
        copy.end()
    );
    string r_order = "Robots order: " + copy[0] -> getName();
    int size = copy.size();
    for(int i=1; i<size; i++){
        r_order+= "--> " + copy[i]-> getName();
    }
    cout << r_order<< endl; 
    cout << endl;
    
    for (auto& robot : robots) {
        if (robot->alive()) {

            // Cout robot upgrade name
            if(robot->getUpgradeCount()>=1){
                vector<string> copy_upgradeNames = robot->get_upgradeNames();
                string sentence = robot->getName() + " is " + copy_upgradeNames[0];
                int size = copy_upgradeNames.size();
                for(size_t i =1; size<i ;i++){
                    sentence+= "," + copy_upgradeNames[i];
                }
                cout<< sentence <<endl;
            }
            
            executeRobotTurn(robot,copy);
        }

        else if (robot->shouldRespawn()) {
            auto it = find(copy.begin(), copy.end(), robot);
            if (it != copy.end()) {
                cout << "Skipping " << robot->getName() << " because it died in this turn." << endl;
                cout<<endl;
            }
        }      
    }

    // End turn, for those robots who died in turn will go respawn queue
    // for(auto& robot : copy){
    //     if(robot->shouldRespawn()){
    //         respawnQueue.push(robot);
    //     }
    // }

    // Remove dead robots with no lives left
    robots.erase(
        remove_if(robots.begin(), robots.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive() && r->getLives() <= 0;
            }),
        robots.end()
    );
}

void Battlefield::processRespawn() {

    lock_guard<mutex> lock(respawnMutex);
    
    // Got robot that needs to respawn
    if (!respawnQueue.empty()) {

        // Print respawn order
        queue<shared_ptr<Robot>> tempQueue = respawnQueue; // Copy queue respawnQueue
        auto copy_robot = tempQueue.front();
        tempQueue.pop(); 
        string respawn_order = "Respawn robots queue: " + copy_robot -> getName();
        while (!tempQueue.empty()) {
            shared_ptr<Robot> robot = tempQueue.front();
            respawn_order+= "--> " + robot->getName();
            tempQueue.pop();
        }
        cout << respawn_order<< endl; 

        // ProcessRespawn
        auto robot = respawnQueue.front();
        respawnQueue.pop();

        int remainingLives = robot->getLives();
        if (remainingLives <= 0) return; 
        
        // check robot has live
        if (robot->getLives() > 0) {
            int newX, newY;
            int attempts = 0;
            do {
                newX = 1 + rand() % width;  
                newY = 1 + rand() % height;  
                newX = 1 + rand() % width;  
                newY = 1 + rand() % height;  
                if (++attempts > 100) {
                    cout << "Couldn't find empty spot for " << robot->getName() << endl;
                    respawnQueue.push(robot);  // Retry next turn
                    return;
                }
            } while (findRobotAt(newX,newY));
            
            auto gr = make_shared<GenericRobot>(
                robot->getName(), newX, newY, width, height, this
            );
            gr->setLives(remainingLives);
            
            replaceRobot(robot, gr);
            gr->respawn(newX, newY);
            
            // display();
        }
    }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy) {
    if (!robot->alive()) return;  // Skip dead robots
    if (auto trackBot = dynamic_cast<TrackBot*>(robot.get())) {
        trackBot->displayTracked();
    }

    if (auto gr = dynamic_cast<GenericRobot*>(robot.get())) {
        gr->resetTurn();
        gr->think(); 

        // robot->resetTurn();
        // robot->think();

        // Create all possible action permutations
        const vector<vector<string>> actionOrders = {
            // {"look", "fire", "move"},
            // {"look", "move", "fire"},
            {"fire", "look", "move"},
            // {"fire", "move", "look"},
            // {"move", "look", "fire"},
            // {"move", "fire", "look"}
        };

        // Cout order action
        auto& order = actionOrders[rand() % actionOrders.size()];
        cout << order[0] << "--> "<< order[1] << "--> "<< order[2] << endl;

        // ScoutBot
        if (robot->isScout()) {
            int count = robot->getScoutCount(); // Check how many times power has been used  
            if(count == 3){
                cout << "Cannot see the entire battlefield — the ability has already been used 3 times." << endl;
            }else{
                // move/fire --> look, use power. 
                // Push values to vectors 
                // 1) enemy points which are surrounding the robot, 
                // 2) enemy points which are NOT surrounding the robot, 
                // 3) empty points which are surrounding the robot
                if(order[0]!="look"){ 

                    // All enemies point
                    for (auto& copy_robot : copy){
                        if(copy_robot->getName()!= robot->getName() && copy_robot->alive()){
                            robot->addScoutPoint({copy_robot->getX(), copy_robot->getY()}); 
                        }else continue; 
                    }

                    robot->setUseScout(true);
                    robot->setScoutCount(count + 1);
                    vector<string> ordinal_numbers ={"1","2","3"};
                    cout << "ScoutBot -- See the entire battlefield (Total: " << ordinal_numbers[count] << "/3)." << endl;

                    int x = robot->getX();
                    int y = robot->getY();

                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {

                            int lookX = x + dx;
                            int lookY = y + dy;         

                            if (dx == 0 && dy == 0) continue; // Robot itself point
                           
                            else if (lookX <=0 ||lookY <=0 || lookX > getWidth() || lookY > getHeight()) continue;  // Out of bounds

                            else if (isRobotAt(lookX, lookY)) { // Enemy robot
                                robot->add_LookGotEnemyPoint({lookX, lookY}); // 1) enemy points which are surrounding the robot, 

                            }else{ // Empty space   
                                robot->add_EmptyPoint({lookX, lookY}); // 2) enemy points which are NOT surrounding the robot, 
                            }
                        }
                    }

                    // 3) empty points which are surrounding the robot
                    for (const auto& point : robot->getScoutPoints()) {
                        bool isSurrounding = false;
                        for (const auto& s : robot->get_LookGotEnemyPoint()) {
                            if (point == s) {
                                isSurrounding = true;
                                break;
                            }
                        }
                        if (!isSurrounding) {
                            robot->add_enemy_Outside_surrouding_point(point);
                        }
                    }

                    vector<pair<int, int>> copy_scoutPoints = robot->getScoutPoints();
                    int size = copy_scoutPoints.size();
                    if (size == 0){
                        cout << "No enemy found on the battlefield." << endl;
                    }else{
                        auto enemy = findRobotAt(copy_scoutPoints[0].first, copy_scoutPoints[0].second);
                        string sentence = "ScoutBot -- All enemies' points: " + enemy->getName() +"(" + to_string(copy_scoutPoints[0].first) + "," +to_string(copy_scoutPoints[0].second)+")";
                        for(size_t i =1;i<size; i++ ){
                            enemy = findRobotAt(copy_scoutPoints[i].first, copy_scoutPoints[i].second);
                            sentence+= ", "+ enemy->getName()+"(" + to_string(copy_scoutPoints[i].first) + "," +to_string(copy_scoutPoints[i].second)+")";
                        }
                        cout<< sentence <<endl;
                        }
                        
                }
                else{
                    cout << "ScoutBot -- Since the order of actions starts with LOOK," <<endl;
                    cout<<"the power to see the entire battlefield will be preserved for next time." << endl;
                }
            }
        }

    for (const auto& action : order){
        int dx,dy;
        if (action == "look") {
            robot->look(dx, dy);
        }

            else if (action == "fire"){
                gr->fire(dx, dy);
            }
            
            else{
                gr->move(rand() % 3 - 1, rand() % 3 - 1);
                // display();

        }
    }

        cout<<endl;

        // useScout = false;
        for (auto& copy_robot : copy){
            if(copy_robot->getUseScout()){
                robot->setUseScout(false); 
            }
            else continue; 
        }


        // Handle destruction if out of shells
        if (gr->getShells() <= 0 && !gr->hasSelfDestructed()) {
            gr->destroy();
            if (gr->shouldRespawn()) {
                respawnQueue.push(shared_ptr<Robot>(robot));
            }
        }
    }
}

void Battlefield::replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot) {
    newBot->setPosition(oldBot->getX(), oldBot->getY());
    newBot->setLives(oldBot->getLives()); 
    newBot->setShells(oldBot->getShells());
    
    auto it = find(robots.begin(), robots.end(), oldBot);
    if (it != robots.end()) {
        *it = newBot; 
    }
}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(height, '.')); 

    // for (const auto& robot : robots) {
    //     if (robot->alive()) {
    //         grid[robot->getX()-1][robot->getY()-1] = 'R';
    //     }
    // }
    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()-1][robot->getX()-1] = robot->getName()[0];
        }
    }

    cout << "--- Battlefield Status ---\n";
    for (int i =0 ; i < width; i++) {
        for (int j = 0; j < height; j++) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }

}

void Battlefield::processBestMove(int& newX, int& newY,
                     const vector<pair<int, int>>& empty_points,
                     const vector<pair<int, int>>& enemy_outside_surrouding_point,
                     Battlefield* battlefield){
                        
    pair<int, int> best_move = empty_points[0];
    int min_distance = INT_MAX;
    string final_enemy_name; 

    for (auto& space : empty_points) {
        int closest_enemy_dist = INT_MAX;
        string closest_enemy_name; 

        for (auto& enemy : enemy_outside_surrouding_point) {
            int dist = abs(space.first - enemy.first) + abs(space.second - enemy.second);
            auto enemyy = battlefield->findRobotAt(enemy.first, enemy.second);

            if (dist < closest_enemy_dist) {
                closest_enemy_dist = dist;
                closest_enemy_name = enemyy->getName();
            }
        }

        // Keep the space that gives the minimum distance to any enemy
        if (closest_enemy_dist < min_distance) {
            min_distance = closest_enemy_dist;
            best_move = space;
            final_enemy_name = closest_enemy_name;
        }
    }
    cout << "ScoutBot -- Best move is to (" << best_move.first << "," << best_move.second << ")"
        << " with closest enemy "<< final_enemy_name << endl;
        newX = best_move.first;
        newY = best_move.second;
}