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

#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <queue>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>
#include <set>
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
    int countLiveRobot() const;


    // Action sequence methods
    void executeRobotTurn(shared_ptr<Robot> robot);
    void executeAttack(GenericRobot* gr);
    void executeConserve(GenericRobot* gr);
    void executeExplore(GenericRobot* gr);

    set<pair<int, int>> mines;
    void placeMineAt(int x, int y);
    bool checkMineAt(int x, int y) const;
    void triggerMineIfAny(Robot* robot, int x, int y);
};

#endif