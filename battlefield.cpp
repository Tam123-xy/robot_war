#include "battlefield.h"
#include "robots.h"
#include <iostream>
using namespace std;

Battlefield::Battlefield(int w, int h)
    : width(w), height(h), gen(rd()), xDist(0, w-1), yDist(0, h-1) {}

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
    std::shuffle(robots.begin(), robots.end(), gen);
    
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
    if (!respawnQueue.empty()) {
        auto robot = respawnQueue.front();
        respawnQueue.pop();
        
        if (robot->getLives() > 0) {
            int x, y;
            int attempts = 0;
            do {
                x = xDist(gen);
                y = yDist(gen);
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
    if (auto gr = dynamic_cast<GenericRobot*>(robot.get())) {
        gr->think();
        
        // Randomize action order while respecting limits
        vector<int> actions = {0, 1, 2}; // 0=look, 1=fire, 2=move
        shuffle(actions.begin(), actions.end(), gen);
        
        for (int action : actions) {
            switch(action) {
                case 0: // Look
                    if (gr->canLook()) gr->look(rand()%3-1, rand()%3-1);
                    break;
                case 1: // Fire
                    if (gr->canFire()) {
                        int dx, dy;
                        do {
                            dx = rand()%3-1;
                            dy = rand()%3-1;
                        } while (dx == 0 && dy == 0);
                        gr->fire(dx, dy);
                    }
                    break;
                case 2: // Move
                    if (gr->canMove()) gr->move(rand()%3-1, rand()%3-1);
                    break;
            }
        }
    }
}

void Battlefield::executeAttack(GenericRobot* gr) {
    // 1. Look around first
    if (gr->canLook()) {
        auto surroundings = gr->look(0, 0);
        for (const auto& s : surroundings) {
            std::cout << s << std::endl;
        }
    }
    
    // 2. Fire at enemies if possible
    if (gr->canFire()) {
        // Simple AI: Fire at first spotted enemy
        auto surroundings = gr->look(0, 0);
        for (const auto& s : surroundings) {
            if (s.find("Enemy") != std::string::npos) {
                // Parse coordinates from look info
                size_t start = s.find('(');
                size_t mid = s.find(',');
                size_t end = s.find(')');
                int x = std::stoi(s.substr(start+1, mid-start-1));
                int y = std::stoi(s.substr(mid+1, end-mid-1));
                int dx = x - gr->getX();
                int dy = y - gr->getY();
                gr->fire(dx, dy);
                break;
            }
        }
    }
    
    // 3. Move if possible
    if (gr->canMove()) {
        gr->move(rand()%3-1, rand()%3-1);  // Random move
    }
}

void Battlefield::executeConserve(GenericRobot* gr) {
    // 1. Look around
    if (gr->canLook()) {
        auto surroundings = gr->look(0, 0);
        for (const auto& s : surroundings) {
            std::cout << s << std::endl;
        }
    }
    
    // 2. Move (higher priority when conserving)
    if (gr->canMove()) {
        gr->move(rand()%3-1, rand()%3-1);
    }
    
    // 3. Only fire with 30% probability when conserving
    if (gr->canFire() && (rand()%100 < 30)) {
        gr->fire(rand()%3-1, rand()%3-1);
    }
}

void Battlefield::executeExplore(GenericRobot* gr) {
    // 1. Move first when exploring
    if (gr->canMove()) {
        gr->move(rand()%3-1, rand()%3-1);
    }
    
    // 2. Look around
    if (gr->canLook()) {
        auto surroundings = gr->look(0, 0);
        for (const auto& s : surroundings) {
            std::cout << s << std::endl;
        }
    }
    
    // 3. Only fire with 70% probability when exploring
    if (gr->canFire() && (rand()%100 < 70)) {
        gr->fire(rand()%3-1, rand()%3-1);
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