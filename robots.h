/**********|**********|**********|
Program: main.cpp / robots.h / battlefield.h
Course: OOPDS
Trimester: 2520
Name: TAM XIN YI | YIAP WEI SHANZ |TAY SHI XIANG
ID: 243UC247G6 | 243UC247CV | 243UC247GE
Lecture Section: TC1L
Tutorial Section: TT2L
Email: TAM.XIN.YI@student.mmu.edu.my | YIAP.WEI.SHANZ@student.mmu.edu.my | TAY.SHI.XIANG@student.mmu.edu.my
Phone: 011-11026051 | 011-59964357 | 019-3285968
**********|**********|**********/

#ifndef ROBOTS_H
#define ROBOTS_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <set>
using namespace std;

class Battlefield;  // Forward declaration

class Robot {
protected:
    string name;
    int lives = 3;
    bool isAlive = true;
    bool inRespawnQueue = false;
    int positionX, positionY;  // Changed from private to protected
    int width, height;
   
public:
    Robot(string name, int x, int y, int w, int h)
        : name(name), positionX(x), positionY(y), width(w), height(h) {}
    virtual ~Robot() = default;

    int getX() const { return positionX; }
    int getY() const { return positionY; }

    virtual string getType() const = 0;

    void setPosition(int x, int y) { positionX = x; positionY = y; }
    bool alive() const { return isAlive; }
    void destroy();
    virtual void respawn(int x, int y);
    bool shouldRespawn() const;
    string getName() const { return name; }
    int getLives() const { return lives; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    virtual bool getIsHidden() const { return false; }
    virtual void unhide() {};
};

class MovingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual ~MovingRobot() = default;

    virtual void move(int dx, int dy) = 0;
};

class HideBot : virtual public MovingRobot {
protected:
    bool hasHideAbility = false;
    int hideCount = 0;
    bool isHidden = false;
    int hiddenTurnsRemaining = 0;

public:
    bool hide() {
        if (canHide()) {
            isHidden = true;
            hideCount++;
            hiddenTurnsRemaining = 1; // stays hidden for one opponent turn
            cout << name << " is now hidden! (" << 3 - hideCount
                << " hides left)" << endl;
            return true;
        }
        return false;
    }

    void unhide() override {
        if (isHidden && hiddenTurnsRemaining > 0) {
            hiddenTurnsRemaining--;
            if (hiddenTurnsRemaining == 0) {
                isHidden = false;
                cout << name << " is now visible again!" << endl;
            }
        }
    }

    void activateHideAbility() {
        hasHideAbility = true;
        cout << name << " gained HideBot abilities!" << endl;
    }

    bool getIsHidden() const override { return isHidden; }
    bool canHide() const { return hasHideAbility && hideCount < 3 && !isHidden; }
};

class JumpBot : virtual public MovingRobot {
protected:
    bool hasJumpAbility = false;
    int jumpCount = 0;

public:
    virtual bool jump(int newX, int newY) {
        if (canJump() && 
            newX > 0 && newX < width && 
            newY > 0 && newY < height && 
            !(newX == positionX && newY == positionY)) {

            positionX = newX;
            positionY = newY;
            jumpCount++;
            cout << name << " jumped to (" << newX << "," << newY << ") (" 
                << 3 - jumpCount << " jumps left)" << endl;
            return true;
        }
        return false;
    }

    void activateJumpAbility() {
        hasJumpAbility = true;
        cout << name << " gained JumpBot abilities!" << endl;
    }

    bool canJump() const { return hasJumpAbility && jumpCount < 3; }
};


class ShootingRobot : virtual public Robot {
protected:
    int shells = 10;
    int fireRange = 1; // default = 1
    bool hasThirtyShots = false;


public:
    using Robot::Robot;
    virtual void fire(int dx, int dy) = 0;

    int getShells() const { return shells; }
    void reloadThirtyShots() { shells = 30; hasThirtyShots = true; }    //ThirtyShots
    void extendRange() { fireRange = 3; }   //longshot

    bool isLandmine = false;
    bool isSemiAuto = false;

    vector<pair<int, int>> minePositions;

    int getRange() const { return fireRange; }
    
    void useShell() {
        if (--shells <= 0) selfDestruct();
    }

    virtual void selfDestruct() {
        isAlive = false;
        cout << name << " self-destructed!\n";
    }
};


class SeeingRobot : virtual public Robot {
protected:
    int visionRange = 1;
    int scoutCount = 0;
    int trackCount = 0;

public:
    using Robot::Robot;
    virtual void look(int dx, int dy) = 0;

    // void grantScout() { scoutCount = 3; }
    // void grantTrack() { trackCount = 3; }

    // bool canScout() const { return scoutCount > 0; }
    // void useScout() { if (scoutCount > 0) scoutCount--; }

    // bool canTrack() const { return trackCount > 0; }
    // void useTrack() { if (trackCount > 0) trackCount--; }

    // void extendVision() { visionRange = 9999; } // entire battlefield
    // int getVisionRange() const { return visionRange; }
};


class ThinkingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void think() = 0;
};

class GenericRobot : public virtual MovingRobot,
                     public ShootingRobot, 
                     public SeeingRobot, 
                     public ThinkingRobot,
                     public HideBot, 
                     public JumpBot {
private:
    Battlefield* battlefield;
    int shells;
    bool selfDestructed;
    int upgradeCount = 0;
    set<string> upgradedAreas;
    vector<string> upgradeNames;

   
protected:
    bool hasLooked = false;
    bool hasFired = false;
    bool hasMoved = false;
    vector<pair<int, int>> empty_point;
    vector<pair<int, int>> lookGot_enemy_point;

public:
    GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf);
    
    // Action methods
    void resetTurn() {
        hasLooked = hasFired = hasMoved = false;
    }
    void think() override;
    virtual void look(int dx, int dy) override;
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

    // int upgradeCount = 0;
    // set<string> upgradedAreas;
    // vector<string> upgradeNames;
    // int jumpCount = 0;
    // int hideCount = 0;
    // int fireRange = 1;
    // int visionRange = 1;
    // bool semiAutoFire = false;

    void chooseUpgrade();
    void chooseUpgrade(int upgradeOption);
    void showUpgrades() const;
    

    int getX() const;   
    int getY() const; 
    string getType() const override;

};

#endif