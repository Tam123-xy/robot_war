// /**********|**********|**********|
// Program: main.cpp / robots.h / battlefield.h
// Course: OOPDS
// Trimester: 2520
// Name: TAY SHI XIANG | YIAP WEI SHANZ | TAM XIN YI
// ID: 243UC247GE | 243UC247CV | 243UC247G6
// Lecture Section: TC1L
// Tutorial Section: TT2L
// Email: TAY.SHI.XIANG@student.mmu.edu.my | YIAP.WEI.SHANZ@student.mmu.edu.my | TAM.XIN.YI@student.mmu.edu.my
// Phone: 019-3285968 | 011-59964357 | 011-11026051
// **********|**********|**********/

// #ifndef ROBOTS_H
// #define ROBOTS_H

// #include <iostream>
// #include <string>
// #include <memory>
// #include <vector>
// #include <utility>
// #include <set>
// #include <mutex>
// #include "battlefield.h"
// #include <random>
// using namespace std;

// class Battlefield;  // Forward declaration

// class Robot : public std::enable_shared_from_this<Robot>{
// protected:
//     Battlefield* battlefield;
//     string name;
//     int lives = 3;
//     bool isAlive = true;
//     bool inRespawnQueue = false;
//     int positionX, positionY; // Changed from private to protected
//     int width, height;

//     int shells = 10;
//     int fireRange = 1;
//     bool selfDestructed = false;
   
// public:
//    Robot(string name, int x, int y, int w, int h, Battlefield* bf)  
//         : name(name), positionX(x), positionY(y), width(w), height(h), 
//           battlefield(bf) {}

//     virtual ~Robot() = default;

//     int getX() const { return positionX; }
//     int getY() const { return positionY; }

//     virtual string getType() const = 0;
//     Robot() = delete;

//     void setLives(int l) { lives = l; } 
//     virtual bool isHidden() const { return false; } 

//     virtual void resetTurn() = 0;
//     virtual void think() = 0;
//     virtual void look(int dx, int dy) = 0;
//     virtual void fire(int dx, int dy) = 0;
//     virtual void move(int dx, int dy) = 0;

//     void setPosition(int x, int y) { positionX = x; positionY = y; }
//     bool alive() const { return isAlive; }
//     void destroy();
//     virtual void respawn(int x, int y);
//     bool shouldRespawn() const;
//     string getName() const { return name; }
//     int getLives() const { return lives; }
//     int getWidth() const { return width; }
//     int getHeight() const { return height; }
//     void setShells(int s) { shells = s; }
//     virtual bool hasSelfDestructed() const = 0;
//     virtual int getShells() const = 0;
//     // virtual bool canBeHit() const { return true; }  // Default implementation - can always be hit
//     // virtual bool hide() { return false; } // Base implementation - no defense

//     virtual void add_EmptyPoint(pair<int, int> pos) =0;
//     virtual const vector<pair<int, int>>& get_EmptyPoint() const =0;
//     virtual void add_LookGotEnemyPoint(pair<int, int> pos) =0;
//     virtual const vector<pair<int, int>>& get_LookGotEnemyPoint() const =0;
    
//     // Update
//     virtual int getUpgradeCount() const { return 0; }
//     virtual void init_Upgrade() {}
//     virtual const vector<string>& get_upgradeNames() const = 0;

   
// };

// class MovingRobot : virtual public Robot {
// public:
//     MovingRobot(string name, int x, int y, int w, int h, Battlefield* bf) 
//         : Robot(name, x, y, w, h, bf) {}
//     virtual ~MovingRobot() = default;

//     //Basic Movement
//     virtual void move(int dx, int dy) = 0;
    
//     //Movement validation
//     bool isValidMove(int dx, int dy) const {
//         int newX = positionX + dx;
//         int newY = positionY + dy;
//         return (newX > 0 && newX < width && newY > 0 && newY < height);
//     }
// };

// class SeeingRobot : virtual public Robot {
// public:
//     SeeingRobot(string name, int x, int y, int w, int h, Battlefield* bf) 
//             : Robot(name, x, y, w, h, bf) {}
//     virtual ~SeeingRobot() = default;

//     using Robot::Robot;
//     virtual void look(int dx, int dy) = 0;

// };

// class ShootingRobot : virtual public Robot {
// protected:
//     int shells = 10;
//     int fireRange = 1; // default = 1
//     bool hasThirtyShots = false;


// public:
//     ShootingRobot(string name, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(name, x, y, w, h, bf) {}
//     virtual void fire(int dx, int dy) = 0;

//     void reloadThirtyShots() { 
//         shells = 30; 
//         hasThirtyShots = true; 
//     }
//     void extendRange() { 
//         fireRange = 3; 
//     }
    
//     void useShell() {
//         if (--shells <= 0) selfDestruct();
//     }

//     virtual void selfDestruct() {
//         isAlive = false;
//         std::cout << name << " self-destructed!\n";
//     }
// };

// class ThinkingRobot : virtual public Robot {
// public:
//     ThinkingRobot(string name, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(name, x, y, w, h, bf) {}
//     virtual void think() = 0;
// };

// class GenericRobot : public MovingRobot, public ShootingRobot, 
//                     public SeeingRobot, public ThinkingRobot {
// private:
//     // Battlefield* battlefield;
//     // int shells;
//     // bool selfDestructed;
//     // int upgradeCount = 0;
//     set<string> upgradedAreas;
//     vector<string> upgradeNames;
//     template<typename T>
//     shared_ptr<T> createUpgradedBot();
//     void replaceWithCombination(const vector<string>& types);
//     mt19937 gen;

// protected:
//     int upgradeCount = 0;
//     bool hasLooked = false;
//     bool hasFired = false;
//     bool hasMoved = false;
//     vector<pair<int, int>> empty_point;
//     vector<pair<int, int>> lookGot_enemy_point;

//     Battlefield* battlefield;
//     int shells;
//     bool selfDestructed;

//     int fireDx; 
//     int fireDy;

//     void getFireTarget(int& x, int& y) const {
//         x = getX() + fireDx;
//         y = getY() + fireDy;
//     }
    
// public:
//     GenericRobot(const string& name, int x, int y, int w, int h, Battlefield* bf);

//     // Update
//     int getUpgradeCount() const override { return upgradeCount; }
//     void init_Upgrade() override { 
//         upgradeCount = 0; 
//         upgradedAreas.clear();
//         upgradeNames.clear();

//     }
//     const vector<string>& get_upgradeNames() const override{return upgradeNames;}

//     // Look
//     void add_EmptyPoint(pair<int, int> pos) override{empty_point.push_back(pos);}
//     const vector<pair<int, int>>& get_EmptyPoint() const override {return empty_point;}
//     void add_LookGotEnemyPoint(pair<int, int> pos) override{ lookGot_enemy_point.push_back(pos);}
//     const vector<pair<int, int>>& get_LookGotEnemyPoint() const override{ return lookGot_enemy_point;}
    
//     // Action methods
//     void resetTurn() {
//         hasLooked = hasFired = hasMoved = false;
//         empty_point.clear();
//         lookGot_enemy_point.clear();
//     }
//     void think() override;
//     void look(int dx, int dy) override;
//     void move(int dx, int dy) override;
//     void fire(int dx, int dy) override;
    
//     // State check methods
//     bool canLook() const { return !hasLooked; }
//     bool canFire() const { return !hasFired && shells > 0; }
//     bool canMove() const { return !hasMoved; }
    
//     int getShells() const { return shells; }
//     bool hasSelfDestructed() const { return selfDestructed;}
//     void destroy();     
//     bool shouldRespawn() const;
//     virtual void respawn(int x, int y);

//     void chooseUpgrade();
//     void chooseUpgrade(int upgradeOption);
//     // void showUpgrades() const;
    
//     int getX() const;   
//     int getY() const; 
//     string getType() const override;

//     // fire
//     void surrouding_point_TARGET(int& targetX, int& targetY){
//         int centerX = getX() ;
//         int centerY = getY() ;
//         vector<pair<int, int>> surrounding_points;
        
//         for (int dy = -1; dy <= 1; ++dy) {
//             for (int dx = -1; dx <= 1; ++dx) {
//                 int pointX = centerX + dx;
//                 int pointY = centerY + dy;       

//                 if (dx == 0 && dy == 0) continue; // Robot itself
//                 else if (pointX <= 0 || pointY <=0 || pointX > battlefield->getWidth() || pointY > battlefield->getHeight()) continue; // Out of bounds
//                 else{ surrounding_points.push_back({pointX, pointY});} // Enemy + empty points
//             }
//         }

//         uniform_int_distribution<> dis(0, surrounding_points.size() - 1);
//         int num = dis(gen);
//         targetX = surrounding_points[num].first;
//         targetY = surrounding_points[num].second;
//     }
    
//     void shot_higher_enemy(int& targetX, int& targetY, const vector<pair<int, int>>& enemy_point ){
//         int i = 0;
//         int max_i = 0;
//         int max = 0;
//         int count;
//         for (const auto& point : enemy_point){
//             auto enemy = battlefield->findRobotAt(point.first, point.second);
//             count = enemy -> getUpgradeCount();
//             if(count> max){
//                 max = count;
//                 max_i = i;
//             }
//             i++;
//         }

//         targetX = enemy_point[max_i].first;
//         targetY = enemy_point[max_i].second;
//         auto enemy = battlefield->findRobotAt(targetX, targetY );
//         if(max!=0){
//             cout << name << " found out that " << enemy->getName()
//             << " has " << max << " updates and a higher level compared to other enemies." << endl;
//         }
//     }
// };

// class HideBot : virtual public GenericRobot {
//     int hideCount;
//     bool hidden; 
// public:
//     HideBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf), 
//             hideCount(3),
//             hidden(false) {}

//     bool isHidden() const override { return hidden; } 
//     void setHidden(bool state) { hidden = state; }

//     void move(int dx, int dy) override {
//         if (hideCount > 0) {
//             hideCount--;
//             setHidden(true);
//             cout << name << " is now hidden! (" << hideCount << " hides left)" << endl;
//         } else {
//             GenericRobot::move(dx, dy);
//             setHidden(false);
//         }
//     }
    
//     string getType() const override { return "HideBot"; }
// };

// class JumpBot : virtual public GenericRobot {
//     int jumpCount;
//     int newX;
//     int newY;
// public:
//     JumpBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf),
//             jumpCount(3) {}

//     void move(int dx, int dy) override {
//         if (jumpCount > 0) {
//             while (true) {
//                 newX = 1 + rand() % battlefield->getWidth();
//                 newY = 1 + rand() % battlefield->getHeight();
                
//                 if (!battlefield->isRobotAt(newX, newY) && 
//                     (abs(newX - getX()) > 1 || abs(newY - getY()) > 1)) {
//                     break;  // Valid jump found
//                 }
//             }
//             setPosition(newX, newY);
//             jumpCount--;
//             cout << name << " jumped to (" << newX << "," << newY << ") (" 
//                 << jumpCount << " jumps left)" << endl;
//         } else {
//             GenericRobot::move(dx, dy);
//         }
//     }

//     string getType() const override { return "JumpBot"; }
// };

// class GlideBot : virtual public GenericRobot {
//     int glideCount;
//     int newX;
//     int newY;
// public:
//     GlideBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf),
//             glideCount(3) {}

//     void move(int dx, int dy) override {
//         if (glideCount > 0) {
//             int maxDistance = 0;

//             vector<pair<int, int>> directions = {
//                 {1, 0},   // Right
//                 {-1, 0},  // Left
//                 {0, 1},   // Down
//                 {0, -1}   // Up
//             };

//             for (const auto& dir : directions) {
//                 int stepX = dir.first;
//                 int stepY = dir.second;
//                 int tempX = getX();
//                 int tempY = getY();
//                 int distance = 0;

//                 while (true) {
//                     int nextX = tempX + stepX;
//                     int nextY = tempY + stepY;

//                     if (!battlefield->isInside(nextX, nextY) || battlefield->isRobotAt(nextX, nextY)) {
//                         break;
//                     }

//                     tempX = nextX;
//                     tempY = nextY;
//                     distance++;
//                 }

//                 if (distance > maxDistance) {
//                     maxDistance = distance;
//                     newX = tempX;
//                     newY = tempY;
//                 }
//             }

//             if (maxDistance > 0) {
//                 setPosition(newX, newY);
//                 glideCount--;
//                 cout << getName() << " glided to (" << newX << "," << newY << ") (" 
//                     << glideCount << " glides left)" << endl;
//             }
//         } else {
//             GenericRobot::move(dx, dy);
//         }
//     }

//     string getType() const override { return "GlideBot"; }
// };

// class RevealBot : virtual public GenericRobot {
// public:
//     RevealBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf) {}

//     string getType() const override { return "RevealBot"; }
//     void look(int dx, int dy) override{
//         hasLooked = true;

//         int centerX = getX() ;
//         int centerY = getY() ;

//         cout << name << " at (" << centerX << "," << centerY << "), LOOK around ..." <<endl; 

//         for (int dy = -1; dy <= 1; ++dy) {
//             for (int dx = -1; dx <= 1; ++dx) {

//                 int lookX = centerX + dx;
//                 int lookY = centerY + dy;

//                 string status;           

//                 // Robot itself point
//                 if (dx == 0 && dy == 0 )  continue;

//                 // Out of bounds
//                 else if (lookX <=0 ||lookY <=0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight()) continue;

//                 // Enemy robot
//                 else if (battlefield->isRobotAt(lookX, lookY)) {
//                     auto enemy = battlefield->findRobotAt(lookX, lookY);
//                     if(enemy->isHidden()){
//                         status = enemy->getName() + " Hidden enemy";
//                     }
//                     else{
//                         status = enemy->getName();
//                         lookGot_enemy_point.push_back({lookX, lookY}); 
//                     }
//                     cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
//                 }

//                 else if(battlefield->isLandmine(lookX, lookY)){
//                     status = "Landmine";
//                 }

//                 // Empty space
//                 else {
//                     status = "Empty space";
//                     empty_point.push_back({lookX, lookY}); 
//                     cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
//                 }
//             }
//         }
//     }
// };

// class ScoutBot : virtual public GenericRobot {
//     int scoutUses;

// public:
//     ScoutBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf), scoutUses(3) {}

//     void look(int dx, int dy) override {
//         if (scoutUses > 0) {
//             cout << name << " sees entire battlefield:" << endl;
//             for (int y = 1; y <= battlefield->getHeight(); ++y) {
//                 for (int x = 1; x <= battlefield->getWidth(); ++x) {
//                     if (battlefield->isRobotAt(x, y)) {
//                         auto robot = battlefield->findRobotAt(x, y);
//                         cout << "  (" << x << "," << y << "): " 
//                              << robot->getName() << endl;
//                     }
//                 }
//             }
//             scoutUses--;
//         } else {
//             GenericRobot::look(dx, dy);
//         }
//     }
//     string getType() const override { return "ScoutBot"; }
// };

// class TrackBot : virtual public GenericRobot {
//     int trackers;
//     set<shared_ptr<Robot>> trackedRobots; 
// public:
//     TrackBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf), trackers(3) {}

//     void look(int dx, int dy) override {
//         if (trackers > 0 && !lookGot_enemy_point.empty()) {
//             int idx = rand() % lookGot_enemy_point.size();
//             auto& point = lookGot_enemy_point[idx];
//             int x = point.first;
//             int y = point.second;
//             if (auto target = battlefield->findRobotAt(x, y)) {
//                 trackedRobots.insert(target);
//                 trackers--;
//                 std:: cout << name << " tracks " << target->getName() 
//                      << " (" << trackers << " trackers left)" << endl;
//             }
//         }
//         GenericRobot::look(dx, dy);
//     }
    
//     // 新增追踪显示
//     void displayTracked() {
//         for (auto& robot : trackedRobots) {
//             if (robot->alive()) {
//                 std:: cout << "  Tracking " << robot->getName()
//                      << " at (" << robot->getX() << "," << robot->getY() << ")" << endl;
//             }
//         }
//     }

//     string getType() const override { return "TrackBot"; }
// };

// class LongShotBot : virtual public GenericRobot { // 修改5: 继承GenericRobot
//     int fireRange = 3;
// public:
//     LongShotBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf) {}

//     void fire(int dx, int dy) override {
//         if (abs(dx) + abs(dy) > fireRange) {
//             std:: cout << name << " can't fire that far! (Max " << fireRange << ")" << endl;
//             return;
//         }
//         GenericRobot::fire(dx, dy); 
//     }

//     string getType() const override { return "LongShotBot"; }
// };

// class SemiAutoBot : virtual public GenericRobot {
// public:
//     SemiAutoBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf) {}

//     void fire(int dx, int dy) override {
        
//         for (int i = 0; i < 3 && canFire(); ++i) {
//             int targetX = lookGot_enemy_point[0].first;
//             int targetY = lookGot_enemy_point[0].second;

//             if (battlefield->isRobotAt(targetX, targetY)) {
//                 auto target = battlefield->findRobotAt(targetX, targetY);
//                 if (target && target->alive()) {
//                     target->destroy();
//                     std:: cout << name << " hits " << target->getName() 
//                          << " at (" << targetX << "," << targetY << ")!" << endl;
//                 }
//             } else {
//                 break;
//             }

//             if (shells == 0){
//                 break;
//             }

//             useShell();  // consume a shell
//         }

//         hasFired = true;  // mark as fired for the turn
//     }

//     string getType() const override { return "SemiAutoBot"; }
// };


// class ThirtyShotBot : virtual public GenericRobot {
// public:
//     ThirtyShotBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         :   Robot(name, x, y, w, h, bf),
//             GenericRobot(name, x, y, w, h, bf)  {
//         shells = 30; // 修改8: 直接设置弹药量
//         std:: cout << name << " loaded 30 shells!" << endl;
//     }
//     string getType() const override { return "ThirtyShotBot"; }
// };

// class LandmineBot : virtual public GenericRobot {
// public:
//     LandmineBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(name, x, y, w, h, bf),  // 显式初始化虚基类
//           GenericRobot(name, x, y, w, h, bf) {}

//     void fire(int dx, int dy) override {
//         int targetX = getX() + dx;
//         int targetY = getY() + dy;

//         // 调用基类开火逻辑
//         GenericRobot::fire(dx, dy);

//         // 如果没有击中目标
//         if (!battlefield->isRobotAt(targetX, targetY)) {
//             battlefield->addLandmine(targetX, targetY);
//             std:: cout << name << " placed landmine at (" << targetX << "," << targetY << ")\n";
//         }
//     }

//     string getType() const override { return "LandmineBot"; }
// };

// class HideLongShotBot : public HideBot, public LongShotBot {
// public:
//     HideLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLongShotBot"; }
// };

// class HideSemiAutoBot : public HideBot, public SemiAutoBot {
// public:
//     HideSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n,x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideSemiAutoBot"; }
// };

// class HideThirtyShotBot : public HideBot, public ThirtyShotBot {
// public:
//     HideThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideThirtyShotBot"; }
// };

// class HideLandmineBot : public HideBot, public LandmineBot {
// public:
//     HideLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLandmineBot"; }
// };

// class JumpLongShotBot : public JumpBot, public LongShotBot {
// public:
//     JumpLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLongShotBot"; }
// };

// class JumpSemiAutoBot : public JumpBot, public SemiAutoBot {
// public:
//     JumpSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpSemiAutoBot"; }
// };

// class JumpThirtyShotBot : public JumpBot, public ThirtyShotBot {
// public:
//     JumpThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpThirtyShotBot"; }
// };

// class JumpLandmineBot : public JumpBot, public LandmineBot {
// public:
//     JumpLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLandmineBot"; }
// };

// class GlideLongShotBot : public GlideBot, public LongShotBot {
// public:
//     GlideLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLongShotBot"; }
// };

// class GlideSemiAutoBot : public GlideBot, public SemiAutoBot {
// public:
//     GlideSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n,x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideSemiAutoBot"; }
// };

// class GlideThirtyShotBot : public GlideBot, public ThirtyShotBot {
// public:
//     GlideThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideThirtyShotBot"; }
// };

// class GlideLandmineBot : public GlideBot, public LandmineBot {
// public:
//     GlideLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLandmineBot"; }
// };

// class HideScoutBot : public HideBot, public ScoutBot {
// public:
//     HideScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideScoutBot"; }
// };

// class HideTrackBot : public HideBot, public TrackBot {
// public:
//     HideTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideTrackBot"; }
// };

// class HideRevealBot : public HideBot, public RevealBot {
// public:
//     HideRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideRevealBot"; }
// };

// class JumpScoutBot : public JumpBot, public ScoutBot {
// public:
//     JumpScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpScoutBot"; }
// };

// class JumpTrackBot : public JumpBot, public TrackBot {
// public:
//     JumpTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpTrackBot"; }
// };

// class JumpRevealBot : public JumpBot, public RevealBot {
// public:
//     JumpRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpRevealBot"; }
// };

// class GlideScoutBot : public GlideBot, public ScoutBot {
// public:
//     GlideScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideScoutBot"; }
// };

// class GlideTrackBot : public GlideBot, public TrackBot {
// public:
//     GlideTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideTrackBot"; }
// };

// class GlideRevealBot : public GlideBot, public RevealBot {
// public:
//     GlideRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideRevealBot"; }
// };

// class LongShotScoutBot : public LongShotBot, public ScoutBot {
// public:
//     LongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LongShotScoutBot"; }
// };

// class LongShotTrackBot : public LongShotBot, public TrackBot {
// public:
//     LongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LongShotTrackBot"; }
// };

// class LongShotRevealBot : public LongShotBot, public RevealBot {
// public:
//     LongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LongShotRevealBot"; }
// };

// class SemiAutoScoutBot : public SemiAutoBot, public ScoutBot {
// public:
//     SemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "SemiAutoScoutBot"; }
// };

// class SemiAutoTrackBot : public SemiAutoBot, public TrackBot {
// public:
//     SemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "SemiAutoTrackBot"; }
// };

// class SemiAutoRevealBot : public SemiAutoBot, public RevealBot {
// public:
//     SemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "SemiAutoRevealBot"; }
// };

// class ThirtyShotScoutBot : public ThirtyShotBot, public ScoutBot {
// public:
//     ThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "ThirtyShotScoutBot"; }
// };

// class ThirtyShotTrackBot : public ThirtyShotBot, public TrackBot {
// public:
//     ThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "ThirtyShotTrackBot"; }
// };

// class ThirtyShotRevealBot : public ThirtyShotBot, public RevealBot {
// public:
//     ThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "ThirtyShotRevealBot"; }
// };

// class LandmineScoutBot : public LandmineBot, public ScoutBot {
// public:
//     LandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LandmineScoutBot"; }
// };

// class LandmineTrackBot : public LandmineBot, public TrackBot {
// public:
//     LandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LandmineTrackBot"; }
// };

// class LandmineRevealBot : public LandmineBot, public RevealBot {
// public:
//     LandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "LandmineRevealBot"; }
// };

// class HideLongShotScoutBot : public HideBot, public LongShotBot, public ScoutBot {
// public:
//     HideLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLongShotScoutBot"; }
// };

// class HideLongShotTrackBot : public HideBot, public LongShotBot, public TrackBot {
// public:
//     HideLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLongShotTrackBot"; }
// };

// class HideLongShotRevealBot : public HideBot, public LongShotBot, public RevealBot {
// public:
//     HideLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLongShotRevealBot"; }
// };

// class HideSemiAutoScoutBot : public HideBot, public SemiAutoBot, public ScoutBot {
// public:
//     HideSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideSemiAutoScoutBot"; }
// };

// class HideSemiAutoTrackBot : public HideBot, public SemiAutoBot, public TrackBot {
// public:
//     HideSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideSemiAutoTrackBot"; }
// };

// class HideSemiAutoRevealBot : public HideBot, public SemiAutoBot, public RevealBot {
// public:
//     HideSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideSemiAutoRevealBot"; }
// };

// class HideThirtyShotScoutBot : public HideBot, public ThirtyShotBot, public ScoutBot {
// public:
//     HideThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideThirtyShotScoutBot"; }
// };

// class HideThirtyShotTrackBot : public HideBot, public ThirtyShotBot, public TrackBot {
// public:
//     HideThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideThirtyShotTrackBot"; }
// };

// class HideThirtyShotRevealBot : public HideBot, public ThirtyShotBot, public RevealBot {
// public:
//     HideThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideThirtyShotRevealBot"; }
// };

// class HideLandmineScoutBot : public HideBot, public LandmineBot, public ScoutBot {
// public:
//     HideLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLandmineScoutBot"; }
// };

// class HideLandmineTrackBot : public HideBot, public LandmineBot, public TrackBot {
// public:
//     HideLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLandmineTrackBot"; }
// };

// class HideLandmineRevealBot : public HideBot, public LandmineBot, public RevealBot {
// public:
//     HideLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "HideLandmineRevealBot"; }
// };

// class JumpLongShotScoutBot : public JumpBot, public LongShotBot, public ScoutBot {
// public:
//     JumpLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLongShotScoutBot"; }
// };

// class JumpLongShotTrackBot : public JumpBot, public LongShotBot, public TrackBot {
// public:
//     JumpLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLongShotTrackBot"; }
// };

// class JumpLongShotRevealBot : public JumpBot, public LongShotBot, public RevealBot {
// public:
//     JumpLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLongShotRevealBot"; }
// };

// class JumpSemiAutoScoutBot : public JumpBot, public SemiAutoBot, public ScoutBot {
// public:
//     JumpSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpSemiAutoScoutBot"; }
// };

// class JumpSemiAutoTrackBot : public JumpBot, public SemiAutoBot, public TrackBot {
// public:
//     JumpSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpSemiAutoTrackBot"; }
// };

// class JumpSemiAutoRevealBot : public JumpBot, public SemiAutoBot, public RevealBot {
// public:
//     JumpSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpSemiAutoRevealBot"; }
// };

// class JumpThirtyShotScoutBot : public JumpBot, public ThirtyShotBot, public ScoutBot {
// public:
//     JumpThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpThirtyShotScoutBot"; }
// };

// class JumpThirtyShotTrackBot : public JumpBot, public ThirtyShotBot, public TrackBot {
// public:
//     JumpThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpThirtyShotTrackBot"; }
// };

// class JumpThirtyShotRevealBot : public JumpBot, public ThirtyShotBot, public RevealBot {
// public:
//     JumpThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpThirtyShotRevealBot"; }
// };

// class JumpLandmineScoutBot : public JumpBot, public LandmineBot, public ScoutBot {
// public:
//     JumpLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLandmineScoutBot"; }
// };

// class JumpLandmineTrackBot : public JumpBot, public LandmineBot, public TrackBot {
// public:
//     JumpLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLandmineTrackBot"; }
// };

// class JumpLandmineRevealBot : public JumpBot, public LandmineBot, public RevealBot {
// public:
//     JumpLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "JumpLandmineRevealBot"; }
// };

// class GlideLongShotScoutBot : public GlideBot, public LongShotBot, public ScoutBot {
// public:
//     GlideLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLongShotScoutBot"; }
// };

// class GlideLongShotTrackBot : public GlideBot, public LongShotBot, public TrackBot {
// public:
//     GlideLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLongShotTrackBot"; }
// };

// class GlideLongShotRevealBot : public GlideBot, public LongShotBot, public RevealBot {
// public:
//     GlideLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLongShotRevealBot"; }
// };

// class GlideSemiAutoScoutBot : public GlideBot, public SemiAutoBot, public ScoutBot {
// public:
//     GlideSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideSemiAutoScoutBot"; }
// };

// class GlideSemiAutoTrackBot : public GlideBot, public SemiAutoBot, public TrackBot {
// public:
//     GlideSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideSemiAutoTrackBot"; }
// };

// class GlideSemiAutoRevealBot : public GlideBot, public SemiAutoBot, public RevealBot {
// public:
//     GlideSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideSemiAutoRevealBot"; }
// };

// class GlideThirtyShotScoutBot : public GlideBot, public ThirtyShotBot, public ScoutBot {
// public:
//     GlideThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideThirtyShotScoutBot"; }
// };

// class GlideThirtyShotTrackBot : public GlideBot, public ThirtyShotBot, public TrackBot {
// public:
//     GlideThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideThirtyShotTrackBot"; }
// };

// class GlideThirtyShotRevealBot : public GlideBot, public ThirtyShotBot, public RevealBot {
// public:
//     GlideThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideThirtyShotRevealBot"; }
// };

// class GlideLandmineScoutBot : public GlideBot, public LandmineBot, public ScoutBot {
// public:
//     GlideLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLandmineScoutBot"; }
// };

// class GlideLandmineTrackBot : public GlideBot, public LandmineBot, public TrackBot {
// public:
//     GlideLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLandmineTrackBot"; }
// };

// class GlideLandmineRevealBot : public GlideBot, public LandmineBot, public RevealBot {
// public:
//     GlideLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
//         : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
//     string getType() const override { return "GlideLandmineRevealBot"; }
// };

// #endif