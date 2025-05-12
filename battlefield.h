#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <queue>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include "robots.h"

class Battlefield {
private:
    int width, height;
    vector<shared_ptr<Robot>> robots;
    queue<shared_ptr<Robot>> respawnQueue;
    random_device rd;
    mt19937 gen;
    uniform_int_distribution<> xDist;
    uniform_int_distribution<> yDist;

    void processRespawn();
    
    
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


    // Action sequence methods
    void executeRobotTurn(shared_ptr<Robot> robot);
    void executeAttack(GenericRobot* gr);
    void executeConserve(GenericRobot* gr);
    void executeExplore(GenericRobot* gr);
};

#endif