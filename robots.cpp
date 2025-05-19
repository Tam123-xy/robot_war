#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Upgrade::Upgrade(string name, int x, int y, int w, int h)
//     : Robot(name, x, y, w, h) {
// }

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



void GenericRobot::move(int dx, int dy) {
    hasMoved = true;
    int centerX = getX();
    int centerY = getY();
    int newX, newY;

    vector<pair<int, int>> empty_points;
    vector<pair<int, int>> surrounding_points;

    for (int dy_ = -1; dy_ <= 1; ++dy_) {
        for (int dx_ = -1; dx_ <= 1; ++dx_) {
            int pointX = centerX + dx_;
            int pointY = centerY + dy_;

            if (dx_ == 0 && dy_ == 0)
                continue;

            // Out of bounds
            if (pointX <= 0 || pointY <=0 || pointX > battlefield->getWidth() || pointY > battlefield->getHeight())
                continue;

            // Enemy or empty
            surrounding_points.push_back({pointX, pointY});
            if (!battlefield->isRobotAt(pointX, pointY)) {
                empty_points.push_back({pointX, pointY});
            }
        }
    }

    srand(time(0));

    // move -> look
    if (!hasLooked) {
        if (surrounding_points.empty()) {
            cout << name << " has no space to move!" << endl;
            return;
        }

        int num = rand() % surrounding_points.size();
        newX = surrounding_points[num].first;
        newY = surrounding_points[num].second;

        if (battlefield->isRobotAt(newX, newY)) {
            auto enemy = battlefield->findRobotAt(newX, newY);
            cout << name << " cannot move to (" << newX << "," << newY << "). This point is occupied by " << enemy->getName() << "." << endl;
        } else {
            setPosition(newX, newY);
            cout << name << " moved to (" << newX << "," << newY << ")." << endl;
            battlefield->triggerMineIfAny(this, newX, newY); 
        }
    }

    // look -> move
    else {
        if (empty_points.empty()) {
            cout << name << " didn't find any empty point to move! " << name << " may be surrounded!" << endl;
            return;
        }

        int num = rand() % empty_points.size();
        newX = empty_points[num].first;
        newY = empty_points[num].second;

        setPosition(newX, newY);
        cout << name << " moved to (" << newX << "," << newY << ")." << endl;
        battlefield->triggerMineIfAny(this, newX, newY); 
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
        if(isSemiAuto){
            int consecutive = 0;
            do{
                auto enemy = battlefield->findRobotAt(targetX, targetY);
                cout << name << " fires "<< enemy->getName() <<" at (" << targetX << "," << targetY << ")";
                cout << " left shells: " << shells << endl;

                if (rand() % 100 < 70){
                    cout << "Target hit! " << enemy->getName() << " has been destroyed! " << endl;
                    enemy->destroy();
                    chooseUpgrade();
                    consecutive = 3;
                    // performUpgrade();
                }
                else{
                    cout << " - MISS!" << endl;
                    consecutive++;
                }
            }while (consecutive < 3);
        }

        else{
            if(battlefield->findRobotAt(targetX, targetY)){
                auto enemy = battlefield->findRobotAt(targetX, targetY);
                cout << name << " fires "<< enemy->getName() <<" at (" << targetX << "," << targetY << ")";
                cout << " left shells: " << shells << endl;

                if (rand() % 100 < 70){
                    cout << "Target hit! " << enemy->getName() << " has been destroyed! " << endl;
                    enemy->destroy();
                    chooseUpgrade();
                    return;
                    // performUpgrade();
                }
                else{
                    cout << " - MISS!" << endl;
                    if (isLandmine) {
                        minePositions.emplace_back(targetX, targetY);
                        battlefield->placeMineAt(targetX, targetY);
                        cout << name << " planted a mine at (" << targetX << "," << targetY << ")" << endl;
                    }
                }
            }
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
        upgradedAreas.clear();
        upgradeNames.clear();
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

void GenericRobot::chooseUpgrade() {
    if (upgradeCount >= 3) {
        cout << name << " now is " ;
        for(auto s: upgradeNames){
            cout << s << ' ' ;
        }
        cout << endl;
        cout << "Cannot Upgrade Anymore" << endl;
        return;
    }

    vector<int> availableOptions;
    if (upgradedAreas.find("move") == upgradedAreas.end()) availableOptions.push_back(0);
    if (upgradedAreas.find("shoot") == upgradedAreas.end()) availableOptions.push_back(1);
    if (upgradedAreas.find("see") == upgradedAreas.end()) availableOptions.push_back(2);

    if (availableOptions.empty()) return;

    int randomIndex = rand() % availableOptions.size();
    int chosenOption = availableOptions[randomIndex];
    chooseUpgrade(chosenOption);
}


void GenericRobot::chooseUpgrade(int upgradeOption) {
    if (upgradeCount >= 3) return;

    switch (upgradeOption) {
        case 0: // Moving upgrade
            if (upgradedAreas.find("move") == upgradedAreas.end()) {
                int choice = rand() % 3;
                if (choice == 0) {
                    grantHide();
                    upgradeNames.push_back("HideBot");
                } else if (choice == 1){
                    grantJump();
                    upgradeNames.push_back("JumpBot");
                } else if (choice == 2){
                    upgradeNames.push_back("??Bot");
                }
                upgradedAreas.insert("move");
                upgradeCount++;
                cout << name << " upgraded movement: " << upgradeNames.back() << endl;
                cout << name << " now is " ;
                for(auto s: upgradeNames){
                    cout << s << ' ' ;
                }
                cout << endl;
            }
        break;

        case 1: // Shooting upgrade
            if (upgradedAreas.find("shoot") == upgradedAreas.end()) {
                //int choice = rand() % 4;
                int choice = 1;
                if (choice == 0) {
                    extendRange();
                    upgradeNames.push_back("LongShotBot");
                } else if (choice == 1) {
                    isSemiAuto = true;
                    upgradeNames.push_back("SemiAutoBot");
                } else if (choice == 2){
                    reloadThirtyShots();
                    upgradeNames.push_back("ThirtyShotBot");
                } else if (choice == 3){
                    isLandmine = true;
                    upgradeNames.push_back("LandmineBot");
                }

                upgradedAreas.insert("shoot");
                upgradeCount++;
                cout << name << " upgraded shooting: " << upgradeNames.back() << endl;
                cout << name << " now is " ;
                for(auto s: upgradeNames){
                    cout << s << ' ' ;
                }
                cout << endl;
            }
        break;

        case 2: // Seeing upgrade
            if (upgradedAreas.find("see") == upgradedAreas.end()) {
                if (rand() % 2 == 0) {
                    grantScout();
                    upgradeNames.push_back("ScoutBot");
                } else {
                    grantTrack();
                    upgradeNames.push_back("TrackBot");
                }
                upgradedAreas.insert("see");
                upgradeCount++;
                cout << name << " upgraded vision: " << upgradeNames.back() << endl;
                cout << name << " now is " ;
                for(auto s: upgradeNames){
                    cout << s << ' ' ;
                }
                cout << endl;
            }
        break;

        default:
            break;
    }
}


// void Upgrade::performUpgrade() {

//     if(upgrade_time == 3){
//         cout << name << " has upgrade 3 times! Cannot upgrade anymore."<< endl;
//         return;
//     }

//     // Alr upgrade move area
//     if (upgrade_move == true){
//         // remove element "move" from vector upgrade_type
//         upgrade_type.erase(
//             remove(upgrade_type.begin(), upgrade_type.end(), "move"),
//             upgrade_type.end()
//         );
//     }

//     if (upgrade_shoot == true){
//         upgrade_type.erase(
//             remove(upgrade_type.begin(), upgrade_type.end(), "shoot"),
//             upgrade_type.end()
//         );
//     }

//     if (upgrade_see == true){
//         upgrade_type.erase(
//             remove(upgrade_type.begin(), upgrade_type.end(), "see"),
//             upgrade_type.end()
//         );
//     }

//     int size = upgrade_type.size();
//     srand(time(0));            
//     int num = rand() % size ;
//     string type = upgrade_type[num];

//     if(type == "move"){
//         upgrade_time++;
//         upgrade_move = true;

//         srand(time(0));
//         int index = rand() % move_type.size();
//         string chosen = move_type[index];

//         if(chosen=="HideBot"){
//             HideBot = true;
//         }

//         else{
//             Jumpbot = true;
//         }
//     }

//     else if(type == "shoot"){
//         upgrade_time++;
//         upgrade_shoot = true;

//         srand(time(0));
//         int index = rand() % shoot_type.size();
//         string chosen = shoot_type[index];

//         if(chosen=="LongShotBot"){
//             LongShotBot = true;
//         }

//         else if(chosen=="SemiAutoBot"){
//             SemiAutoBot = true;
//         }

//         else{
//             ThirtyShotBot = true;
//         }
        
//     }

//     else if(type == "see"){
//         upgrade_time++;
//         upgrade_see = true;

//         srand(time(0));
//         int index = rand() % see_type.size();
//         string chosen = see_type[index];

//         if(chosen=="ScoutBot"){
//             ScoutBot = true;
//         }

//         else{ 
//             TrackBot = true;
//         }
//     }
 
// }