#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;
random_device rd;
mt19937 gen(rd());

GenericRobot::GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf)
    : Robot(name, x, y, w, h), battlefield(bf), shells(10), 
    //   selfDestructed(false),empty_point(empty_point) {
    selfDestructed(false) {
        cout << "GenericRobot " << name << " created at (" << x << "," << y << ")" << endl;
}

void GenericRobot::think() {
    cout << name << " is thinking... ";
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
            if (dx == 0 && dy == 0 )  continue;

            // Out of bounds
            else if (lookX <=0 ||lookY <=0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight()) continue;

            // Enemy robot
            else if (battlefield->isRobotAt(lookX, lookY)) {
                auto enemy = battlefield->findRobotAt(lookX, lookY);
                status = enemy->getName();
                lookGot_enemy_point.push_back({lookX, lookY}); 
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
            }

            // Empty space
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
        lives--;

        isAlive = false;
        setPosition(0, 0); // Move to outside battle field

        if (lives > 0) {
            cout << name << " is waiting to respawn (" << lives << " lives remaining)" << endl;
        } else {
            cout << name << " has no lives remaining!" << endl << endl;
        }
    }
    
}

void Robot::respawn(int x, int y) {
    if (lives > 0) {
        positionX = x;
        positionY = y;
        isAlive = true;
        init_Upgrade();
        cout << name << " respawned at (" << x << "," << y << "), " << lives << " lives remaining." << endl;
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

    // move -> look, take the POINTS which are surrounding robot itself . POINTS --> (is occupied/ move)
    if (!hasLooked && useScout == false) {
        if (surrounding_points.empty()) {
            cout << name << " has no space to move!" << endl;
            return;
        }

        int num = rand() % surrounding_points.size();
        uniform_int_distribution<> dis(0, 99);
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

    // look -> move , take the EMPTY POINTS which are surrounding robot itself . POINTS --> (no point/ move)
    // ScoutBot , if size == 8 , move to closer point
    else if ( hasLooked || useScout == true) {

        if (empty_points.empty()) {
            cout << name << " didn't find any empty point to move! " << name << " may be surrounded!" << endl;
            return;
        }

        // Got empty point to move

        // ScoutBot will move to an emepty point which is the closer point to an enemy, condition "Surrounding no enemy and outside surrounding got enemy"
        if(useScout == true && lookGot_enemy_point.size()==0 && enemy_outside_surrouding_point.size()!=0 && empty_points.size()>=2){
            pair<int, int> best_move = empty_points[0];
            int min_distance = INT_MAX;
            string final_enemy_name; 

            for (auto& space : empty_points) {
                int closest_enemy_dist = INT_MAX;
                string closest_enemy_name; 

                for (auto& enemy : enemy_outside_surrouding_point) {
                    int dist = abs(space.first - enemy.first) + abs(space.second - enemy.second);
                    auto enemyy = battlefield->findRobotAt(enemy.first, enemy.second);
                    if (dist < closest_enemy_dist) {
                        closest_enemy_dist = dist;
                        closest_enemy_name = enemyy->getName();
                    }
                }

                // Keep the space that gives the minimum distance to any enemy
                if (closest_enemy_dist < min_distance) {
                    min_distance = closest_enemy_dist;
                    best_move = space;
                    final_enemy_name = closest_enemy_name;
                }
            }
            cout << "Best move is to (" << best_move.first << ", " << best_move.second << ")"
                << " with closest enemy "<< final_enemy_name << endl;
                newX = best_move.first;
                newY = best_move.second;
        }
        
        else{
            int num = rand() % empty_points.size();
            newX = empty_points[num].first;
            newY = empty_points[num].second;
        }

        setPosition(newX, newY);
        cout << name << " moved to (" << newX << "," << newY << ")." << endl;
        battlefield->triggerMineIfAny(this, newX, newY); 
    }

    // // If we have jump ability and choose to use it
    // if (canJump() && rand() % 4 == 0) { // 25% chance to use jump if available
    //     int newX = rand() % width;
    //     int newY = rand() % height;
    //     if (jump(newX, newY)) {
    //         return;
    //     }
    // }

    // // If we have hide ability and choose to use it
    // if (canHide() && rand() % 4 == 0) { // 25% chance to use hide if available
    //     if (hide()) {
    //         return;
    //     }
    // }
}

void GenericRobot::fire(int dx, int dy) {
    
    if (shells == 0) {
        cout << name << " has no shells left! Self-destructing..." << endl;
        selfDestructed = true;
        destroy();
        return;
    }
    
    int targetX ;
    int targetY ;

    // fire --> look, take the POINTS which are surrounding robot itself . POINTS --> (shot no enemy/ shot enemy)
    if(hasLooked == false && useScout == false){

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
        
    // look --> fire, take the POINTS which are contain enemies. POINTS --> (NO shot no enemy/ shot enemy)
    else if (hasLooked == true || useScout == true){
        hasFired = true;
        int cout_enemy = lookGot_enemy_point.size();

        // NO shot no enemy, return back
        if(cout_enemy==0){
          cout << "Preserving shell for next turn since " << name << " didn't find any robots around." << endl;
          return;
        }

        else if(cout_enemy==1){
            targetX = lookGot_enemy_point[0].first;
            targetY = lookGot_enemy_point[0].second;
        }

        // many enemies, need to check which is the higher level enemy
        else{
            int i = 0;
            int max_i = 0;
            int max = 0;
            int count;
            for (const auto& point : lookGot_enemy_point){
                auto enemy = battlefield->findRobotAt(point.first, point.second);
                count = enemy -> getUpgradeCount();
                if(count> max){
                    max = count;
                    max_i = i;
                }
                i++;
            }

            targetX = lookGot_enemy_point[max_i].first;
            targetY = lookGot_enemy_point[max_i].second;
            auto enemy = battlefield->findRobotAt(targetX, targetY );
            if(max!=0){
                cout << name << " found out that " << enemy->getName()
                << " has " << max << " updates and a higher level compared to other enemies." << endl;
            }
        } 
    }

    if(battlefield->findRobotAt(targetX, targetY)){
        auto enemy = battlefield->findRobotAt(targetX, targetY);

        // Check if target is HideBot and handle defense
        // if (enemy->getType() == "HideBot") {
        //     if (enemy->hide()) {
        //         shells--;  // Still consume a shell
        //         cout << name << " attacked " << enemy->getName() << ", but target is hidden!" << endl;
        //         return;
        //     }
        // }

        //Proceed with normal attack if not defended
        // Robot is Semi Autobot
        if(isSemiAuto){
            int consecutive = 3;    //SemiAutoBot
            do{
                
                shells--;
                cout << name << " fires " << enemy->getName() << " at (" << targetX << "," << targetY << ")";
                cout << " left shells: " << shells << endl;

                if (true) {
                // if (rand() % 100 > 70) {
                    cout << "Target hit! " << enemy->getName() << " has been destroyed!" << endl;
                    enemy->destroy();                    
                    chooseUpgrade();
                    consecutive = 3; // reset if hit
                } else {
                    cout << " - MISS!" << endl;
                    consecutive--;
                }

            } while (consecutive >= 0); // loop ends here
        } 
        
        else {
            shells--;
            cout << name << " fires "<< enemy->getName() <<" at (" << targetX << "," << targetY << ")";
            cout << " left shells: " << shells << endl;

            if (true) {
            // if (rand() % 100 > 70) { // 70% will success destroy enemy
                cout << "Target hit! " << enemy->getName() << " has been destroyed! " << endl;
                enemy->destroy();
                chooseUpgrade(); // Upgrade
            }

            else{
                cout << " - MISS!" << endl;
                if (isLandmine) { // LandmineBot
                    minePositions.emplace_back(targetX, targetY);   // Random point
                    battlefield->placeMineAt(targetX, targetY);
                    cout << name << " planted a mine at (" << targetX << "," << targetY << ")" << endl;
                }
            } 
        }
    } 

    // Shot no enemy
    else{
        shells--;
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
        // upgradeCount =0 ;   有bug，这里init不到。我在复活的时候init。我已经修好了，这里跟你们说一声
        // isScoutBot = false; 这里init不到，你们去robots.h --> init_Upgrade()加你们要init的东西

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
    if (upgradeCount == 3) {
        string sentence = name + " is " + upgradeNames[0];
        int size = upgradeNames.size();
        for(size_t i =1; size<i ;i++){
            sentence+= "," + upgradeNames[i];
        }
        sentence+= ". Cannot upgrade anymore, max upgrade 3 times";
        cout << sentence<< endl;
        return;
    }

    // isLandmine = true;
    // upgradeNames.push_back("LandmineBot");
    // upgradeCount++;

    isScoutBot = true;
    upgradeNames.push_back("ScoutBot");
    upgradeCount++;

    cout<< upgradeCount << " upgradeCount"<< endl;
    string sentence = name + " now is " + upgradeNames[0];
    int size = upgradeNames.size();
    for(size_t i =1; i<size ;i++){
        sentence+= ", " + upgradeNames[i];
    }
    cout << sentence<< endl;

    // vector<int> availableOptions;
    // if (upgradedAreas.find("move") == upgradedAreas.end()) availableOptions.push_back(0);
    // if (upgradedAreas.find("shoot") == upgradedAreas.end()) availableOptions.push_back(1);
    // if (upgradedAreas.find("see") == upgradedAreas.end()) availableOptions.push_back(2);

    // if (availableOptions.empty()) return;

    // int randomIndex = rand() % availableOptions.size();
    // // int chosenOption = availableOptions[randomIndex];
    // // chooseUpgrade(chosenOption);
    // chooseUpgrade(randomIndex);
}

// void GenericRobot::chooseUpgrade(int upgradeOption) {
//     if (upgradeCount >= 3) return;

//     switch (upgradeOption) {
//         case 0: // Moving upgrade
//             if (upgradedAreas.find("move") == upgradedAreas.end()) {
//                 int choice = rand() % 2;
//                 if (choice == 0) {
//                     activateHideAbility();
//                     upgradeNames.push_back("HideBot");
//                     cout << name << " can now hide 3 times per match!\n";
//                 } else if (choice == 1){
//                     activateJumpAbility();
//                     upgradeNames.push_back("JumpBot");
//                     cout << name << " can now jump 3 times per match!\n";
//                 } else if (choice == 2){
//                     upgradeNames.push_back("??Bot");
//                 }
//                 upgradedAreas.insert("move");
//                 upgradeCount++;
//                 cout << name << " upgraded movement: " << upgradeNames.back() << endl;
//                 cout << name << " now has upgrades: " ;
//                 for(auto s: upgradeNames){
//                     cout << s << ' ' ;
//                 }
//                 cout << endl;
//             }
//         break;

//         case 1: // Shooting upgrade
//             if (upgradedAreas.find("shoot") == upgradedAreas.end()) {
//                 int choice = rand() % 4;
//                 if (choice == 0) {
//                     extendRange();
//                     upgradeNames.push_back("LongShotBot");
//                 } else if (choice == 1) {
//                     isSemiAuto = true;
//                     upgradeNames.push_back("SemiAutoBot");
//                 } else if (choice == 2){
//                     reloadThirtyShots();
//                     upgradeNames.push_back("ThirtyShotBot");
//                 } else if (choice == 3){
//                     isLandmine = true;
//                     upgradeNames.push_back("LandmineBot");
//                 }

//                 upgradedAreas.insert("shoot");
//                 upgradeCount++;
//                 cout << name << " upgraded shooting: " << upgradeNames.back() << endl;
//                 cout << name << " now is " ;
//                 for(auto s: upgradeNames){
//                     cout << s << ' ' ;
//                 }
//                 cout << endl;
//             }
//         break;

//         case 2: // Seeing upgrade
//             if (upgradedAreas.find("see") == upgradedAreas.end()) {
//                 if (rand() % 2 == 0) {
//                     upgradeNames.push_back("ScoutBot");
//                 } else {
//                     upgradeNames.push_back("TrackBot");
//                 }
//                 upgradedAreas.insert("see");
//                 upgradeCount++;
//                 cout << name << " upgraded vision: " << upgradeNames.back() << endl;
//                 cout << name << " now is " ;
//                 for(auto s: upgradeNames){
//                     cout << s << ' ' ;
//                 }
//                 cout << endl;
//             }
//         break;

//         default:
//             break;
//     }
// }

