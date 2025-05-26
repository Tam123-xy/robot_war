#include "battlefield.h"
#include "robots.h"
#include <iostream>
#include <mutex>
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
    shuffle(robots.begin(), robots.end(), gen);

    
    for (auto& robot : robots) {
        if (robot->alive()) {
            executeRobotTurn(robot);
        }
        // else if (robot->shouldRespawn()) {
        //     respawnQueue.push(robot);
        // }
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
    lock_guard<mutex> lock(respawnMutex);
    // Respon the first queue robot
    if (!respawnQueue.empty()) {
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
            cout << robot->getName() << " respawned at (" << newX << "," << newY << ") " << remainingLives << " lives remaining." << endl;
            
            display();
        }
    }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot) {
    if (!robot->alive()) return;  // Skip dead robots
    if (auto trackBot = dynamic_cast<TrackBot*>(robot.get())) {
        trackBot->displayTracked();
    }

    robot->resetTurn();
    robot->think();

    // Create all possible action permutations
    const vector<vector<string>> actionOrders = {
        {"look", "fire", "move"},
        {"look", "move", "fire"},
        //{"fire", "look", "move"},
        //{"fire", "move", "look"},
        {"move", "look", "fire"},
        //{"move", "fire", "look"}
    };

    // Select random order
    auto& order = actionOrders[rand() % actionOrders.size()];
    cout << robot->getName() << "'s action order is " << order[0] << "--> "<< order[1] << "--> "<< order[2] << endl;

    for (const auto& action : order){
        int dx,dy;
        if (action == "look") {
            robot->look(dx, dy);
        }

        else if (action == "fire"){
            robot->fire(dx, dy);
        }
        
        else{
            robot->move(rand() % 3 - 1, rand() % 3 - 1);
            display();

        }
    }

    cout<<endl;

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
    }
}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(width, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()-1][robot->getX()-1] = robot->getName()[0];
        }
    }

    cout << "--- Battlefield Status ---\n";
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }
}