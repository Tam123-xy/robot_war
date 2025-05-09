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
    void respawn(int x, int y);
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
    bool hasSelfDestructed;
    pair<int, int> lastShotTarget;
    
protected:
    bool hasLooked = false;
    bool hasFired = false;
    bool hasMoved = false;
    
    void resetTurn() {
        hasLooked = hasFired = hasMoved = false;
    }

public:
    GenericRobot(string name, int x, int y, int w, int h, Battlefield* bf);
    
    // Action methods
    void think() override;
    vector<string> look(int dx, int dy) override;
    void move(int dx, int dy) override;
    void fire(int dx, int dy) override;
    
    // State check methods
    bool canLook() const { return !hasLooked; }
    bool canFire() const { return !hasFired && shells > 0; }
    bool canMove() const { return !hasMoved; }
    
    // Other required methods
    void destroy();     
    int getX() const;   
    int getY() const; 
    pair<int, int> getLastShotTarget() const override;
    string getType() const override;
};
#endif