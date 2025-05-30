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

bool Battlefield::isLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
            return true;
    }
    return false;
}

void Battlefield::simulateTurn() {
    processRespawn();
    bool simulation = true;

    // Shuffle robots for random turn order
    shuffle(robots.begin(), robots.end(), gen);

    // Print order robot
    vector<shared_ptr<Robot>> copy = robots;  
    copy.erase( // Store the robot which are alive in this turn
        remove_if(copy.begin(), copy.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive();
            }),
        copy.end()
    );

    string r_order = "\nRobot's order: " + copy[0] -> getName();

    int size = copy.size();
    for(int i=1; i<size; i++){
        r_order+= " --> " + copy[i]-> getName();
    }
    cout << r_order << endl; 
    // cout << endl;

    for(auto& robot : copy){
        if(robot->alive()){
            executeRobotTurn(robot,copy);
        }

        else if(robot->shouldRespawn()){
            auto it = find(copy.begin(), copy.end(), robot);
            if (it != copy.end()) {
                cout << "\nSkipping " << robot->getName() << " because it died in this turn." << endl;
                cout<<endl;
            }
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
            respawn_order+= " --> " + robot->getName();
            tempQueue.pop();
        }
        cout << respawn_order << endl; 
        // cout << endl;

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

    // lock_guard<mutex> lock(respawnMutex);
    
    // // Got robot that needs to respawn
    // if (!respawnQueue.empty()) {

    //     // Print respawn order
    //     queue<shared_ptr<Robot>> tempQueue = respawnQueue; // Copy queue respawnQueue
    //     auto copy_robot = tempQueue.front();
    //     tempQueue.pop(); 
    //     string respawn_order = "Respawn robots queue: " + copy_robot -> getName();
    //     while (!tempQueue.empty()) {
    //         shared_ptr<Robot> robot = tempQueue.front();
    //         respawn_order+= "--> " + robot->getName();
    //         tempQueue.pop();
    //     }
    //     cout << respawn_order<< endl; 

    //     // ProcessRespawn
    //     auto robot = respawnQueue.front();
    //     respawnQueue.pop();

    //     int remainingLives = robot->getLives();
    //     if (remainingLives <= 0) return; 
        
    //     // check robot has live
    //     if (robot->getLives() > 0) {
    //         int newX, newY;
    //         int attempts = 0;
    //         do {
    //             newX = 1 + rand() % width;  
    //             newY = 1 + rand() % height;  
    //             newX = 1 + rand() % width;  
    //             newY = 1 + rand() % height;  
    //             if (++attempts > 100) {
    //                 cout << "Couldn't find empty spot for " << robot->getName() << endl;
    //                 respawnQueue.push(robot);  // Retry next turn
    //                 return;
    //             }
    //         } while (findRobotAt(newX,newY));
            
    //         auto gr = make_shared<GenericRobot>(
    //             robot->getName(), newX, newY, width, height, this
    //         );
    //         gr->setLives(remainingLives);
            
    //         replaceRobot(robot, gr);
    //         gr->respawn(newX, newY);
            
    //         // display();
    //     }
    // }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy) {
    if (!robot->alive()) return;  // Skip dead robots
    if (auto trackBot = dynamic_cast<TrackBot*>(robot.get())) {
        trackBot->displayTracked();
    }
    robot->resetTurn();

    // Create all possible action permutations
    const vector<vector<string>> actionOrders = {
        {"look", "fire", "move", "think"},
        {"look", "fire", "think", "move"},
        {"look", "move", "fire", "think"},
        {"look", "move", "think", "fire"},
        {"look", "think", "fire", "move"},
        {"look", "think", "move", "fire"},

        {"fire", "look", "move", "think"},
        {"fire", "look", "think", "move"},
        {"fire", "move", "look", "think"},
        {"fire", "move", "think", "look"},
        {"fire", "think", "look", "move"},
        {"fire", "think", "move", "look"},

        {"move", "look", "fire", "think"},
        {"move", "look", "think", "fire"},
        {"move", "fire", "look", "think"},
        {"move", "fire", "think", "look"},
        {"move", "think", "look", "fire"},
        {"move", "think", "fire", "look"},

    };

    // Cout order action
    auto& order = actionOrders[rand() % actionOrders.size()];
    cout << endl << robot->getName() << "'s action order is " << order[0] << " --> "<< order[1] << " --> " << order[2] << " --> " << order[3] << endl;

    for (const auto& action : order){
        int dx,dy;
        if (action == "look") {
            robot->look(dx, dy);
        }

        else if (action == "fire"){
            robot->fire(dx, dy);
        }
        
        else if (action == "move"){
            robot->move(rand() % 3 - 1, rand() % 3 - 1);
            display();

        }
        else{
            robot->think();
        }
    }

    // Handle destruction if out of shells
    if (robot->getShells() <= 0 && !robot->hasSelfDestructed()) {
        robot->destroy();
        if (robot->shouldRespawn()) {
            respawnQueue.push(shared_ptr<Robot>(robot));
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
        robots.erase(it);
    }

}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(height, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()-1][robot->getX()-1] = robot->getName()[0];
        }
    }

    cout << "--- Battlefield Status ---\n";
    for (int i = 0 ; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }

}