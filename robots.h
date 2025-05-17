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
    int lives = 2;
    bool isAlive = true;
    bool inRespawnQueue = false;
    int positionX, positionY;  // Changed from private to protected
    int width, height;

    int upgrade_times = 0;
    vector<string> upgrade_bot;
    bool upgrade_move=false;
    bool upgrade_shoot=false;
    bool upgrade_see=false;


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
    void upgrade();
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
    virtual void look(int dx, int dy) =0;
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

    int getX() const;   
    int getY() const; 
    pair<int, int> getLastShotTarget() const override;
    string getType() const override;
};

class UpGrade_move : public GenericRobot{
protected:
    vector<string>move_type={"HideBot","Jumbot"};
    bool hide = false;
    int hide_times = 3;

public:
    void HideBot();
    void Jumbot();
};
#endif