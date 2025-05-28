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


// class Battlefield;

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
protected:

public:
    MovingRobot(string name, int x, int y, int w, int h, Battlefield* bf) 
        : Robot(name, x, y, w, h, bf) {}
    virtual ~MovingRobot() = default;

    //Basic Movement
    virtual void move(int dx, int dy) =0;
    
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
    bool m_isHidden; 
public:
    HideBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf), 
            hideCount(3),
            m_isHidden(false) {}

    bool isHidden() const override { return m_isHidden; } 
    void setHidden(bool state) { m_isHidden = state; }

    void move(int dx, int dy) override {
        if (hideCount > 0) {
            hideCount--;
            setHidden(true);
        } else {
            GenericRobot::move(dx, dy);
            setHidden(false);
        }
    }
    
    string getType() const override { return "HideBot"; }
};

class JumpBot : virtual public GenericRobot {
    int jumpCount;
public:
    JumpBot(const string& name, int x, int y, int w, int h, Battlefield* bf) 
        :   Robot(name, x, y, w, h, bf),
            GenericRobot(name, x, y, w, h, bf),
            jumpCount(3) {}

    void move(int dx, int dy) override {
        if (jumpCount > 0) {
            int newX = 1 + rand() % battlefield->getWidth();
            int newY = 1 + rand() % battlefield->getHeight();

            newX = (newX < 1) ? 1 : (newX > battlefield->getWidth()) ? battlefield->getWidth() : newX;
            newY = (newY < 1) ? 1 : (newY > battlefield->getHeight()) ? battlefield->getHeight() : newY;
            
            if (!battlefield->isRobotAt(newX, newY)) {
                setPosition(newX, newY);
                jumpCount--;
                cout << name << " jumps to (" << newX << "," << newY << ") "
                     << "(" << jumpCount << " jumps left)" << endl;
            }
        } else {
            GenericRobot::move(dx, dy);
        }
    }
    string getType() const override { return "JumpBot"; }
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
        fireDx = dx;
        fireDy = dy;

        for (int i = 0; i < 3 && canFire(); ++i) {
            int targetX, targetY;
            getFireTarget(targetX, targetY);

            if (battlefield->isRobotAt(targetX, targetY)) {
                auto target = battlefield->findRobotAt(targetX, targetY);
                if (target && target->alive()) {
                    target->destroy();
                    cout << name << " hits " << target->getName() 
                         << " at (" << targetX << "," << targetY << ")!" << endl;
                }
            } else {
                cout << name << " fires at empty space (" << targetX << "," << targetY << ")." << endl;
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


#endif