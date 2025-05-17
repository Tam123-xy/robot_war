#ifndef ROBOTS_H
#define ROBOTS_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <utility>
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
};

class MovingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void move(int dx, int dy) = 0;
};

class ShootingRobot : virtual public Robot {
public:
    pair<int, int> lastShotTarget;
    using Robot::Robot;
    virtual void fire(int dx, int dy) = 0;
    virtual pair<int, int> getLastShotTarget() const = 0;
};

class SeeingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual vector<string> look(int dx, int dy) = 0;
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
    pair<int, int> lastShotTarget;
    bool selfDestructed;
    
protected:
    bool hasLooked = false;
    bool hasFired = false;
    bool hasMoved = false;

public:
    GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf);
    
    // Action methods
    void resetTurn() {
        hasLooked = hasFired = hasMoved = false;
    }
    void think() override;
    vector<string> look(int dx, int dy) override;
    void move(int dx, int dy) override;
    void fire(int dx, int dy) override;
    vector<string> scout(int dx, int dy);
    
    // State check methods
    bool canLook() const { return !hasLooked; }
    bool canFire() const { return !hasFired && shells > 0; }
    bool canMove() const { return !hasMoved; }
    
    int getShells() const { return shells; }
    bool hasSelfDestructed() const { return selfDestructed;}
    void destroy();     
    bool shouldRespawn() const;
    virtual void respawn(int x, int y);

    int upgradeCount = 0;
    set<string> upgradedAreas;
    vector<string> upgradeNames;
    int jumpCount = 0;
    int hideCount = 0;
    int fireRange = 1;
    int visionRange = 1;
    bool semiAutoFire = false;

    void chooseUpgrade(Upgrade* upgrade);
    void showUpgrades() const;

    int getX() const;   
    int getY() const; 
    pair<int, int> getLastShotTarget() const override;
    string getType() const override;
};

class Upgrade {
public:
    virtual void apply(GenericRobot* robot) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0; // "Move", "Fire", "Look"
    virtual ~Upgrade() = default;
};

class JumpBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "JumpBot"; }
    std::string getType() const override { return "Move"; }
};

class HideBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "HideBot"; }
    std::string getType() const override { return "Move"; }
};

class LongShotBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "LongShotBot"; }
    std::string getType() const override { return "Fire"; }
};

class SemiAutoBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "SemiAutoBot"; }
    std::string getType() const override { return "Fire"; }
};

class ScoutBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "ScoutBot"; }
    std::string getType() const override { return "Look"; }
};

class TrackBot : public Upgrade {
public:
    void apply(GenericRobot* robot) override;
    std::string getName() const override { return "TrackBot"; }
    std::string getType() const override { return "Look"; }
};

#endif