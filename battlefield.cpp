#include "battlefield.h"
#include "robots.h"
#include <iostream>
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

void Battlefield::simulateTurn() {
    processRespawn();
    bool simulation = true;

    // Shuffle robots for random turn order
    // shuffle(robots.begin(), robots.end(), gen);

    vector<shared_ptr<Robot>> copy = robots;  

    // Store the robot which are alive in this turn
    copy.erase(
        remove_if(copy.begin(), copy.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive();
            }),
        copy.end()
    );

    // Print order robot
    string r_order = "Robots order: " + copy[0] -> getName();
    int size = copy.size();
    for(int i=1; i<size; i++){
        r_order+= "--> " + copy[i]-> getName();
    }

    // Print robot order
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

    for(auto& robot : copy){
        if(robot->shouldRespawn()){
            respawnQueue.push(robot);
        }
    }

    // Remove dead robots with no lives left
    robots.erase(
        remove_if(robots.begin(), robots.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive() && r->getLives() <= 0;
            }),
        robots.end()
    );
    
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy) {
    if (!robot->alive()) return;  // Skip dead robots

    if (auto gr = dynamic_cast<GenericRobot*>(robot.get())) {
        gr->resetTurn();
        gr->think();  // Always think first

        // Create all possible action permutations
        const vector<vector<string>> actionOrders = {
            // {"look", "fire", "move"},
            // {"look", "move", "fire"},
            {"fire", "look", "move"},
            // {"fire", "move", "look"},
            // {"move", "look", "fire"},
            // {"move", "fire", "look"}
        };

        // Select random order
        auto& order = actionOrders[rand() % actionOrders.size()];
        cout << order[0] << "--> "<< order[1] << "--> "<< order[2] << endl;

        // ScoutBot
        if (robot->isScout()) {

            // Check how many times power has been used  
            int count = robot->getScoutCount();

            if(count == 3){
                cout << "Cannot see the entire battlefield — the ability has already been used 3 times." << endl;
            }

            else{
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
                        }
                        else continue; 
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

                            // Robot itself point
                            if (dx == 0 && dy == 0) continue;

                            // Out of bounds
                            else if (lookX <=0 ||lookY <=0 || lookX > getWidth() || lookY > getHeight()) continue;

                            // Enemy robot
                            if (isRobotAt(lookX, lookY)) {
                                // 1) enemy points which are surrounding the robot, 
                                robot->add_LookGotEnemyPoint({lookX, lookY});
                            }

                            // Empty space
                            else {
                                // 2) enemy points which are NOT surrounding the robot, 
                                robot->add_EmptyPoint({lookX, lookY});
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
                    } 
                    else {
                        auto enemy = findRobotAt(copy_scoutPoints[0].first, copy_scoutPoints[0].second);
                        string sentence = "ScoutBot -- All enemies' points: " + enemy->getName() +"(" + to_string(copy_scoutPoints[0].first) + "," +to_string(copy_scoutPoints[0].second)+")";
                        for(size_t i =1;i<size; i++ ){
                            enemy = findRobotAt(copy_scoutPoints[i].first, copy_scoutPoints[i].second);
                            sentence+= ", "+ enemy->getName()+"(" + to_string(copy_scoutPoints[i].first) + "," +to_string(copy_scoutPoints[i].second)+")";
                        }
                        cout<< sentence <<endl;

                        // cout << "surrounding enemy" << endl;
                        // for (const auto& pt : robot->get_LookGotEnemyPoint()) {
                        //     cout << "(" << pt.first << ", " << pt.second << "), ";
                        // }
                        // cout<< endl;

                        }
                }

                else{
                    cout << "ScoutBot -- Since the order of actions starts with LOOK, the power to see the entire battlefield will be preserved for next time." << endl;
                }
            }
        }

        for (const auto& action : order){
            int dx,dy;
            if (action == "look") {
                gr->look(dx, dy);
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

void Battlefield::processRespawn() {
    
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
        
        // check robot has live
        if (robot->getLives() > 0) {
            int x, y;
            int attempts = 0;
            do {
                x = xDist(gen); // Generate random point x
                y = yDist(gen); // Generate random point x
                if (++attempts > 100) {
                    cout << "Couldn't find empty spot for " << robot->getName() << endl;
                    respawnQueue.push(robot);  // Retry next turn
                    return;
                }
            } while (findRobotAt(x, y));
            
            robot->respawn(x, y);
            // display();
        }
    }
}

void Battlefield::placeMineAt(int x, int y) {
    const_cast<set<pair<int, int>>&>(mines).insert({x, y});
    mines.insert({x, y});
}

bool Battlefield::checkMineAt(int x, int y) const{
    return mines.find({x, y}) != mines.end();
}

void Battlefield::triggerMineIfAny(Robot* robot, int x, int y) {
    if (checkMineAt(x, y)) {
        if (rand() % 100 < 50) {
            cout << robot->getName() << " stepped on a mine at (" << x << "," << y << ") and was damaged!" << endl;
            robot->destroy();
        } else {
            cout << robot->getName() << " stepped on a mine but avoided damage." << endl;
        }
        mines.erase({x, y});
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
            grid[robot->getX()-1][robot->getY()-1] = 'R';
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

