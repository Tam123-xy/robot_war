#include "robots.h"
#include "battlefield.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mutex>
using namespace std;
// random_device rd;
// mt19937 gen(rd());

GenericRobot::GenericRobot(const string& name, int x, int y, int w, int h, Battlefield* bf)
    : Robot(name, x, y, w, h, bf),
      MovingRobot(name, x, y, w, h, bf),
      ShootingRobot(name, x, y, w, h, bf),
      SeeingRobot(name, x, y, w, h, bf),
      ThinkingRobot(name, x, y, w, h, bf),
      battlefield(bf),
      shells(10),
      selfDestructed(false) {
        //cout << "GenericRobot " << name << " created at (" << x << "," << y << ")" << endl;
}

template<typename T>
shared_ptr<T> GenericRobot::createUpgradedBot() {
        auto newBot = make_shared<T>(
            name, 
            getX(), 
            getY(),
            getWidth(),
            getHeight(),
            battlefield
        );

        // newBot->upgradedAreas = this->upgradedAreas;
        // newBot->upgradeNames = this->upgradeNames;
        // newBot->upgradeCount = this->upgradeCount;
        return newBot;
    }

void GenericRobot::think() {
    cout << name << " is thinking... ";
}

void GenericRobot::look(int dx, int dy) {
    hasLooked = true;

    int centerX = getX() ;
    int centerY = getY() ;

    cout << name << " at (" << centerX << "," << centerY << "), LOOK around ..." <<endl; 

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
            cout << name << " is waiting to respawn (Lives remaining: " << lives << "/3)" << endl;
            battlefield->addToRespawn(shared_from_this());
        } else {
            cout << name << " has no lives remaining! (Lives remaining: " << lives << "/3)" << endl;
        }
    }
    
}

void Robot::respawn(int x, int y) {
    if (lives > 0) {
        positionX = x;
        positionY = y;
        isAlive = true;
        init_Upgrade();
        cout << name << " respawned at (" << x << "," << y << "), (Lives remaining: " << lives << "/3)" << endl;
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
            cout << "ScoutBot -- Best move is to (" << best_move.first << "," << best_move.second << ")"
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
    }

    if (battlefield->checkLandmine(newX, newY)) {
        if (rand() % 100 < 50) {
            cout << name << " triggered a landmine at (" 
                << newX << "," << newY << ")!\n";
            destroy();
        } else {
            cout << name << " narrowly avoided a landmine at ("
                << newX << "," << newY << ")!\n";
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
                if (dx == 0 && dy == 0 ) continue;
                
                // Out of bounds
                else if (lookX <=0 ||lookY <=0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight()) continue;

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
          cout << "Preserving shell for next turn since " << name << " didn't find any robots around. (left shells: " << shells << ")" << endl;
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
        std::random_device rd;
        std::mt19937 gen(rd()); // Mersenne Twister
        std::uniform_int_distribution<> dis(0, 99);
              
        shells--;
        cout << name << " fires "<< enemy->getName() <<" at (" << targetX << "," << targetY << ")";
        cout << " (Left shells: " << shells <<"/10)"<< endl;

        if (enemy->isHidden()) {
            cout << "Attack missed! " << enemy->getName() << " is hidden!" << endl;
            return;
        }

        if (true) {
        // if (dis(gen) < 70) {
            cout << "Target hit! " << enemy->getName() << " has been destroyed! ";
            enemy->destroy();
            chooseUpgrade(); // Upgrade
        }

        else{
            cout << " - MISS!" << endl;
        } 
        
    }

    // Shot no enemy
    else{
        shells--;
        cout << name << " fires at (" << targetX << "," << targetY << "). But it is an empty space!";
        cout << " (Left shells: " << shells <<"/10)"<< endl;
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

    isScoutBot = true;
    upgradeNames.push_back("ScoutBot");
    upgradeCount++;

//     string sentence = name + " now is " + upgradeNames[0];
//     int size = upgradeNames.size();
//     for(size_t i =1; i<size ;i++){
//         sentence+= ", " + upgradeNames[i];
//     }
//     cout << sentence<< endl;

//     vector<int> availableOptions;
//     if (upgradedAreas.find("move") == upgradedAreas.end()) availableOptions.push_back(0);
//     if (upgradedAreas.find("shoot") == upgradedAreas.end()) availableOptions.push_back(1);
//     if (upgradedAreas.find("see") == upgradedAreas.end()) availableOptions.push_back(2);

//     if (availableOptions.empty()) {
//         cout << name << " has no more areas to upgrade!" << endl;
//         return;
//     }

//     int randomIndex = rand() % availableOptions.size();
//     int chosenOption = availableOptions[randomIndex];
//     chooseUpgrade(chosenOption);
// }

// void GenericRobot::chooseUpgrade(int upgradeOption) {
//     if (upgradeCount >= 3) {
//         string sentence = name + " is " + upgradeNames[0];
//         int size = upgradeNames.size();
//         for(size_t i =1; size<i ;i++){
//             sentence+= "," + upgradeNames[i];
//         }
//         sentence+= ". Cannot upgrade anymore, max upgrade 3 times";
//         cout << sentence<< endl;
//         return;
//     }

//     isScoutBot = true;
//     upgradeNames.push_back("ScoutBot");
//     upgradeCount++;
    
//     const char* area = "";
//     switch (upgradeOption) {
//         case 0: area = "move"; break;
//         case 1: area = "shoot"; break;
//         case 2: area = "see"; break;
//         default: 
//             cout << "Invalid upgrade option: " << upgradeOption << endl;
//             return;
//     }
    
//     if (upgradedAreas.find(area) != upgradedAreas.end()) {
//         cout << name << " already upgraded " << area << " area!" << endl;
//         return;
//     }

//     auto self = shared_from_this();
//     shared_ptr<GenericRobot> newBot;
//     string upgradeName = "";

// switch (upgradeOption) {
//     case 0: // Moving upgrade
//         {
//             int choice = 0;
//             if (choice == rand() % 2) {
//                 upgradeName = "HideBot";
//                 newBot = createUpgradedBot<HideBot>();
//             } else {
//                 upgradeName = "JumpBot";
//                 newBot = createUpgradedBot<JumpBot>();
//             }
//             cout << name << " upgraded movement: " << upgradeName << endl;
//         }
//         break;

//         case 1: // Shooting upgrade
//             {
//                 int choice = rand() % 4;
//                 if (choice == 0) {
//                     upgradeName = "LongShotBot";
//                     newBot = createUpgradedBot<LongShotBot>();
//                 } else if (choice == 1) {
//                     upgradeName = "SemiAutoBot";
//                     newBot = createUpgradedBot<SemiAutoBot>();
//                 } else if (choice == 2) {
//                     upgradeName = "ThirtyShotBot";
//                     newBot = createUpgradedBot<ThirtyShotBot>();
//                 } else {
//                     upgradeName = "LandmineBot";
//                     newBot = createUpgradedBot<LandmineBot>();
//                 }
//                 cout << name << " upgraded shooting: " << upgradeName << endl;
//             }
//             break;

//         case 2: // Seeing upgrade
//             {
//                 int choice = rand() % 2;
//                 if (choice == 0) {
//                     upgradeName = "ScoutBot";
//                     newBot = createUpgradedBot<ScoutBot>();
//                 } else {
//                     upgradeName = "TrackBot";
//                     newBot = createUpgradedBot<TrackBot>();
//                 }
//                 cout << name << " upgraded vision: " << upgradeName << endl;
//             }
//             break;
//     }

//     if (newBot) {
//         upgradeNames.push_back(upgradeName);
//         upgradedAreas.insert(area);
//         upgradeCount++;
        
//         newBot->upgradeNames = this->upgradeNames;
//         newBot->upgradedAreas = this->upgradedAreas;
//         newBot->upgradeCount = this->upgradeCount;
//         newBot->name = this->name;  
        
//         battlefield->replaceRobot(self, newBot);
        
//         cout << name << " now has upgrades: ";
//         for(const auto& s: upgradeNames){
//             cout << s << ' ';
//         }
//         cout << " (Total: " << upgradeCount << "/3)" << endl;
        

//         if(upgradeCount >= 2) {
//             newBot->replaceWithCombination(newBot->upgradeNames);
//         }
//     } else {
//         cout << "Failed to create upgraded robot for " << upgradeName << endl;
//     }
}

// void GenericRobot::replaceWithCombination(const vector<string>& types) {
//     if (types.size() < 2) return; 
    
//     auto self = shared_from_this();
//     shared_ptr<GenericRobot> newBot;
//     string combinationName = "";
    
//     auto hasType = [&](const string& type) {
//         return find(types.begin(), types.end(), type) != types.end();
//     };

//     cout << "Attempting combination with " << types.size() << " upgrades: ";
//     for (const auto& t : types) cout << t << " ";
//     cout << endl;

//     if (types.size() == 2) {
//         // Movement + Shooting combinations
//         if (hasType("HideBot") && hasType("LongShotBot")) {
//             newBot = createUpgradedBot<HideLongShotBot>();
//             combinationName = "HideLongShotBot";
//         } else if (hasType("HideBot") && hasType("SemiAutoBot")) {
//             newBot = createUpgradedBot<HideSemiAutoBot>();
//             combinationName = "HideSemiAutoBot";
//         } else if (hasType("HideBot") && hasType("ThirtyShotBot")) {
//             newBot = createUpgradedBot<HideThirtyShotBot>();
//             combinationName = "HideThirtyShotBot";
//         } else if (hasType("HideBot") && hasType("LandmineBot")) {
//             newBot = createUpgradedBot<HideLandmineBot>();
//             combinationName = "HideLandmineBot";
//         } else if (hasType("JumpBot") && hasType("LongShotBot")) {
//             newBot = createUpgradedBot<JumpLongShotBot>();
//             combinationName = "JumpLongShotBot";
//         } else if (hasType("JumpBot") && hasType("SemiAutoBot")) {
//             newBot = createUpgradedBot<JumpSemiAutoBot>();
//             combinationName = "JumpSemiAutoBot";
//         } else if (hasType("JumpBot") && hasType("ThirtyShotBot")) {
//             newBot = createUpgradedBot<JumpThirtyShotBot>();
//             combinationName = "JumpThirtyShotBot";
//         } else if (hasType("JumpBot") && hasType("LandmineBot")) {
//             newBot = createUpgradedBot<JumpLandmineBot>();
//             combinationName = "JumpLandmineBot";
//         }
//         // Movement + Vision combinations
//         else if (hasType("HideBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<HideScoutBot>();
//             combinationName = "HideScoutBot";
//         } else if (hasType("HideBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<HideTrackBot>();
//             combinationName = "HideTrackBot";
//         } else if (hasType("JumpBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<JumpScoutBot>();
//             combinationName = "JumpScoutBot";
//         } else if (hasType("JumpBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<JumpTrackBot>();
//             combinationName = "JumpTrackBot";
//         }
//         // Shooting + Vision combinations
//         else if (hasType("LongShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<LongShotScoutBot>();
//             combinationName = "LongShotScoutBot";
//         } else if (hasType("LongShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<LongShotTrackBot>();
//             combinationName = "LongShotTrackBot";
//         } else if (hasType("SemiAutoBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<SemiAutoScoutBot>();
//             combinationName = "SemiAutoScoutBot";
//         } else if (hasType("SemiAutoBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<SemiAutoTrackBot>();
//             combinationName = "SemiAutoTrackBot";
//         } else if (hasType("ThirtyShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<ThirtyShotScoutBot>();
//             combinationName = "ThirtyShotScoutBot";
//         } else if (hasType("ThirtyShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<ThirtyShotTrackBot>();
//             combinationName = "ThirtyShotTrackBot";
//         } else if (hasType("LandmineBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<LandmineScoutBot>();
//             combinationName = "LandmineScoutBot";
//         } else if (hasType("LandmineBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<LandmineTrackBot>();
//             combinationName = "LandmineTrackBot";
//         }
//     } 
//     else if (types.size() == 3) {
//         if (hasType("HideBot") && hasType("LongShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<HideLongShotScoutBot>();
//             combinationName = "HideLongShotScoutBot";
//         } else if (hasType("HideBot") && hasType("LongShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<HideLongShotTrackBot>();
//             combinationName = "HideLongShotTrackBot";
//         } else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<HideSemiAutoScoutBot>();
//             combinationName = "HideSemiAutoScoutBot";
//         } else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<HideSemiAutoTrackBot>();
//             combinationName = "HideSemiAutoTrackBot";
//         } else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<HideThirtyShotScoutBot>();
//             combinationName = "HideThirtyShotScoutBot";
//         } else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<HideThirtyShotTrackBot>();
//             combinationName = "HideThirtyShotTrackBot";
//         } else if (hasType("HideBot") && hasType("LandmineBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<HideLandmineScoutBot>();
//             combinationName = "HideLandmineScoutBot";
//         } else if (hasType("HideBot") && hasType("LandmineBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<HideLandmineTrackBot>();
//             combinationName = "HideLandmineTrackBot";
//         } else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<JumpLongShotScoutBot>();
//             combinationName = "JumpLongShotScoutBot";
//         } else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<JumpLongShotTrackBot>();
//             combinationName = "JumpLongShotTrackBot";
//         } else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<JumpSemiAutoScoutBot>();
//             combinationName = "JumpSemiAutoScoutBot";
//         } else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<JumpSemiAutoTrackBot>();
//             combinationName = "JumpSemiAutoTrackBot";
//         } else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<JumpThirtyShotScoutBot>();
//             combinationName = "JumpThirtyShotScoutBot";
//         } else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<JumpThirtyShotTrackBot>();
//             combinationName = "JumpThirtyShotTrackBot";
//         } else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("ScoutBot")) {
//             newBot = createUpgradedBot<JumpLandmineScoutBot>();
//             combinationName = "JumpLandmineScoutBot";
//         } else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("TrackBot")) {
//             newBot = createUpgradedBot<JumpLandmineTrackBot>();
//             combinationName = "JumpLandmineTrackBot";
//         }
//     }

//     if (newBot && !combinationName.empty()) {
        
//         newBot->upgradeNames = this->upgradeNames;
//         newBot->upgradedAreas = this->upgradedAreas;
//         newBot->upgradeCount = this->upgradeCount;
//         newBot->name = this->name;
        
//         battlefield->replaceRobot(self, newBot);
//         cout << "Successfully combined " << name << " into " << combinationName << "!" << endl;
//     } else {
//         cout << name << " upgrade combination not found for: ";
//         for (const auto& s : types) cout << s << ' ';
//         cout << "\nUsing individual upgrades instead." << endl;
//     }
// }