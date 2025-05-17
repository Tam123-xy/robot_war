#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

GenericRobot::GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf)
    : Robot(name, x, y, w, h), battlefield(bf), shells(10), 
      selfDestructed(false) {
    cout << "GenericRobot " << name << " created at (" << x << "," << y << ")" << endl;
}

void GenericRobot::think() {
    cout << name << " is thinking...\n";

}

vector<string> GenericRobot::look(int dx, int dy) {
// vector<string> GenericRobot::look() {
    vector<string> surroundings;

    if (hasLooked) {
        return surroundings;
    }
    hasLooked = true;

    if (hasFired==true){
        hasFired = false;
    }
    else if(hasFired==false){
        hasFired =true;
    }

    int centerX = getX() ;
    int centerY = getY() ;

    cout << name << " now at (" << centerX << "," << centerY << ")" <<endl; 


    for (int yOffset = -1; yOffset <= 1; ++yOffset) {
        for (int xOffset = -1; xOffset <= 1; ++xOffset) {

            int lookX = centerX + xOffset;
            int lookY = centerY + yOffset;

            string status;           

            // Robot itself point
            if (yOffset == 0 && xOffset == 0 ){
                status = "Sendiri";
                // continue;
            }
                
            else if (!(lookX <0 || lookY <0 ||lookX > battlefield->getWidth() || lookY > battlefield->getHeight())){
                //status = "Out of bounds";
                // continue;
                if (battlefield->isRobotAt(lookX, lookY)) {
                    status = "Enemy robot";
                    cout << name << "(" << lookX << "," << lookY << ") " << status <<endl;
                }

                else {
                    status = "Empty space";
                    cout << name << "(" << lookX << "," << lookY << ") " << status <<endl;
                    // surroundings.push_back("(" + to_string(lookX) + "," + 
                    //                  to_string(lookY) + "): " + status);
                }


            }
            // else if (battlefield->isRobotAt(lookX, lookY)) {
            //     status = "Enemy robot";
            //     if (hasFired=false){
            //         hasFired=true;
            //     }
            //     else{
            //         hasFired = false;
            //         dy = lookY - centerY;
            //         dx = lookX - centerX;
            //         fire(dx,dy);
            //         hasFired = true;
            //     }
            //     // surroundings.push_back("(" + to_string(lookX) + "," + 
            //     //                  to_string(lookY) + "): " + status);
            // }


            surroundings.push_back("(" + to_string(lookX) + "," + 
                                 to_string(lookY) + "): " + status);

            if (&&battlefield->isRobotAt(lookX, lookY)) {
                    if (hasFired==false){
                        hasFired=true;
                    }
                    else{
                        hasFired = false;
                        dy = lookY - centerY;
                        dx = lookX - centerX;
                        fire(dx,dy);
                        hasFired = true;
                    }
            }
                                 
        }
    }

            // if (battlefield->isRobotAt(lookX, lookY)) {
            //     if (hasFired=false){
            //         hasFired=true;
            //     }
            //     else{
            //         hasFired = false;
            //         dy = lookY - centerY;
            //         dx = lookX - centerX;
            //         fire(dx,dy);
            //         hasFired = true;
            //     }
            // }
                
    return surroundings;
}

void Robot::destroy() {
    if (isAlive) {
        isAlive = false;
        cout << name << " has been destroyed! ";
        if (lives > 0) {
            cout << "Waiting to respawn (" << lives << " lives remaining)" << endl;
        } else {
            cout << "No lives remaining!" << endl;
        }
    }
}

void Robot::respawn(int x, int y) {
    if (lives > 0) {
        positionX = x;
        positionY = y;
        isAlive = true;
        lives--;
        cout << "After robot " << name << " respawned, " << lives << " lives remaining." << endl;
    }
}

bool Robot::shouldRespawn() const {
    return !isAlive && lives > 0;
}


string GenericRobot::getType() const {
    return "GenericRobot";
}


pair<int, int> GenericRobot::getLastShotTarget() const {
    return lastShotTarget;
}

void GenericRobot::move(int dx, int dy) {
    if (hasMoved) {
        cout << name << " can only move once per turn!" << endl;
        return;
    }
    
    // Validate movement direction
    if (abs(dx) > 1 || abs(dy) > 1) {
        cout << name << ": Invalid move direction (" << dx << "," << dy << ")!" << endl;
        return;
    }
    
    int newX = getX() + dx;
    int newY = getY() + dy;
    
    if (newX > 0 && newX < getWidth() && newY > 0 && newY < getHeight()) {
        if (!battlefield->isRobotAt(newX, newY)) {
            setPosition(newX, newY);
            hasMoved = true;
            cout << name << " moved to (" << newX << "," << newY << ")" << endl;
        }
        else {
            cout << name << ": Cannot move - space occupied!" << endl;
        }
    }
    else {
        cout << name << ": Cannot move out of bounds!" << endl;
    }
}

void GenericRobot::fire(int dx, int dy) {
    if (!canFire()) {
        return;
    }

    if (dx == 0 && dy == 0) {
        cout << name << ": Cannot fire at own position!" << endl;
        return;
    }
    
    if (abs(dx) > 1 || abs(dy) > 1) {
        cout << name << ": Can only fire at adjacent positions!" << endl;
        return;
    }
    
    if (shells <= 0) {
        cout << name << " has no shells left! Self-destructing..." << endl;
        selfDestructed = true;
        destroy();
        return;
    }
    
    
    shells--;
    int targetX = getX() + dx;
    int targetY = getY() + dy;
    lastShotTarget = {targetX, targetY};
    
    cout << name << " fires at (" << targetX << "," << targetY << ")";
    cout << " shells: " << shells << endl;
    if (rand() % 100 < 70) {
        if (battlefield->isEnemyAt(targetX, targetY)) {
            cout << "Target hit!" << endl;
            auto enemy = battlefield->findRobotAt(targetX, targetY);
            if (enemy && enemy->alive()) {
                enemy->destroy();
            }
        }
    } 
    else {
        cout << " - MISS!" << endl;
    }
  
}


void GenericRobot::respawn(int x, int y) {
    Robot::respawn(x, y);  
    if (alive()) {
        shells = 10;
        selfDestructed = false;
        resetTurn();
    }
}



void GenericRobot::destroy() {
    if (!selfDestructed) {
        selfDestructed = true;
        Robot::destroy();  
    }
    // cout << name << " has been destroyed! ";
    // if (lives > 0) {
    //     cout << "Waiting to respawn (" << lives << " lives remaining)" << endl;
    // } else {
    //     cout << "No lives remaining!" << endl;
    // }
}


bool GenericRobot::shouldRespawn() const {
    return !isAlive && lives > 0;
}
int GenericRobot::getX() const {
    return positionX;
}

int GenericRobot::getY() const {
    return positionY;
}