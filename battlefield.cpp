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
    shuffle(robots.begin(), robots.end(), gen);
    
    for (auto& robot : robots) {
        if (robot->alive()) {
            executeRobotTurn(robot);
        }
        else if (robot->shouldRespawn()) {
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

void Battlefield::processRespawn() {
    
    // Respon the first queue robot
    if (!respawnQueue.empty()) {
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
            cout << robot->getName() << " respawned at (" << x << "," << y << ")" << endl;
        }
    }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot) {
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
        cout << robot->getName() << "'s action order is " << order[0] << "--> "<< order[1] << "--> "<< order[2] << endl;

        for (const auto& action : order){
            int dx,dy;
            if (action == "look") {
                gr->look(dx, dy);
            }

            else if (action == "fire"){
                gr->fire(dx, dy);
            }
            
            else{
                // gr->move(rand() % 3 - 1, rand() % 3 - 1);
            }
        }

        cout<<endl;

        // Handle destruction if out of shells
        if (gr->getShells() <= 0 && !gr->hasSelfDestructed()) {
            gr->destroy();
            if (gr->shouldRespawn()) {
                respawnQueue.push(shared_ptr<Robot>(robot));
            }
        }
    }
}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(width, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getX()][robot->getY()] = 'R';
        }
    }

    cout << "--- Battlefield Status ---\n";
    for (int i = 1; i < height; i++) {
        for (int j = 1; j < width; j++) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }
}