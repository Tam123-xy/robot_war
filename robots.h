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

#ifndef ROBOTS_H
#define ROBOTS_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <set>
#include <mutex>
#include "battlefield.h"
using namespace std;


class Battlefield;

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
protected:
    int visionRange = 1;

public:
    SeeingRobot(string name, int x, int y, int w, int h, Battlefield* bf)
        : Robot(name, x, y, w, h, bf) {}
    virtual void look(int dx, int dy) = 0;

};


class ShootingRobot : virtual public Robot {
protected:
    int shells = 10;
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
        cout << name << " self-destructed!\n";
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
    // Battlefield* battlefield;
    // int shells;
    // bool selfDestructed;
    int upgradeCount = 0;
    set<string> upgradedAreas;
    vector<string> upgradeNames;
    template<typename T>
    shared_ptr<T> createUpgradedBot();
    void replaceWithCombination(const vector<string>& types);

   
protected:
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
    
    // Action methods
    void resetTurn() {
        hasLooked = hasFired = hasMoved = false;
    }
    void think() override;
    void look(int dx, int dy) override;
    void move(int dx, int dy) override;
    void fire(int dx, int dy) override;
    // vector<string> scout(int dx, int dy);
    
    // State check methods
    bool canLook() const { return !hasLooked; }
    bool canFire() const { return !hasFired && shells > 0; }
    bool canMove() const { return !hasMoved; }
    
    int getShells() const { return shells; }
    bool hasSelfDestructed() const { return selfDestructed;}
    void destroy();     
    bool shouldRespawn() const;
    virtual void respawn(int x, int y);

    void chooseUpgrade();
    void chooseUpgrade(int upgradeOption);
    void showUpgrades() const;
    

    int getX() const;   
    int getY() const; 
    string getType() const override;

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
            }
        } else {
            GenericRobot::move(dx, dy);
        }
    }

    string getType() const override { return "GlideBot"; }
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
            for (int y = 1; y <= battlefield->getHeight(); ++y) {
                for (int x = 1; x <= battlefield->getWidth(); ++x) {
                    if (battlefield->isRobotAt(x, y)) {
                        auto robot = battlefield->findRobotAt(x, y);
                        cout << "  (" << x << "," << y << "): " 
                             << robot->getName() << endl;
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
                cout << name << " tracks " << target->getName() 
                     << " (" << trackers << " trackers left)" << endl;
            }
        }
        GenericRobot::look(dx, dy);
    }
    
    // 新增追踪显示
    void displayTracked() {
        for (auto& robot : trackedRobots) {
            if (robot->alive()) {
                cout << "  Tracking " << robot->getName()
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
            cout << name << " can't fire that far! (Max " << fireRange << ")" << endl;
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
            int targetX = lookGot_enemy_point[0].first;
            int targetY = lookGot_enemy_point[0].second;

            if (battlefield->isRobotAt(targetX, targetY)) {
                auto target = battlefield->findRobotAt(targetX, targetY);
                if (target && target->alive()) {
                    target->destroy();
                    cout << name << " hits " << target->getName() 
                         << " at (" << targetX << "," << targetY << ")!" << endl;
                }
            } else {
                break;
            }

            if (shells == 0){
                break;
            }

            useShell();  // consume a shell
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
        shells = 30; // 修改8: 直接设置弹药量
        cout << name << " loaded 30 shells!" << endl;
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

        // 调用基类开火逻辑
        GenericRobot::fire(dx, dy);

        // 如果没有击中目标
        if (!battlefield->isRobotAt(targetX, targetY)) {
            battlefield->addLandmine(targetX, targetY);
            cout << name << " placed landmine at (" << targetX << "," << targetY << ")\n";
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

#endif