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
    // int upgradeCount = 0;   
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
    // int getCoutUpgrade() const { return upgradeCount; }
    int getLives() const { return lives; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    virtual bool canBeHit() const { return true; }  // Default implementation - can always be hit
    virtual bool hide() { return false; } // Base implementation - no defense

    virtual const vector<pair<int, int>>& getScoutPoints() const = 0;
    virtual int getsize_ScoutPoints() const { return 0; }
    virtual bool isScout() const { return false; } // 默认不是 Scout
    virtual int getScoutCount() const { return 0; }
    virtual void setScoutCount(int) {}
    virtual void addScoutPoint(pair<int, int> pos) = 0;


    
};

class MovingRobot : virtual public Robot {
protected:
    bool hasJumpAbility = false;
    bool hasHideAbility = false;
    int jumpCount = 0;
    int hideCount = 0;
    bool isHidden = false;

public:
    using Robot::Robot;
    virtual ~MovingRobot() = default;

    //Basic Movement
    virtual void move(int dx, int dy) = 0;
    
    //Special movement abilities
    virtual bool jump(int newX, int newY) {
        if (canJump() && newX > 0 && newX < width && newY > 0 && newY < height) {
            positionX = newX;
            positionY = newY;
            jumpCount++;
            cout << name << "jumped to (" << newX << "," << newY << ") (" << 3 - jumpCount << " jumps left)" << endl;
            return true;
        }
        return false;
    }

    virtual bool hide() {
        if (canHide()) {
            isHidden = true;
            hideCount++;
            cout << name << " is now hidden (" << 3 - hideCount << " hides left)" << endl;
            return true;
        }
        return false;
    }

    virtual void reveal() {
        if (isHidden) {
            isHidden = false;
            cout << name << " is now visible again" << endl;
        }
    }

    //Ability activation
    void activateJumpAbility() {
        hasJumpAbility = true;
        cout << name << " gained JumpBot abilities!" << endl;
    }

    void activateHideAbility() {
        hasHideAbility = true;
        cout << name << " gained HideBot abilities!" << endl;
    }

    virtual bool defendAgainstAttack() {
        return false;   // Base robots don't defend by default
    }

    //Status checks
    bool canJump() const { return hasJumpAbility && jumpCount < 3; }
    bool canHide() const { return hasHideAbility && hideCount < 3; }
    bool isCurrentlyHidden() const { return isHidden; }
    int getRemainingJumps() const { return 3 - jumpCount; }
    int getRemainingHides() const { return 3 - hideCount; }

    //Movement validation
    bool isValidMove(int dx, int dy) const {
        int newX = positionX + dx;
        int newY = positionY + dy;
        return (newX > 0 && newX < width && newY > 0 && newY < height);
    }
};

class HideBot : virtual public MovingRobot {
private:
    int hideUses = 3;   //Track remaining hides
    bool isHidden = false;  //Current hidden state
    bool hideWhenAttacked = true;

public:
    HideBot(string name, int x, int y, int w, int h)
        : Robot(name, x, y, w, h), MovingRobot(name, x, y, w, h) {
        activateHideAbility();
    }
    
    string getType() const override { return "HideBot" ; }

    //Override hide to provide invulnerability
    bool hide() override {
        if (hideUses > 0 && !isHidden) {
            isHidden = true;
            hideUses--;
            return true;
        }
        return false;
    }

    bool defendAgainstAttack() {
        if (canHide() && hideWhenAttacked && !isHidden) {
            return hide();  //Automatically hide when attacked
        }
        return false;
    }

    void reveal() override {
        if (isHidden) {
            isHidden = false;
        }
    }

    // If we have hide ability and choose to use it
    void move(int dx, int dy) override {
        if (canHide() && rand() % 2 == 0) { // 50% chance to use hide if available
            if (hide()) {
                return;
            }
        }
    }

    //Method to check if robot can be hit (for attack logic)
    bool canBeHit() const override {
        return !isHidden;   // Can only be hit when not hidden
    }

    int getRemainingHides() const { return hideUses; }
    bool canHide() const { return hideUses > 0; }
    bool isCurrentlyHidden() const { return isHidden; }
};

class JumpBot : virtual public MovingRobot {
private:
    int jumpUses = 3;   //Track remaining jumps

public:
    JumpBot(string name, int x, int y, int w, int h)
        : Robot(name, x, y, w, h), MovingRobot(name, x, y, w, h) {
        activateJumpAbility();
    }
    
    string getType() const override { return "JumpBot" ; }

    // Override jump to allow jumping anywhere on the map
    bool jump(int newX, int newY) override {
        if (jumpUses > 0 && newX > 0 && newX < width && newY > 0 && newY < height) {
            positionX = newX;
            positionY = newY;
            jumpUses--;
            cout << name << " jumped to (" << newX << "," << newY << ")! (" << jumpUses << " jumps remaining)" << endl;
            return true;
        }
        return false;
    }

    void move(int dx, int dy) override {
        //JumpBot can choose to use normal move or jump
        if (canJump() && rand() % 2 == 0) { // 50% chance to use jump if available
            int newX = rand() % width;
            int newY = rand() % height;
            if (jump(newX, newY)) {
                return;
            }
        }

        // Normal movement if jump not used or not available
        if (isValidMove(dx, dy)) {
            positionX += dx;
            positionY += dy;
            cout << name << " moved to (" << positionX << "," << positionY << ")" << endl;
        }
    }

    int getRemainingJumps() const { return jumpUses; }
    bool canJump() const { return jumpUses > 0; }
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
    vector<pair<int, int>> ScoutPoint;

public:
    using Robot::Robot;
    virtual void look(int dx, int dy) = 0;

    bool isScoutBot = false;
    bool isTrackBot = false;
    bool isPredictBot = false;

    // bool getScoutBot() const { return isScoutBot; }
    bool getTrackBot() const { return isTrackBot; }
    bool getPredictBot() const { return isPredictBot; }

    // 使用构造函数设定 scout 类型
    SeeingRobot(bool isScout = false) : isScoutBot(isScout) {}

    // ✅ 重写虚函数
    bool isScout() const override { return isScoutBot; }
    int getScoutCount() const override { return scoutCount; }
    void setScoutCount(int c) override { scoutCount = c; }

   void addScoutPoint(pair<int, int> pos) override {
        ScoutPoint.push_back(pos);
    }


    const vector<pair<int, int>>& getScoutPoints() const override {
        return ScoutPoint;
    }

    // void addScoutPoint(int x, int y) {
    //     ScoutPoint.emplace_back(x, y);
    // }

    int getsize_ScoutPoints() const override{ return ScoutPoint.size(); }



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

class GenericRobot : public MovingRobot, public ShootingRobot, 
                    public SeeingRobot, public ThinkingRobot {
private:
    Battlefield* battlefield;
    int shells;
    bool selfDestructed;
    // int upgradeCount = 0;
    set<string> upgradedAreas;
    vector<string> upgradeNames;

   
protected:
    int upgradeCount = 0;
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

    // int getCoutUpgrade() const { return upgradeCount; }

};

#endif