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

void Battlefield::simulateTurn() {
    processRespawn();
    
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
    
    // Check if only one robot remains
    int aliveCount = count_if(robots.begin(), robots.end(),
        [](const shared_ptr<Robot>& r) { return r->alive(); });
    
    if (aliveCount <= 1) {
        cout << "Simulation ending - only one robot remains!" << endl;
    }
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
            {"look", "fire", "move"},
            {"look", "move", "fire"},
            {"fire", "look", "move"},
            {"fire", "move", "look"},
            {"move", "look", "fire"},
            {"move", "fire", "look"}
        };

        // Select random order
        auto& order = actionOrders[rand() % actionOrders.size()];
        cout << robot->getName() << "'s action order is " << order[0] << "--> "<< order[1] << "--> "<< order[2] << endl;

        for (const auto& action : order) {
            if (action == "look" && gr->canLook()) {
                int dx = rand() % 3 - 1;  // -1, 0, or 1
                int dy = rand() % 3 - 1;
                auto surroundings = gr->look(dx, dy);
                // auto surroundings = gr->look();
                for (const auto& s : surroundings) {
                    cout << s << endl;
                }
            }
          //else if (action == "fire" && gr->canFire()) {
            else if (action == "fire") {
                int dx, dy;
                do {
                    dx = rand() % 3 - 1;
                    dy = rand() % 3 - 1;
                } while (dx == 0 && dy == 0);  // Can't fire at self
                gr->fire(dx, dy);
            }
            else if (action == "move" && gr->canMove()) {
                gr->move(rand() % 3 - 1, rand() % 3 - 1);
            }
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

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(width, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()][robot->getX()] = 'R'; 
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