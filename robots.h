#ifndef ROBOTS_H
#define ROBOTS_H

#include <string>
#include <memory>
#include <vector>
#include <utility> 

using namespace std;

class Battlefield;

class Robot {
protected:
    string name;
    int positionX, positionY;
    int width, height;
    int lives = 3;
    bool isAlive = true;
    bool inRespawnQueue = false;

public:
    Robot(string name, int x, int y, int w, int h)
        : name(name), positionX(x), positionY(y), width(w), height(h) {}

    virtual ~Robot() {}


    virtual string getType() const = 0;


    int getX() const { return positionX; }
    int getY() const { return positionY; }
    bool alive() const { return isAlive; }
    void destroy() {
        if (!isAlive) return;

        lives--;
        isAlive = false;

        if (lives > 0) {
            inRespawnQueue = true;
            cout << name << " destroyed! " << lives << " lives remain. Waiting to respawn.\n";
        } else {
            cout << name << " permanently destroyed!\n";
        }
    }

    void respawn(int x, int y) {
        positionX = x;
        positionY = y;
        isAlive = true;
        inRespawnQueue = false;
        cout << name << " respawned at (" << x << "," << y << ")\n";
    }
    bool shouldRespawn() const { return !isAlive && lives > 0 && inRespawnQueue; }

    string getName() const { return name; }
    int getLives() const { return lives; }
};

class MovingRobot : virtual public Robot {
public:
    using Robot::Robot;
    virtual void move(int dx, int dy) = 0;
};

class ShootingRobot : virtual public Robot {
protected:
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
    int shells;
    bool hasSelfDestructed = false;

public:
    GenericRobot(string name, int x, int y, int w, int h)
        : Robot(name, x, y, w, h), shells(10) {}

    string getType() const override { return "GenericRobot"; }

    void think() override {
        cout << name << " is deciding its actions..." << endl;
    }

    vector<string> look(int dx, int dy) override {
        if (abs(dx) > 1 || abs(dy) > 1) {
            return {"Cannot look that far!"};
        }

        return {"Scanning area at (" + to_string(getX()+dx) + "," + to_string(getY()+dy) + ")"};
    }

    void move(int dx, int dy) override {
        if (abs(dx) > 1 || abs(dy) > 1) {
            cout << name << " can only move to adjacent positions!" << endl;
            return;
        }

        int nx = getX() + dx;
        int ny = getY() + dy;

        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            positionX = nx;
            positionY = ny;
            cout << name << " moves to (" << positionX << "," << positionY << ")" << endl;
        } else {
            cout << name << " cannot move outside battlefield!" << endl;
        }
    }

    void fire(int dx, int dy) override {
        if (hasSelfDestructed) return;

        if (abs(dx) > 1 || abs(dy) > 1) {
            cout << name << " can only fire at adjacent positions!\n";
            return;
        }

        if (shells <= 0) {
            cout << name << " has no shells left! Self-destructing...\n";
            destroy();
            hasSelfDestructed = true;
            return;
        }

        shells--;
        lastShotTarget = {getX()+dx, getY()+dy};
        cout << name << " fires at (" << lastShotTarget.first
             << "," << lastShotTarget.second << ") (" << shells << " left)\n";
    }

    pair<int, int> getLastShotTarget() const override {
        return lastShotTarget;
    }
};

#endif
