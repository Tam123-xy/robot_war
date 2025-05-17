#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

GenericRobot::GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf)
    : Robot(name, x, y, w, h), battlefield(bf), shells(10), 
    //   selfDestructed(false),empty_point(empty_point) {
    selfDestructed(false) {
    cout << "GenericRobot " << name << " created at (" << x << "," << y << ")" << endl;
}

void GenericRobot::think() {
    cout << name << " is thinking...\n";

}

void GenericRobot::look(int dx, int dy) {
    hasLooked = true;

    int centerX = getX() ;
    int centerY = getY() ;

    cout << name << " now at (" << centerX << "," << centerY << ")" <<endl; 

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {

            int lookX = centerX + dx;
            int lookY = centerY + dy;

            string status;           

            // Robot itself point
            if (dx == 0 && dy == 0 ){
                // status = "Sendiri";
                continue;
            }
            
            // Out of bounds
            else if (lookX <=0 ||lookY <=0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight()){
                // status = "Out of bounds";
                continue;
            }

            // Enemy robot
            else if (battlefield->isRobotAt(lookX, lookY)) {
                status = "Enemy robot";
                lookGot_enemy_point.push_back({lookX, lookY}); 
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;

                // fire --> look
                if (hasFired == false){
                    fire(dx,dy);
                    hasFired = true;
                }

                
            }
            else {
                status = "Empty space";
                empty_point.push_back({lookX, lookY}); 
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
            }
        }
    }
}

void Robot::destroy() {
    if (isAlive) {
        isAlive = false;

        cout << "Target hit!" << endl;
        cout << name << " has been destroyed! ";
        setPosition(0, 0); // Move to outside battle field

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
    // cout << "hi" << endl;
    if (hasMoved) {
        cout << name << " can only move once per turn!" << endl;
        return;
    }
    
    // Validate movement direction
    if (abs(dx) > 1 || abs(dy) > 1) {
        cout << name << ": Invalid move direction (" << dx << "," << dy << ")!" << endl;
        return;
    }

    if(hasLooked=false) // move --> look
    {
        int newX = getX() + dx;
        int newY = getY() + dy;
        
        if (newX >= 0 && newX < getWidth() && newY >= 0 && newY < getHeight()) {
            
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

    if(hasLooked=true){ // look --> move

        cout << "look --> move"<<endl;
        cout << "size" << empty_point.size()<<endl;

        for (int i = 0; i < empty_point.size(); i++) {
            cout << "(" << empty_point[i].first << ", " << empty_point[i].second << ")" << endl;
        }

    }
}

void GenericRobot::fire(int dx, int dy) {

    if (!canFire()) {
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
    cout << " left shells: " << shells << endl;

    if (rand() % 100 < 70) {
        // Find enemy
        auto enemy = battlefield->findRobotAt(targetX, targetY);
        
        // Got enemy and it is alive
        if (enemy && enemy->alive()) {
            enemy->destroy();
        }
        
    }
        // if (battlefield->isEnemyAt(targetX, targetY)) {
        //     cout << "Target hit!" << endl;
        //     auto enemy = battlefield->findRobotAt(targetX, targetY);
        //     if (enemy && enemy->alive()) {
        //         enemy->destroy();
        //     }
        // }

    else { cout << " - MISS!" << endl; }
  
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

// Upgrade
void UpGrade_move::HideBot(){

}

void UpGrade_move::Jumbot(){
    
}

void Robot::upgrade(){
    srand(time(0));            
    int num = rand() % 3 + 1;
    
    if (num ==1 && upgrade_move == false){
        upgrade_move = true;
    }

    else if (num==2 && upgrade_shoot == false){
        upgrade_shoot = true;
    }

    
    else if (num==3 && upgrade_see == false){
        upgrade_shoot = true;
    }

    else{

    }


    
}