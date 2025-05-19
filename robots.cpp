#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

Upgrade::Upgrade(string name, int x, int y, int w, int h)
    : Robot(name, x, y, w, h) {
}

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
        setPosition(0, 0); // Move to outside battle field

        if (lives > 0) {
            cout << name << " is waiting to respawn (" << lives << " lives remaining)" << endl;
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
        cout << name << " respawned, " << lives << " lives remaining." << endl;
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
    hasMoved = true;
    int centerX = getX();
    int centerY = getY();
    int newX;
    int newY;
    vector<pair<int, int>> empty_points;
    vector<pair<int, int>> surrounding_point;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {

            int pointX = centerX + dx;
            int pointY = centerY + dy;       

            // Enemy point
            if (battlefield->isRobotAt(pointX, pointY)){
                surrounding_point.push_back({pointX, pointY}); 
            }
            
            // Empty point
            else if (!(dx == 0 && dy == 0) && !(pointX <=0 ||pointY <=0 || pointX > battlefield->getWidth() || pointY > battlefield->getHeight())){
                empty_points.push_back({pointX, pointY}); 
                surrounding_point.push_back({pointX, pointY}); 
            }
        }
    }

    // move --> look (move to an enemy point or empty point)
    if(hasLooked=false){
        int size = surrounding_point.size();
        srand(time(0));            
        int num = rand() % size ;
        newX = surrounding_point[num].first;
        newY = surrounding_point[num].second;

        if(battlefield->isRobotAt(newX, newY)){
            auto enemy = battlefield->findRobotAt(newX, newY);
            cout << name << " cannot move to ("<< newX << ","<< newY << "). This point has occuppied by" << enemy->getName() << endl;
        }

        else{
            setPosition(newX, newY);
            cout << name << " moved to (" << newX << "," << newY << ")" << endl;
        }
    }

    // look --> move (move to an empty point)
    if(hasLooked=true){

        int size = empty_points.size();
        srand(time(0));            
        int num = rand() % size ;
        newX = empty_points[num].first;
        newY = empty_points[num].second;

        if(size == 0){
            cout << name << " doesn't found any empty point to move! Maybe "<< name << " is surounding by enemies! "<< endl;
        }

        else if(battlefield->isRobotAt(newX, newY)){
            auto enemy = battlefield->findRobotAt(newX, newY);
            cout << name << " cannot move to ("<< newX << ","<< newY << "). This point has occuppied by" << enemy->getName() << endl;
        }

        else{
            setPosition(newX, newY);
            cout << name << " moved to (" << newX << "," << newY << ")" << endl;
        }
    }
}

void GenericRobot::fire(int dx, int dy) {
    
    if (shells == 0) {
        cout << name << " has no shells left! Self-destructing..." << endl;
        selfDestructed = true;
        destroy();
        return;
    }
    
    shells--;
    int targetX ;
    int targetY ;

    // fire --> look
    if(hasLooked == false){
        vector<pair<int, int>> surrounding_point;
        int centerX = getX() ;
        int centerY = getY() ;
        
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {

                int lookX = centerX + dx;
                int lookY = centerY + dy;       

                // Robot itself point
                if (dx == 0 && dy == 0 ){
                    continue;
                }
                
                // Out of bounds
                else if (lookX <=0 ||lookY <=0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight()){
                    continue;
                }

                else{
                    surrounding_point.push_back({lookX, lookY});
                }
            }
        }

        int size = surrounding_point.size();
        srand(time(0));            
        int num = rand() % size ;
        targetX = surrounding_point[num].first;
        targetY = surrounding_point[num].second;
    }
        
    // look --> fire
    else{
        hasFired = true;
        int cout_enemy = lookGot_enemy_point.size();

        if(cout_enemy==0){
          cout << "Preserving shell for next turn since " << name << " didn't find any robots around." << endl;
          return;
        }

        else if(cout_enemy==1){
            targetX = lookGot_enemy_point[0].first;
            targetY = lookGot_enemy_point[0].second;
        }

        else{
           return;
            // more enemy, need to check which is the higher enemy
        } 
    }

    
    if(battlefield->findRobotAt(targetX, targetY)){
        auto enemy = battlefield->findRobotAt(targetX, targetY);
        cout << name << " fires "<< enemy->getName() <<" at (" << targetX << "," << targetY << ")";
        cout << " left shells: " << shells << endl;

        if (rand() % 100 < 70){
            cout << "Target hit! " << enemy->getName() << " has been destroyed! " << endl;
            enemy->destroy();
            // performUpgrade();
        }
        else{
            cout << " - MISS!" << endl;
        }

    }

    else{
        cout << name << " fires at (" << targetX << "," << targetY << "). But it is an empty space!";
        cout << " left shells: " << shells << endl;

    }
    

    

    lookGot_enemy_point.clear();
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

void Upgrade::performUpgrade() {

    if(upgrade_time == 3){
        cout << name << " has upgrade 3 times! Cannot upgrade anymore."<< endl;
        return;
    }

    // Alr upgrade move area
    if (upgrade_move == true){
        // remove element "move" from vector upgrade_type
        upgrade_type.erase(
            remove(upgrade_type.begin(), upgrade_type.end(), "move"),
            upgrade_type.end()
        );
    }

    if (upgrade_shoot == true){
        upgrade_type.erase(
            remove(upgrade_type.begin(), upgrade_type.end(), "shoot"),
            upgrade_type.end()
        );
    }

    if (upgrade_see == true){
        upgrade_type.erase(
            remove(upgrade_type.begin(), upgrade_type.end(), "see"),
            upgrade_type.end()
        );
    }

    int size = upgrade_type.size();
    srand(time(0));            
    int num = rand() % size ;
    string type = upgrade_type[num];

    if(type == "move"){
        upgrade_time++;
        upgrade_move = true;

        srand(time(0));
        int index = rand() % move_type.size();
        string chosen = move_type[index];

        if(chosen=="HideBot"){
            HideBot = true;
        }

        else{
            Jumpbot = true;
        }
    }

    else if(type == "shoot"){
        upgrade_time++;
        upgrade_shoot = true;

        srand(time(0));
        int index = rand() % shoot_type.size();
        string chosen = shoot_type[index];

        if(chosen=="LongShotBot"){
            LongShotBot = true;
        }

        else if(chosen=="SemiAutoBot"){
            SemiAutoBot = true;
        }

        else{
            ThirtyShotBot = true;
        }
        
    }

    else if(type == "see"){
        upgrade_time++;
        upgrade_see = true;

        srand(time(0));
        int index = rand() % see_type.size();
        string chosen = see_type[index];

        if(chosen=="ScoutBot"){
            ScoutBot = true;
        }

        else{ 
            TrackBot = true;
        }
    }
 
}