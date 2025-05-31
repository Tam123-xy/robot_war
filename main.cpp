/**********|**********|**********|
Program: main.cpp / robots.h / battlefield.h
Course: OOPDS
Trimester: 2520
Name: TAY SHI XIANG | YIAP WEI SHANZ | TAM XIN YI
ID: 243UC247GE | 243UC247CV | 243UC247G6
Lecture Section: TC1L
Tutorial Section: TT2L
Email: TAY.SHI.XIANG@student.mmu.edu.my | YIAP.WEI.SHANZ@student.mmu.edu.my | TAM.XIN.YI@student.mmu.edu.my
Phone: 019-3285968 | 011-59964357 | 011-11026051
**********|**********|**********/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime> 

#include <string>
#include <memory>
#include <utility>
#include <set>
#include <mutex>
#include <queue>
#include <algorithm>
using namespace std;
ofstream logFile("output_log.txt");

class Battlefield; 
class Robot;

class Battlefield {
private:
    int width, height;
    vector<shared_ptr<Robot>> robots;
    queue<shared_ptr<Robot>> respawnQueue;
    random_device rd;
    mt19937 gen;
    uniform_int_distribution<> xDist;
    uniform_int_distribution<> yDist;
    mutex respawnMutex;
    set<pair<int, int>> landmines;
    
public:
    Battlefield(int w, int h);
    int getWidth() const;
    int getHeight() const;
    bool isRobotAt(int x, int y) const;
    void addRobot(shared_ptr<Robot> robot);
    bool isEmpty() const;
    void simulateTurn();
    void display();
    bool isEnemyAt(int x, int y) const;
    int countAliveRobots() const;
    Robot* getAliveRobot() const;
    shared_ptr<Robot> findRobotAt(int x, int y);
    int countLiveRobot() const;
    void processRespawn();
    void addToRespawn(shared_ptr<Robot> robot);

    // Action sequence methods
    void executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy);
    void replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot);
    bool checkAttackHit(shared_ptr<Robot> attacker, shared_ptr<Robot> target);
    void addLandmine(int x, int y);
    bool checkLandmine(int x, int y) ;
    bool isLandmine(int x, int y) ;

     //GlideBot
    bool isInside(int x, int y) const { return x >= 1 && x <= width && y >= 1 && y <= height; }
};

class Robot : public std::enable_shared_from_this<Robot>{
protected:
    Battlefield* battlefield;
    string name;
    int lives = 3;
    bool isAlive = true;
    bool inRespawnQueue = false;
    int positionX, positionY; // Changed from private to protected
    int width, height;

    int shells = 10;
    int fireRange = 1;
    bool selfDestructed = false;
   
public:
   Robot(string name, int x, int y, int w, int h, Battlefield* bf)  
        : name(name), positionX(x), positionY(y), width(w), height(h), 
          battlefield(bf) {}

    virtual ~Robot() = default;

    int getX() const { return positionX; }
    int getY() const { return positionY; }

    virtual string getType() const = 0;
    Robot() = delete;

    void setLives(int l) { lives = l; } 
    virtual bool isHidden() const { return false; } 

    virtual void resetTurn() = 0;
    virtual void think() = 0;
    virtual void look(int dx, int dy) = 0;
    virtual void fire(int dx, int dy) = 0;
    virtual void move(int dx, int dy) = 0;

    void setPosition(int x, int y) { positionX = x; positionY = y; }
    bool alive() const { return isAlive; }
    bool isNearby;
    void destroy();
    virtual void respawn(int x, int y);
    bool shouldRespawn() const;
    string getName() const { return name; }
    int getLives() const { return lives; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void setShells(int s) { shells = s; }
    virtual bool hasSelfDestructed() const = 0;
    virtual int getShells() const = 0;

    virtual void add_EmptyPoint(pair<int, int> pos) =0;
    virtual const vector<pair<int, int>>& get_EmptyPoint() const =0;
    virtual void add_LookGotEnemyPoint(pair<int, int> pos) =0;
    virtual const vector<pair<int, int>>& get_LookGotEnemyPoint() const =0;
    
    // Update
    virtual int getUpgradeCount() const { return 0; }
    virtual void init_Upgrade() {}
    virtual const vector<string>& get_upgradeNames() const = 0;
};

class MovingRobot : virtual public Robot {
public:
    MovingRobot(string name, int x, int y, int w, int h, Battlefield* bf) 
        : Robot(name, x, y, w, h, bf) {}
    virtual ~MovingRobot() = default;

    //Basic Movement
    virtual void move(int dx, int dy) = 0;
    
    //Movement validation
    bool isValidMove(int dx, int dy) const {
        int newX = positionX + dx;
        int newY = positionY + dy;
        return (newX > 0 && newX < width && newY > 0 && newY < height);
    }
};

class SeeingRobot : virtual public Robot {
public:
    SeeingRobot(string name, int x, int y, int w, int h, Battlefield* bf) 
            : Robot(name, x, y, w, h, bf) {}
    virtual ~SeeingRobot() = default;

    using Robot::Robot;
    virtual void look(int dx, int dy) = 0;

};

class ShootingRobot : virtual public Robot {
protected:
    int shells = 10;
    int max_shells = 10;
    int fireRange = 1; // default = 1
    bool hasThirtyShots = false;


public:
    ShootingRobot(string name, int x, int y, int w, int h, Battlefield* bf)
        : Robot(name, x, y, w, h, bf) {}
    virtual void fire(int dx, int dy) = 0;

    void reloadThirtyShots() { 
        shells = 30; 
        hasThirtyShots = true; 
    }
    void extendRange() { 
        fireRange = 3; 
    }
    
    void useShell() {
        if (--shells <= 0) selfDestruct();
    }

    virtual void selfDestruct() {
        isAlive = false;
        std::cout << name << " self-destructed!\n";
        logFile << name << " self-destructed!\n";
    }
};

class ThinkingRobot : virtual public Robot {
public:
    ThinkingRobot(string name, int x, int y, int w, int h, Battlefield* bf)
        : Robot(name, x, y, w, h, bf) {}
    virtual void think() = 0;
};

class GenericRobot : public MovingRobot, public ShootingRobot, 
                    public SeeingRobot, public ThinkingRobot {
private:
    set<string> upgradedAreas;
    vector<string> upgradeNames;
    template<typename T>
    shared_ptr<T> createUpgradedBot();
    void replaceWithCombination(const vector<string>& types);
    mt19937 gen;

protected:
    int upgradeCount = 0;
    bool hasLooked = false;
    bool hasFired = false;
    bool hasMoved = false;
    vector<pair<int, int>> empty_point;
    vector<pair<int, int>> lookGot_enemy_point;

    Battlefield* battlefield;
    int shells;
    bool selfDestructed;

    int fireDx; 
    int fireDy;

    void getFireTarget(int& x, int& y) const {
        x = getX() + fireDx;
        y = getY() + fireDy;
    }
    
public:
    GenericRobot(const string& name, int x, int y, int w, int h, Battlefield* bf);

    // Update
    int getUpgradeCount() const override { return upgradeCount; }
    void init_Upgrade() override { 
        upgradeCount = 0; 
        upgradedAreas.clear();
        upgradeNames.clear();

    }
    const vector<string>& get_upgradeNames() const override{return upgradeNames;}

    // Look
    void add_EmptyPoint(pair<int, int> pos) override{empty_point.push_back(pos);}
    const vector<pair<int, int>>& get_EmptyPoint() const override {return empty_point;}
    void add_LookGotEnemyPoint(pair<int, int> pos) override{ lookGot_enemy_point.push_back(pos);}
    const vector<pair<int, int>>& get_LookGotEnemyPoint() const override{ return lookGot_enemy_point;}
    
    // Action methods
    void resetTurn() {
        hasLooked = hasFired = hasMoved = false;
        empty_point.clear();
        lookGot_enemy_point.clear();
    }
    void think() override;
    void look(int dx, int dy) override;
    void move(int dx, int dy) override;
    void fire(int dx, int dy) override;
    
    // State check methods
    bool canLook() const { return !hasLooked; }
    bool canFire() const { return !hasFired && shells > 0; }
    bool canMove() const { return !hasMoved; }
    
    int getShells() const { return shells; }
    int getMaxShells() const { return max_shells; }
    bool hasSelfDestructed() const { return selfDestructed;}
    void destroy();     
    bool shouldRespawn() const;
    virtual void respawn(int x, int y);

    void chooseUpgrade();
    void chooseUpgrade(int upgradeOption);
    // void showUpgrades() const;
    
    int getX() const;   
    int getY() const; 
    string getType() const override;

    // fire
    void surrouding_point_TARGET(int& targetX, int& targetY){
        int centerX = getX() ;
        int centerY = getY() ;
        vector<pair<int, int>> surrounding_points;
        
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int pointX = centerX + dx;
                int pointY = centerY + dy;       

                if (dx == 0 && dy == 0) continue; // Robot itself
                else if (pointX <= 0 || pointY <=0 || pointX > battlefield->getWidth() || pointY > battlefield->getHeight()) continue; // Out of bounds
                else{ surrounding_points.push_back({pointX, pointY});} // Enemy + empty points
            }
        }

        uniform_int_distribution<> dis(0, surrounding_points.size() - 1);
        int num = dis(gen);
        targetX = surrounding_points[num].first;
        targetY = surrounding_points[num].second;
    }
    
    void shot_higher_enemy(int& targetX, int& targetY, const vector<pair<int, int>>& enemy_point ){
        int i = 0;
        int max_i = 0;
        int max = 0;
        int count;
        for (const auto& point : enemy_point){
            auto enemy = battlefield->findRobotAt(point.first, point.second);
            count = enemy -> getUpgradeCount();
            if(count> max){
                max = count;
                max_i = i;
            }
            i++;
        }

        targetX = enemy_point[max_i].first;
        targetY = enemy_point[max_i].second;
        auto enemy = battlefield->findRobotAt(targetX, targetY );
        if(max!=0){
            cout << name << " found out that " << enemy->getName()
            << " has " << max << " updates and a higher level compared to other enemies." << endl;

            logFile << name << " found out that " << enemy->getName()
            << " has " << max << " updates and a higher level compared to other enemies." << endl;
        }
    }
};

class HideBot : virtual public GenericRobot {
    int hideCount;
    bool hidden; 
public:
    HideBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf), 
            hideCount(3),
            hidden(false) {}

    bool isHidden() const override { return hidden; } 
    void setHidden(bool state) { hidden = state; }

    void move(int dx, int dy) override {
        if (hideCount > 0) {
            hideCount--;
            setHidden(true);
            cout << name << " is now hidden! (" << hideCount << " hides left)" << endl;
            logFile << name << " is now hidden! (" << hideCount << " hides left)" << endl;
        } else {
            GenericRobot::move(dx, dy);
            setHidden(false);
        }
    }
    
    string getType() const override { return "HideBot"; }
};

class JumpBot : virtual public GenericRobot {
    int jumpCount;
    int newX;
    int newY;
public:
    JumpBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf),
            jumpCount(3) {}

    void move(int dx, int dy) override {
        if (jumpCount > 0) {
            while (true) {
                newX = 1 + rand() % battlefield->getWidth();
                newY = 1 + rand() % battlefield->getHeight();
                
                if (!battlefield->isRobotAt(newX, newY) && 
                    (abs(newX - getX()) > 1 || abs(newY - getY()) > 1)) {
                    break;  // Valid jump found
                }
            }
            setPosition(newX, newY);
            jumpCount--;
            cout << name << " jumped to (" << newX << "," << newY << ") (" 
                << jumpCount << " jumps left)" << endl;

            logFile << name << " jumped to (" << newX << "," << newY << ") (" 
                << jumpCount << " jumps left)" << endl;
        } else {
            GenericRobot::move(dx, dy);
        }
    }

    string getType() const override { return "JumpBot"; }
};

class GlideBot : virtual public GenericRobot {
    int glideCount;
    int newX;
    int newY;
public:
    GlideBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf),
            glideCount(3) {}

    void move(int dx, int dy) override {
        if (glideCount > 0) {
            int maxDistance = 0;

            vector<pair<int, int>> directions = {
                {1, 0},   // Right
                {-1, 0},  // Left
                {0, 1},   // Down
                {0, -1}   // Up
            };

            for (const auto& dir : directions) {
                int stepX = dir.first;
                int stepY = dir.second;
                int tempX = getX();
                int tempY = getY();
                int distance = 0;

                while (true) {
                    int nextX = tempX + stepX;
                    int nextY = tempY + stepY;

                    if (!battlefield->isInside(nextX, nextY) || battlefield->isRobotAt(nextX, nextY)) {
                        break;
                    }

                    tempX = nextX;
                    tempY = nextY;
                    distance++;
                }

                if (distance > maxDistance) {
                    maxDistance = distance;
                    newX = tempX;
                    newY = tempY;
                }
            }

            if (maxDistance > 0) {
                setPosition(newX, newY);
                glideCount--;
                cout << getName() << " glided to (" << newX << "," << newY << ") (" 
                    << glideCount << " glides left)" << endl;
                
                logFile << getName() << " glided to (" << newX << "," << newY << ") (" 
                    << glideCount << " glides left)" << endl;
            }
        } else {
            GenericRobot::move(dx, dy);
        }
    }

    string getType() const override { return "GlideBot"; }
};

class RevealBot : virtual public GenericRobot {
public:
    RevealBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf) {}

    void look(int dx, int dy) override{
        hasLooked = true;

        int centerX = getX() ;
        int centerY = getY() ;

        cout << name << " is now at (" << centerX << "," << centerY << "), looking around ..." <<endl; 
        logFile << name << " is now at (" << centerX << "," << centerY << "), looking around ..." <<endl;

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
                    if(enemy->isHidden()){
                        status = enemy->getName() + " Hidden enemy";
                    }
                    else{
                        status = enemy->getName();
                        lookGot_enemy_point.push_back({lookX, lookY}); 
                    }
                    cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
                    logFile << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
                }

                else if(battlefield->isLandmine(lookX, lookY)){
                    status = "Landmine";
                }

                // Empty space
                else {
                    status = "Empty space";
                    empty_point.push_back({lookX, lookY}); 
                    cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
                    logFile << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl ;
                }
            }
        }
    }
    string getType() const override { return "RevealBot"; }
};

class ScoutBot : virtual public GenericRobot {
    int scoutUses;

public:
    ScoutBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf), scoutUses(3) {}

    void look(int dx, int dy) override {
        if (scoutUses > 0) {
            cout << name << " sees entire battlefield:" << endl;
            logFile << name << " sees entire battlefield:" << endl;
            for (int y = 1; y <= battlefield->getHeight(); ++y) {
                for (int x = 1; x <= battlefield->getWidth(); ++x) {
                    if (battlefield->isRobotAt(x, y)) {
                        auto robot = battlefield->findRobotAt(x, y);
                        cout << "  (" << x << "," << y << "): " << robot->getName() << endl;
                        logFile << "  (" << x << "," << y << "): " << robot->getName() << endl;
                    }
                }
            }
            scoutUses--;
        } else {
            GenericRobot::look(dx, dy);
        }
    }
    string getType() const override { return "ScoutBot"; }
};

class TrackBot : virtual public GenericRobot {
    int trackers;
    set<shared_ptr<Robot>> trackedRobots; 
public:
    TrackBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf), trackers(3) {}

    void look(int dx, int dy) override {
        if (trackers > 0 && !lookGot_enemy_point.empty()) {
            int idx = rand() % lookGot_enemy_point.size();
            auto& point = lookGot_enemy_point[idx];
            int x = point.first;
            int y = point.second;
            if (auto target = battlefield->findRobotAt(x, y)) {
                trackedRobots.insert(target);
                trackers--;
                std:: cout << name << " tracks " << target->getName() 
                     << " (" << trackers << " trackers left)" << endl;

                logFile << name << " tracks " << target->getName() 
                     << " (" << trackers << " trackers left)" << endl;
            }
        }
        GenericRobot::look(dx, dy);
    }
    
    void displayTracked() {
        for (auto& robot : trackedRobots) {
            if (robot->alive()) {
                std:: cout << "  Tracking " << robot->getName()
                     << " at (" << robot->getX() << "," << robot->getY() << ")" << endl;
                logFile << "  Tracking " << robot->getName()
                     << " at (" << robot->getX() << "," << robot->getY() << ")" << endl;
            }
        }
    }

    string getType() const override { return "TrackBot"; }
};

class LongShotBot : virtual public GenericRobot { // 修改5: 继承GenericRobot
    int fireRange = 3;
public:
    LongShotBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf) {}

    void fire(int dx, int dy) override {
        if (abs(dx) + abs(dy) > fireRange) {
            std:: cout << name << " can't fire that far! (Max " << fireRange << ")" << endl;
            logFile << name << " can't fire that far! (Max " << fireRange << ")" << endl;
            return;
        }
        GenericRobot::fire(dx, dy); 
    }

    string getType() const override { return "LongShotBot"; }
};

class SemiAutoBot : virtual public GenericRobot {
public:
    SemiAutoBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf) {}

    void fire(int dx, int dy) override {
        
        for (int i = 0; i < 3 && canFire(); ++i) {
            GenericRobot::fire(dx, dy);
        }

        hasFired = true;  // mark as fired for the turn
    }

    string getType() const override { return "SemiAutoBot"; }
};


class ThirtyShotBot : virtual public GenericRobot {
public:
    ThirtyShotBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf)  {
        max_shells = 30;
        shells = 30;
        std:: cout << name << " loaded 30 shells!" << endl;
        logFile << name << " loaded 30 shells!" << endl;
    }
    string getType() const override { return "ThirtyShotBot"; }
};

class LandmineBot : virtual public GenericRobot {
public:
    LandmineBot(const string& name, int x, int y, int w, int h, Battlefield* bf)
        : Robot(name, x, y, w, h, bf),  // 显式初始化虚基类
          GenericRobot(name, x, y, w, h, bf) {}

    void fire(int dx, int dy) override {
        int targetX = getX() + dx;
        int targetY = getY() + dy;

        GenericRobot::fire(dx, dy);
        
        if (!battlefield->isRobotAt(targetX, targetY)) {
            battlefield->addLandmine(targetX, targetY);
            std:: cout << name << " placed landmine at (" << targetX << "," << targetY << ")\n";
            logFile << name << " placed landmine at (" << targetX << "," << targetY << ")\n";
        }
    }

    string getType() const override { return "LandmineBot"; }
};

class HideLongShotBot : public HideBot, public LongShotBot {
public:
    HideLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLongShotBot"; }
};

class HideSemiAutoBot : public HideBot, public SemiAutoBot {
public:
    HideSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n,x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideSemiAutoBot"; }
};

class HideThirtyShotBot : public HideBot, public ThirtyShotBot {
public:
    HideThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideThirtyShotBot"; }
};

class HideLandmineBot : public HideBot, public LandmineBot {
public:
    HideLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLandmineBot"; }
};

class JumpLongShotBot : public JumpBot, public LongShotBot {
public:
    JumpLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLongShotBot"; }
};

class JumpSemiAutoBot : public JumpBot, public SemiAutoBot {
public:
    JumpSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpSemiAutoBot"; }
};

class JumpThirtyShotBot : public JumpBot, public ThirtyShotBot {
public:
    JumpThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpThirtyShotBot"; }
};

class JumpLandmineBot : public JumpBot, public LandmineBot {
public:
    JumpLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLandmineBot"; }
};

class GlideLongShotBot : public GlideBot, public LongShotBot {
public:
    GlideLongShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLongShotBot"; }
};

class GlideSemiAutoBot : public GlideBot, public SemiAutoBot {
public:
    GlideSemiAutoBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n,x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideSemiAutoBot"; }
};

class GlideThirtyShotBot : public GlideBot, public ThirtyShotBot {
public:
    GlideThirtyShotBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideThirtyShotBot"; }
};

class GlideLandmineBot : public GlideBot, public LandmineBot {
public:
    GlideLandmineBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLandmineBot"; }
};

class HideScoutBot : public HideBot, public ScoutBot {
public:
    HideScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideScoutBot"; }
};

class HideTrackBot : public HideBot, public TrackBot {
public:
    HideTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideTrackBot"; }
};

class HideRevealBot : public HideBot, public RevealBot {
public:
    HideRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideRevealBot"; }
};

class JumpScoutBot : public JumpBot, public ScoutBot {
public:
    JumpScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpScoutBot"; }
};

class JumpTrackBot : public JumpBot, public TrackBot {
public:
    JumpTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpTrackBot"; }
};

class JumpRevealBot : public JumpBot, public RevealBot {
public:
    JumpRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpRevealBot"; }
};

class GlideScoutBot : public GlideBot, public ScoutBot {
public:
    GlideScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideScoutBot"; }
};

class GlideTrackBot : public GlideBot, public TrackBot {
public:
    GlideTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideTrackBot"; }
};

class GlideRevealBot : public GlideBot, public RevealBot {
public:
    GlideRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideRevealBot"; }
};

class LongShotScoutBot : public LongShotBot, public ScoutBot {
public:
    LongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LongShotScoutBot"; }
};

class LongShotTrackBot : public LongShotBot, public TrackBot {
public:
    LongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LongShotTrackBot"; }
};

class LongShotRevealBot : public LongShotBot, public RevealBot {
public:
    LongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LongShotRevealBot"; }
};

class SemiAutoScoutBot : public SemiAutoBot, public ScoutBot {
public:
    SemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "SemiAutoScoutBot"; }
};

class SemiAutoTrackBot : public SemiAutoBot, public TrackBot {
public:
    SemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "SemiAutoTrackBot"; }
};

class SemiAutoRevealBot : public SemiAutoBot, public RevealBot {
public:
    SemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "SemiAutoRevealBot"; }
};

class ThirtyShotScoutBot : public ThirtyShotBot, public ScoutBot {
public:
    ThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "ThirtyShotScoutBot"; }
};

class ThirtyShotTrackBot : public ThirtyShotBot, public TrackBot {
public:
    ThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "ThirtyShotTrackBot"; }
};

class ThirtyShotRevealBot : public ThirtyShotBot, public RevealBot {
public:
    ThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "ThirtyShotRevealBot"; }
};

class LandmineScoutBot : public LandmineBot, public ScoutBot {
public:
    LandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LandmineScoutBot"; }
};

class LandmineTrackBot : public LandmineBot, public TrackBot {
public:
    LandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LandmineTrackBot"; }
};

class LandmineRevealBot : public LandmineBot, public RevealBot {
public:
    LandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "LandmineRevealBot"; }
};

class HideLongShotScoutBot : public HideBot, public LongShotBot, public ScoutBot {
public:
    HideLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLongShotScoutBot"; }
};

class HideLongShotTrackBot : public HideBot, public LongShotBot, public TrackBot {
public:
    HideLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLongShotTrackBot"; }
};

class HideLongShotRevealBot : public HideBot, public LongShotBot, public RevealBot {
public:
    HideLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLongShotRevealBot"; }
};

class HideSemiAutoScoutBot : public HideBot, public SemiAutoBot, public ScoutBot {
public:
    HideSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideSemiAutoScoutBot"; }
};

class HideSemiAutoTrackBot : public HideBot, public SemiAutoBot, public TrackBot {
public:
    HideSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideSemiAutoTrackBot"; }
};

class HideSemiAutoRevealBot : public HideBot, public SemiAutoBot, public RevealBot {
public:
    HideSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideSemiAutoRevealBot"; }
};

class HideThirtyShotScoutBot : public HideBot, public ThirtyShotBot, public ScoutBot {
public:
    HideThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideThirtyShotScoutBot"; }
};

class HideThirtyShotTrackBot : public HideBot, public ThirtyShotBot, public TrackBot {
public:
    HideThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideThirtyShotTrackBot"; }
};

class HideThirtyShotRevealBot : public HideBot, public ThirtyShotBot, public RevealBot {
public:
    HideThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideThirtyShotRevealBot"; }
};

class HideLandmineScoutBot : public HideBot, public LandmineBot, public ScoutBot {
public:
    HideLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLandmineScoutBot"; }
};

class HideLandmineTrackBot : public HideBot, public LandmineBot, public TrackBot {
public:
    HideLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLandmineTrackBot"; }
};

class HideLandmineRevealBot : public HideBot, public LandmineBot, public RevealBot {
public:
    HideLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), HideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "HideLandmineRevealBot"; }
};

class JumpLongShotScoutBot : public JumpBot, public LongShotBot, public ScoutBot {
public:
    JumpLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLongShotScoutBot"; }
};

class JumpLongShotTrackBot : public JumpBot, public LongShotBot, public TrackBot {
public:
    JumpLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLongShotTrackBot"; }
};

class JumpLongShotRevealBot : public JumpBot, public LongShotBot, public RevealBot {
public:
    JumpLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLongShotRevealBot"; }
};

class JumpSemiAutoScoutBot : public JumpBot, public SemiAutoBot, public ScoutBot {
public:
    JumpSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpSemiAutoScoutBot"; }
};

class JumpSemiAutoTrackBot : public JumpBot, public SemiAutoBot, public TrackBot {
public:
    JumpSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpSemiAutoTrackBot"; }
};

class JumpSemiAutoRevealBot : public JumpBot, public SemiAutoBot, public RevealBot {
public:
    JumpSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpSemiAutoRevealBot"; }
};

class JumpThirtyShotScoutBot : public JumpBot, public ThirtyShotBot, public ScoutBot {
public:
    JumpThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpThirtyShotScoutBot"; }
};

class JumpThirtyShotTrackBot : public JumpBot, public ThirtyShotBot, public TrackBot {
public:
    JumpThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpThirtyShotTrackBot"; }
};

class JumpThirtyShotRevealBot : public JumpBot, public ThirtyShotBot, public RevealBot {
public:
    JumpThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpThirtyShotRevealBot"; }
};

class JumpLandmineScoutBot : public JumpBot, public LandmineBot, public ScoutBot {
public:
    JumpLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLandmineScoutBot"; }
};

class JumpLandmineTrackBot : public JumpBot, public LandmineBot, public TrackBot {
public:
    JumpLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLandmineTrackBot"; }
};

class JumpLandmineRevealBot : public JumpBot, public LandmineBot, public RevealBot {
public:
    JumpLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), JumpBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "JumpLandmineRevealBot"; }
};

class GlideLongShotScoutBot : public GlideBot, public LongShotBot, public ScoutBot {
public:
    GlideLongShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLongShotScoutBot"; }
};

class GlideLongShotTrackBot : public GlideBot, public LongShotBot, public TrackBot {
public:
    GlideLongShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLongShotTrackBot"; }
};

class GlideLongShotRevealBot : public GlideBot, public LongShotBot, public RevealBot {
public:
    GlideLongShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LongShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLongShotRevealBot"; }
};

class GlideSemiAutoScoutBot : public GlideBot, public SemiAutoBot, public ScoutBot {
public:
    GlideSemiAutoScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideSemiAutoScoutBot"; }
};

class GlideSemiAutoTrackBot : public GlideBot, public SemiAutoBot, public TrackBot {
public:
    GlideSemiAutoTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideSemiAutoTrackBot"; }
};

class GlideSemiAutoRevealBot : public GlideBot, public SemiAutoBot, public RevealBot {
public:
    GlideSemiAutoRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), SemiAutoBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideSemiAutoRevealBot"; }
};

class GlideThirtyShotScoutBot : public GlideBot, public ThirtyShotBot, public ScoutBot {
public:
    GlideThirtyShotScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideThirtyShotScoutBot"; }
};

class GlideThirtyShotTrackBot : public GlideBot, public ThirtyShotBot, public TrackBot {
public:
    GlideThirtyShotTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideThirtyShotTrackBot"; }
};

class GlideThirtyShotRevealBot : public GlideBot, public ThirtyShotBot, public RevealBot {
public:
    GlideThirtyShotRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), ThirtyShotBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideThirtyShotRevealBot"; }
};

class GlideLandmineScoutBot : public GlideBot, public LandmineBot, public ScoutBot {
public:
    GlideLandmineScoutBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), ScoutBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLandmineScoutBot"; }
};

class GlideLandmineTrackBot : public GlideBot, public LandmineBot, public TrackBot {
public:
    GlideLandmineTrackBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), TrackBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLandmineTrackBot"; }
};

class GlideLandmineRevealBot : public GlideBot, public LandmineBot, public RevealBot {
public:
    GlideLandmineRevealBot(string n, int x, int y, int w, int h, Battlefield* bf)
        : Robot(n, x, y, w, h, bf), GenericRobot(n, x, y, w, h, bf), GlideBot(n, x, y, w, h, bf), LandmineBot(n, x, y, w, h, bf), RevealBot(n, x, y, w, h, bf) {}
    string getType() const override { return "GlideLandmineRevealBot"; }
};



// robots.cpp
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
        return newBot;
    }

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

void GenericRobot::think()
{
    cout << name << " is thinking... " << endl;
    logFile << name << " is thinking... " << endl;
}

void GenericRobot::look(int dx, int dy)
{
    hasLooked = true;

    int centerX = getX();
    int centerY = getY();

    cout << name << " is now at (" << centerX << "," << centerY << "), looking around ..." << endl;
    logFile << name << " is now at (" << centerX << "," << centerY << "), looking around ..." << endl;

    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {

            int lookX = centerX + dx;
            int lookY = centerY + dy;

            string status;

            // Robot itself point
            if (dx == 0 && dy == 0)
                continue;

            // Out of bounds
            else if (lookX <= 0 || lookY <= 0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight())
                continue;

            // Enemy robot
            else if (battlefield->isRobotAt(lookX, lookY))
            {
                auto enemy = battlefield->findRobotAt(lookX, lookY);
                status = enemy->getName();
                lookGot_enemy_point.push_back({lookX, lookY});
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
                logFile << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
            }

            // Empty space
            else
            {
                status = "Empty space";
                empty_point.push_back({lookX, lookY});
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
                logFile << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
            }
        }
    }
}

void Robot::destroy()
{
    if (isAlive)
    {
        lives--;

        isAlive = false;
        setPosition(0, 0); // Move to outside battle field

        if (lives > 0)
        {
            cout << name << " is waiting to respawn (Lives remaining: " << lives << "/3)" << endl;
            logFile << name << " is waiting to respawn (Lives remaining: " << lives << "/3)" << endl;
            battlefield->addToRespawn(shared_from_this());
        }
        else
        {
            cout << name << " has no lives remaining! (Lives remaining: " << lives << "/3)" << endl;
            logFile << name << " has no lives remaining! (Lives remaining: " << lives << "/3)" << endl;
        }
    }
}

void Robot::respawn(int x, int y)
{
    if (lives > 0)
    {
        positionX = x;
        positionY = y;
        isAlive = true;
        init_Upgrade();
        cout << name << " respawned at (" << x << "," << y << ") (Lives remaining: " << lives << "/3)" << endl;
        logFile << name << " respawned at (" << x << "," << y << ") (Lives remaining: " << lives << "/3)" << endl;
    }
}

bool Robot::shouldRespawn() const
{
    return !isAlive && lives > 0;
}

string GenericRobot::getType() const
{
    return "GenericRobot";
}

void GenericRobot::move(int dx, int dy)
{
    hasMoved = true;
    int newX, newY;

    // move -> look, surrounding POINTS --> (is occupied/ move)
    if (!hasLooked)
    {
        bool track_move = true;

        surrouding_point_TARGET(newX, newY);

        if (battlefield->isRobotAt(newX, newY))
        {
            auto enemy = battlefield->findRobotAt(newX, newY);
            cout << name << " cannot move to (" << newX << "," << newY << "). This point is occupied by " << enemy->getName() << "." << endl;
            logFile << name << " cannot move to (" << newX << "," << newY << "). This point is occupied by " << enemy->getName() << "." << endl;
        }
        else
        {
            setPosition(newX, newY);
            cout << name << " moved to (" << newX << "," << newY << ")." << endl;
            logFile<< name << " moved to (" << newX << "," << newY << ")." << endl;
        }
    }

    // look -> move , empty POINTS --> (no point/ move)
    else if (hasLooked)
    {

        if (empty_point.empty())
        {
            cout << name << " didn't find any empty point to move! " << name << " may be surrounded!" << endl;
            logFile << name << " didn't find any empty point to move! " << name << " may be surrounded!" << endl;
            return;
        }

        else
        {
            uniform_int_distribution<> dis(0, empty_point.size() - 1);
            int num = dis(gen);
            newX = empty_point[num].first;
            newY = empty_point[num].second;
        }

        setPosition(newX, newY);
        cout << name << " moved to (" << newX << "," << newY << ")." << endl;
        logFile << name << " moved to (" << newX << "," << newY << ")." << endl;
    }

    if (battlefield->checkLandmine(newX, newY))
    {
        if (rand() % 100 < 50)
        {
            cout << name << " triggered a landmine at ("
                 << newX << "," << newY << ")!\n";
            logFile << name << " triggered a landmine at ("
                 << newX << "," << newY << ")!\n";
            destroy();
        }
        else
        {
            cout << name << " narrowly avoided a landmine at ("
                 << newX << "," << newY << ")!\n";
            logFile << name << " narrowly avoided a landmine at ("
                 << newX << "," << newY << ")!\n";
        }
    }
}

void GenericRobot::fire(int dx, int dy)
{
    if (shells == 0)
    {
        cout << name << " has no shells left! Self-destructing..." << endl;
        logFile << name << " has no shells left! Self-destructing..." << endl;
        selfDestructed = true;
        destroy();
        return;
    }

    int targetX;
    int targetY;

    // fire --> look, surrounding POINTS --> (shot no enemy/ shot enemy)
    if (hasLooked == false)
    {
        surrouding_point_TARGET(targetX, targetY);
    }

    // look --> fire, enemy POINTS --> (NO shot no enemy/ shot enemy)
    else if (hasLooked == true)
    {
        hasFired = true;
        int cout_enemy = lookGot_enemy_point.size();

        // NO shot no enemy, return back
        if (cout_enemy == 0)
        {
            cout << "Preserving shell for next turn since " << name << " didn't find any robots around. (left shells: " << shells << ")" << endl;
            logFile << "Preserving shell for next turn since " << name << " didn't find any robots around. (left shells: " << shells << ")" << endl;
            return;
        }

        else if (cout_enemy == 1)
        {
            targetX = lookGot_enemy_point[0].first;
            targetY = lookGot_enemy_point[0].second;

            int curX = getX();
            int curY = getY();

            bool isNearby = false;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = curX + dx;
                    int ny = curY + dy;

                    if (nx == targetX && ny == targetY) {
                        isNearby = true;
                        break;
                    }
                }
                if (isNearby) break;
            }

            if (!isNearby) {
                cout << name << " can't find the previous target after moving. Skipping fire to save shell." << endl;
                logFile << name << " can't find the previous target after moving. Skipping fire to save shell." << endl;
                return;
            }
        }

        // many enemies, need to check which is the higher level enemy
        else
        {
            shot_higher_enemy(targetX, targetY, lookGot_enemy_point);
        }
    }

    // shot robot
    if (battlefield->findRobotAt(targetX, targetY))
    {
        int curX = getX();
        int curY = getY();
        
        for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = curX + dx;
                    int ny = curY + dy;

                    if (nx == targetX && ny == targetY) {
                        isNearby = true;
                        break;
                    }
                }
                if (isNearby) break;
            }

        if(!isNearby){
            cout << name << " can't find the previous target after moving. Skipping fire to save shell." << endl;
            logFile << name << " can't find the previous target after moving. Skipping fire to save shell." << endl;
            return;
        }
        auto enemy = battlefield->findRobotAt(targetX, targetY);
        uniform_int_distribution<> dis(0, 99);
        shells--;


        cout << name << " fires " << enemy->getName() << " at (" << targetX << "," << targetY << ")";
        cout << " (Left shells: " << shells << "/" << max_shells << ")" << endl;

        logFile << name << " fires " << enemy->getName() << " at (" << targetX << "," << targetY << ")";
        logFile << " (Left shells: " << shells << "/" << max_shells << ")" << endl;


        if (enemy->isHidden())
        {
            cout << "Attack missed! " << enemy->getName() << " is hidden!" << endl;
            logFile << "Attack missed! " << enemy->getName() << " is hidden!" << endl;
            return;
        }

        if (dis(gen) < 70)
        {
            cout << "Target hit! " << enemy->getName() << " has been destroyed! ";
            logFile << "Target hit! " << enemy->getName() << " has been destroyed! ";
            enemy->destroy();
            chooseUpgrade(); // Upgrade
        }

        else
        {
            cout << " - MISS!" << endl;
            logFile << " - MISS!" << endl;
        }
    }

    // Shot no enemy
    else
    {
        shells--;
        cout << name << " fires at (" << targetX << "," << targetY << "). But it is an empty space!";
        cout << " (Left shells: " << shells << "/10)" << endl;

        logFile << name << " fires at (" << targetX << "," << targetY << "). But it is an empty space!";
        logFile << " (Left shells: " << shells << "/10)" << endl;
    }
}

void GenericRobot::respawn(int x, int y)
{
    Robot::respawn(x, y);
    if (alive())
    {
        shells = 10;
        selfDestructed = false;
        resetTurn();
    }
}

void GenericRobot::destroy()
{
    if (!selfDestructed)
    {
        selfDestructed = true;
        Robot::destroy();
        upgradedAreas.clear();
        upgradeNames.clear();
    }
}

bool GenericRobot::shouldRespawn() const
{
    return !isAlive && lives > 0;
}

int GenericRobot::getX() const
{
    return positionX;
}

int GenericRobot::getY() const
{
    return positionY;
}

void GenericRobot::chooseUpgrade()
{
    if (upgradeCount == 3)
    {
        string sentence = name + " is " + upgradeNames[0];
        int size = upgradeNames.size();
        for (size_t i = 1; size < i; i++)
        {
            sentence += "," + upgradeNames[i];
        }
        sentence += ". Cannot upgrade anymore, max upgrade 3 times";
        cout << sentence << endl;
        logFile << sentence << endl;
        return;
    }

    vector<int> availableOptions;
    if (upgradedAreas.find("move") == upgradedAreas.end())
        availableOptions.push_back(0);
    if (upgradedAreas.find("shoot") == upgradedAreas.end())
        availableOptions.push_back(1);
    if (upgradedAreas.find("see") == upgradedAreas.end())
        availableOptions.push_back(2);

    if (availableOptions.empty())
    {
        cout << name << " has no more areas to upgrade!" << endl;
        logFile << name << " has no more areas to upgrade!" << endl;
        return;
    }

    int randomIndex = rand() % availableOptions.size();
    int chosenOption = availableOptions[randomIndex];
    chooseUpgrade(chosenOption);
}

void GenericRobot::chooseUpgrade(int upgradeOption)
{
    if (upgradeCount >= 3) {
        cout << name << " cannot upgrade anymore (max 3 upgrades reached)" << endl;
        logFile << name << " cannot upgrade anymore (max 3 upgrades reached)" << endl;
        return;
    }

    const char *area = "";
    switch (upgradeOption)
    {
    case 0:
        area = "move";
        break;
    case 1:
        area = "shoot";
        break;
    case 2:
        area = "see";
        break;
    default:
        cout << "Invalid upgrade option: " << upgradeOption << endl;
        logFile << "Invalid upgrade option: " << upgradeOption << endl;
        return;
    }

    if (upgradedAreas.find(area) != upgradedAreas.end())
    {
        cout << name << " already upgraded " << area << " area!" << endl;
        logFile << name << " already upgraded " << area << " area!" << endl;
        return;
    }

    auto self = shared_from_this();
    shared_ptr<GenericRobot> newBot;
    string upgradeName = "";
    
    
    switch (upgradeOption)
    {
    case 0: // Moving upgrade
    {
        int choice = rand() % 3;
        if (choice == 0)
        {
            upgradeName = "HideBot";
            newBot = createUpgradedBot<HideBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "JumpBot";
            newBot = createUpgradedBot<JumpBot>();
        }
        else
        {
            upgradeName = "GlideBot";
            newBot = createUpgradedBot<GlideBot>();
        }
        cout << name << " upgraded movement: " << upgradeName << endl;
        logFile << name << " upgraded movement: " << upgradeName << endl;
    }
    break;

    case 1: // Shooting upgrade
    {
        //int choice = rand() % 4;
        int choice = 1;
        if (choice == 0)
        {
            upgradeName = "LongShotBot";
            newBot = createUpgradedBot<LongShotBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "SemiAutoBot";
            newBot = createUpgradedBot<SemiAutoBot>();
        }
        else if (choice == 2)
        {
            upgradeName = "ThirtyShotBot";
            newBot = createUpgradedBot<ThirtyShotBot>();
        }
        else
        {
            upgradeName = "LandmineBot";
            newBot = createUpgradedBot<LandmineBot>();
        }
        cout << name << " upgraded shooting: " << upgradeName << endl;
        logFile << name << " upgraded shooting: " << upgradeName << endl;
    }
    break;

    case 2: // Seeing upgrade
    {
        int choice = rand() % 3;
        if (choice == 0)
        {
            upgradeName = "ScoutBot";
            newBot = createUpgradedBot<ScoutBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "TrackBot";
            newBot = createUpgradedBot<TrackBot>();
        }
        else
        {
            upgradeName = "RevealBot";
            newBot = createUpgradedBot<RevealBot>();
        }
        cout << name << " upgraded vision: " << upgradeName << endl;
        logFile << name << " upgraded vision: " << upgradeName << endl;
    }
    break;
    }

    if (newBot)
    {       
        upgradeNames.push_back(upgradeName);
        upgradedAreas.insert(area);
        upgradeCount++;

        // Transfer all state to new robot
        newBot->upgradeNames = this->upgradeNames;
        newBot->upgradedAreas = this->upgradedAreas;
        newBot->upgradeCount = this->upgradeCount;
        newBot->name = this->name;

        battlefield->replaceRobot(self, newBot);

        string sentence = name + " now has upgrades: " + upgradeNames[0];
        int size = upgradeNames.size();
        for (size_t i = 1; i < size; i++)
        {
            sentence += ", " + upgradeNames[i];
        }

        cout << sentence << " (Total: " << upgradeCount << "/3)" << endl;
        logFile << sentence << " (Total: " << upgradeCount << "/3)" << endl;

        if (upgradeCount >= 2)
        {
            newBot->replaceWithCombination(newBot->upgradeNames);
        }
    }
    else
    {
        cout << "Failed to create upgraded robot for " << upgradeName << endl;
        logFile << "Failed to create upgraded robot for " << upgradeName << endl;
    }
}

void GenericRobot::replaceWithCombination(const vector<string> &types)
{
    if (types.size() < 2)
        return;

    auto self = shared_from_this();
    shared_ptr<GenericRobot> newBot;
    string combinationName = "";

    auto hasType = [&](const string &type)
    {
        return find(types.begin(), types.end(), type) != types.end();
    };

    cout << "Attempting combination with " << types.size() << " upgrades: ";
    logFile << "Attempting combination with " << types.size() << " upgrades: ";
    for (size_t i = 0; i < types.size(); ++i) {
        cout << types[i];
        logFile << types[i];
        if (i != types.size() - 1) {
            cout << ", ";
            logFile << ", ";
        }
    }
    cout << endl;
    logFile << endl;
        
    if (types.size() == 2)
    {
        // Movement + Shooting combinations
        if (hasType("HideBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<HideLongShotBot>();
            combinationName = "HideLongShotBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoBot>();
            combinationName = "HideSemiAutoBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotBot>();
            combinationName = "HideThirtyShotBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<HideLandmineBot>();
            combinationName = "HideLandmineBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<JumpLongShotBot>();
            combinationName = "JumpLongShotBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoBot>();
            combinationName = "JumpSemiAutoBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotBot>();
            combinationName = "JumpThirtyShotBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<JumpLandmineBot>();
            combinationName = "JumpLandmineBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<GlideLongShotBot>();
            combinationName = "GlideLongShotBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoBot>();
            combinationName = "GlideSemiAutoBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotBot>();
            combinationName = "GlideThirtyShotBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<GlideLandmineBot>();
            combinationName = "GlideLandmineBot";
        }
        // Movement + Vision combinations
        else if (hasType("HideBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideScoutBot>();
            combinationName = "HideScoutBot";
        }
        else if (hasType("HideBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideTrackBot>();
            combinationName = "HideTrackBot";
        }
        else if (hasType("HideBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideRevealBot>();
            combinationName = "HideRevealBot";
        }
        else if (hasType("JumpBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpScoutBot>();
            combinationName = "JumpScoutBot";
        }
        else if (hasType("JumpBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpTrackBot>();
            combinationName = "JumpTrackBot";
        }
        else if (hasType("JumpBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpRevealBot>();
            combinationName = "JumpRevealBot";
        }
        else if (hasType("GlideBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideScoutBot>();
            combinationName = "GlideScoutBot";
        }
        else if (hasType("GlideBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideTrackBot>();
            combinationName = "GlideTrackBot";
        }
        else if (hasType("GlideBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideRevealBot>();
            combinationName = "GlideRevealBot";
        }
        // Shooting + Vision combinations
        else if (hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<LongShotScoutBot>();
            combinationName = "LongShotScoutBot";
        }
        else if (hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<LongShotTrackBot>();
            combinationName = "LongShotTrackBot";
        }
        else if (hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<LongShotRevealBot>();
            combinationName = "LongShotRevealBot";
        }
        else if (hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<SemiAutoScoutBot>();
            combinationName = "SemiAutoScoutBot";
        }
        else if (hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<SemiAutoTrackBot>();
            combinationName = "SemiAutoTrackBot";
        }
        else if (hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<SemiAutoRevealBot>();
            combinationName = "SemiAutoRevealBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<ThirtyShotScoutBot>();
            combinationName = "ThirtyShotScoutBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<ThirtyShotTrackBot>();
            combinationName = "ThirtyShotTrackBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<ThirtyShotRevealBot>();
            combinationName = "ThirtyShotRevealBot";
        }
        else if (hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<LandmineScoutBot>();
            combinationName = "LandmineScoutBot";
        }
        else if (hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<LandmineTrackBot>();
            combinationName = "LandmineTrackBot";
        }
        else if (hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<LandmineRevealBot>();
            combinationName = "LandmineRevealBot";
        }
    }
    else if (types.size() == 3)
    {
        if (hasType("HideBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideLongShotScoutBot>();
            combinationName = "HideLongShotScoutBot";
        }
        else if (hasType("HideBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideLongShotTrackBot>();
            combinationName = "HideLongShotTrackBot";
        }
        else if (hasType("HideBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideLongShotRevealBot>();
            combinationName = "HideLongShotRevealBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoScoutBot>();
            combinationName = "HideSemiAutoScoutBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoTrackBot>();
            combinationName = "HideSemiAutoTrackBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoRevealBot>();
            combinationName = "HideSemiAutoRevealBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotScoutBot>();
            combinationName = "HideThirtyShotScoutBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotTrackBot>();
            combinationName = "HideThirtyShotTrackBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotRevealBot>();
            combinationName = "HideThirtyShotRevealBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideLandmineScoutBot>();
            combinationName = "HideLandmineScoutBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideLandmineTrackBot>();
            combinationName = "HideLandmineTrackBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideLandmineRevealBot>();
            combinationName = "HideLandmineRevealBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpLongShotRevealBot>();
            combinationName = "JumpLongShotScoutBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpLongShotTrackBot>();
            combinationName = "JumpLongShotTrackBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpLongShotRevealBot>();
            combinationName = "JumpLongShotRevealBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoScoutBot>();
            combinationName = "JumpSemiAutoScoutBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoTrackBot>();
            combinationName = "JumpSemiAutoTrackBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoRevealBot>();
            combinationName = "JumpSemiAutoRevealBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotScoutBot>();
            combinationName = "JumpThirtyShotScoutBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotTrackBot>();
            combinationName = "JumpThirtyShotTrackBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotRevealBot>();
            combinationName = "JumpThirtyShotRevealBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpLandmineScoutBot>();
            combinationName = "JumpLandmineScoutBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpLandmineTrackBot>();
            combinationName = "JumpLandmineTrackBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpLandmineRevealBot>();
            combinationName = "JumpLandmineRevealBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideLongShotScoutBot>();
            combinationName = "GlideLongShotScoutBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideLongShotTrackBot>();
            combinationName = "GlideLongShotTrackBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideLongShotRevealBot>();
            combinationName = "GlideLongShotRevealBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoScoutBot>();
            combinationName = "GlideSemiAutoScoutBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoTrackBot>();
            combinationName = "GlideSemiAutoTrackBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("Reveal"))
        {
            newBot = createUpgradedBot<GlideSemiAutoRevealBot>();
            combinationName = "GlideSemiAutoRevealBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotScoutBot>();
            combinationName = "GlideThirtyShotScoutBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotTrackBot>();
            combinationName = "GlideThirtyShotTrackBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotRevealBot>();
            combinationName = "GlideThirtyShotRevealBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideLandmineScoutBot>();
            combinationName = "GlideLandmineScoutBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideLandmineTrackBot>();
            combinationName = "GlideLandmineTrackBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideLandmineRevealBot>();
            combinationName = "GlideLandmineRevealBot";
        }
    }

    if (newBot && !combinationName.empty())
    {

        newBot->upgradeNames = this->upgradeNames;
        newBot->upgradedAreas = this->upgradedAreas;
        newBot->upgradeCount = this->upgradeCount;
        newBot->name = this->name;

        battlefield->replaceRobot(self, newBot);
        cout << "Successfully combined " << name << " into " << combinationName << "!" << endl;
        logFile << "Successfully combined " << name << " into " << combinationName << "!" << endl;
    }
    else
    {
        cout << name << " upgrade combination not found for: ";
        logFile << name << " upgrade combination not found for: ";
        for (const auto &s : types){    
            cout << s << ' ';
            cout << "\nUsing individual upgrades instead." << endl;
            logFile << s << ' ';
            logFile << "\nUsing individual upgrades instead." << endl;
        }
           
    }
    
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

void Battlefield::addToRespawn(shared_ptr<Robot> robot) {
        lock_guard<mutex> lock(respawnMutex);
        respawnQueue.push(robot);
}

bool Battlefield::checkAttackHit(shared_ptr<Robot> attacker, 
                               shared_ptr<Robot> target) {
    if (auto hideBot = dynamic_cast<HideBot*>(target.get())) {
        return !hideBot->isHidden();
    }
    return true;
}

void Battlefield::addLandmine(int x, int y) {
        landmines.emplace(x, y);
}

bool Battlefield::checkLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
        landmines.erase(it); 
        if (rand() % 100 < 50) { 
            cout << "Landmine triggered at (" << x << "," << y << ")!\n";
            logFile << "Landmine triggered at (" << x << "," << y << ")!\n";
            return true;
        }
    }
    return false;
}

bool Battlefield::isLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
            return true;
    }
    return false;
}

void Battlefield::simulateTurn() {
    processRespawn();
    bool simulation = true;

    // Shuffle robots for random turn order
    shuffle(robots.begin(), robots.end(), gen);

    // Print order robot
    vector<shared_ptr<Robot>> copy = robots;  
    copy.erase( // Store the robot which are alive in this turn
        remove_if(copy.begin(), copy.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive();
            }),
        copy.end()
    );

    string r_order = "\nRobot's order: " + copy[0] -> getName();

    int size = copy.size();
    for(int i=1; i<size; i++){
        r_order+= " --> " + copy[i]-> getName();
    }
    cout << r_order << endl; 
    logFile << r_order << endl; 

    for(auto& robot : copy){
        if(robot->alive()){
            executeRobotTurn(robot,copy);
        }

        else if(robot->shouldRespawn()){
            auto it = find(copy.begin(), copy.end(), robot);
            if (it != copy.end()) {
                cout << "\nSkipping " << robot->getName() << " because it died in this turn." << endl;
                cout<<endl;

                logFile << "\nSkipping " << robot->getName() << " because it died in this turn." << endl;
                logFile<<endl;
            }
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

    // Got robot that needs to respawn
    if (!respawnQueue.empty()) {

        // Print respawn order
        queue<shared_ptr<Robot>> tempQueue = respawnQueue; // Copy queue respawnQueue
        auto copy_robot = tempQueue.front();
        tempQueue.pop(); 
        string respawn_order = "Respawn robots queue: " + copy_robot -> getName();
        while (!tempQueue.empty()) {
            shared_ptr<Robot> robot = tempQueue.front();
            respawn_order+= " --> " + robot->getName();
            tempQueue.pop();
        }
        cout << respawn_order << endl; 
        logFile << respawn_order << endl; 


        // ProcessRespawn
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
                    logFile << "Couldn't find empty spot for " << robot->getName() << endl;
                    respawnQueue.push(robot);  // Retry next turn
                    return;
                }
            } while (findRobotAt(x, y));
            
            robot->respawn(x, y);
            display();
        }
    }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy) {
    if (!robot->alive()) return;  // Skip dead robots
    if (auto trackBot = dynamic_cast<TrackBot*>(robot.get())) {
        trackBot->displayTracked();
    }
    robot->resetTurn();

    // Create all possible action permutations
    const vector<vector<string>> actionOrders = {
        {"look", "fire", "move", "think"},
        {"look", "fire", "think", "move"},
        {"look", "move", "fire", "think"},
        {"look", "move", "think", "fire"},
        {"look", "think", "fire", "move"},
        {"look", "think", "move", "fire"},

        {"fire", "look", "move", "think"},
        {"fire", "look", "think", "move"},
        {"fire", "move", "look", "think"},
        {"fire", "move", "think", "look"},
        {"fire", "think", "look", "move"},
        {"fire", "think", "move", "look"},

        {"move", "look", "fire", "think"},
        {"move", "look", "think", "fire"},
        {"move", "fire", "look", "think"},
        {"move", "fire", "think", "look"},
        {"move", "think", "look", "fire"},
        {"move", "think", "fire", "look"}

    };

    // Cout order action
    auto& order = actionOrders[rand() % actionOrders.size()];
    cout << endl << robot->getName() << "'s action order is " << order[0] << " --> "<< order[1] << " --> " << order[2] << " --> " << order[3] << endl;
    logFile << endl << robot->getName() << "'s action order is " << order[0] << " --> "<< order[1] << " --> " << order[2] << " --> " << order[3] << endl;

    for (const auto& action : order){
        int dx,dy;
        if (action == "look") {
            robot->look(dx, dy);
        }

        else if (action == "fire"){
            robot->fire(dx, dy);
        }
        
        else if (action == "move"){
            robot->move(rand() % 3 - 1, rand() % 3 - 1);
            display();

        }
        else{
            robot->think();
        }
    }

    // Handle destruction if out of shells
    if (robot->getShells() <= 0 && !robot->hasSelfDestructed()) {
        robot->destroy();
        if (robot->shouldRespawn()) {
            respawnQueue.push(shared_ptr<Robot>(robot));
        }
    }
}

void Battlefield::replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot) {
    // Copy important state from old robot to new robot
    newBot->setPosition(oldBot->getX(), oldBot->getY());
    newBot->setLives(oldBot->getLives()); 
    newBot->setShells(oldBot->getShells());
    
    // Find and replace the robot in the vector
    auto it = find(robots.begin(), robots.end(), oldBot);
    if (it != robots.end()) {
        *it = newBot;  // Replace the robot
        // Don't erase it after replacing! This was the bug
        // robots.erase(it);  // Remove this line
    } else {
        cout << "Warning: Could not find robot to replace in battlefield" << endl;
        logFile << "Warning: Could not find robot to replace in battlefield" << endl;
    }
}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(height, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()-1][robot->getX()-1] = robot->getName()[0];
        }
    }

    cout << "--- Battlefield Status ---\n";
    logFile << "--- Battlefield Status ---\n";
    for (int i = 0 ; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << grid[i][j] << ' ';
            logFile << grid[i][j] << ' ';
        }
        cout << endl;
        logFile << endl;
    }

}

int check_point(const vector<int>& point, int &max_height ); // Check if the set point is not out of bounds
void One_Spaces_only(string& line); // Leave only one space between each word.
string extractWord(const string& line, const int& substr, int& i);
void parse_or_random(const string& value, int max, vector<int>& result); // check if it is a number or what the system generate a random number

int main() {
    ifstream file("set.txt");
    // ofstream logFile("output_log.txt");
    string line;
    vector<string> names;
    vector<int> initial_x;
    vector<int> initial_y;
    int M, N, steps = 0, num_robots = 0;
    srand(time(0));

    while (getline(file, line)) {
        One_Spaces_only(line); // Leave only one space between each word.

        if (line.rfind("M by N :", 0) == 0) {
            int i;
            M = stoi(extractWord(line, 9, i)); 
            N = stoi(line.substr(i));          
        } else if (line.rfind("steps:", 0) == 0) {
            steps = stoi(line.substr(7));      
        } else if (line.rfind("robots:", 0) == 0) {
            num_robots = stoi(line.substr(8));
        } else if (line.rfind("GenericRobot", 0) == 0) {
            int i;
            names.push_back(extractWord(line, 13, i)); 
            string x = extractWord(line, i, i);
            string y = line.substr(i);

            
            parse_or_random(x, M, initial_x); // Check if it is a number or what the system generate a random number
            parse_or_random(y, N, initial_y);
            
        }
    }

    // Check if the number of robot is not equal to the number of setting robot
    if (num_robots != names.size()) {
        cout << "Robot count mismatch! Check your set.txt.\n";
        logFile << "Robot count mismatch! Check your set.txt.\n";
        return -1;
    }

    // Check if the set point is not out of bounds
    int x_OutOfBound = check_point(initial_x,M);
    if(x_OutOfBound != -1){
        cout << "You have assigned robot " << names[x_OutOfBound] << " to an out-of-bounds point!";
        logFile << "You have assigned robot " << names[x_OutOfBound] << " to an out-of-bounds point!";
        return -1;
    }

    // Check if the set point is not out of bounds
    int y_OutOfBound = check_point(initial_y,N);
    if(y_OutOfBound != -1){
        cout << "You have assigned robot " << names[y_OutOfBound] << " to an out-of-bounds point!";
        logFile << "You have assigned robot " << names[y_OutOfBound] << " to an out-of-bounds point!";
        return -1;
    }

    Battlefield field(M, N);

    for (int i = 0; i < num_robots; ++i) {
        auto r = std::make_shared<GenericRobot>(names[i], initial_x[i], initial_y[i], M, N, &field);
        field.addRobot(r);
    }


    for (int i = 0; i < steps; ++i) {
        cout << "\n--- Turn " << i + 1 << " ---\n";
        logFile << "\n--- Turn " << i + 1 << " ---\n";
        field.display();
        field.simulateTurn();
        
        // Field got 0 alive robot && the total live of dead robots is 0
        if (field.isEmpty() && field.countLiveRobot() == 0) {
            cout << "All robots are destroyed. Simulation ends.\n";
            logFile << "All robots are destroyed. Simulation ends.\n";
            break;
        }

        field.display();
        cout << "\n--- Turn " << i + 1 << " END---\n";
        logFile << "\n--- Turn " << i + 1 << " END---\n";


        // Field got 1 alive robot && the total live of dead robots is 0
        if (field.countAliveRobots() == 1 && field.countLiveRobot() ==0) {
            cout << "Simulation ends! ";
            logFile << "Simulation ends! ";
            auto winner = field.getAliveRobot();
            cout << "Winner: " << winner->getName() << endl;
            break;
        }
    }
    logFile.close();
    return 0;
}

void One_Spaces_only(string& line) {
    istringstream iss(line);
    string word, result;
    while (iss >> word) {
        if (!result.empty()) result += " ";
        result += word;
    }
    line = result;
}

string extractWord(const string& line, const int& substr, int& i) {
    string word;
    for (char c : line.substr(substr)) {
        if (c == ' ') break;
        word += c;
    }
    i = substr + word.size() + 1;
    return word;
}

void parse_or_random(const string& value, int max, vector<int>& result) {
    if (value == "random") {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(1, max);
        result.push_back(dist(gen));
    } else {
        result.push_back(stoi(value));
    }
}

int check_point(const vector<int>& point, int &max_height ){
    for(int i=0; i< point.size(); i++){
        if(point[i]> max_height|| point[i] <= 0){
            return i;
        }
    }
    return -1;
}