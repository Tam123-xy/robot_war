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

#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <queue>
#include <random>
#include <algorithm>
#include <memory>
#include <vector>
#include <set>
#include <iostream>
#include <mutex>
using namespace std;

class Robot;
class HideBot;
class JumpBot;

class Battlefield {
private:
    int width, height;
    vector<shared_ptr<Robot>> robots;
    queue<shared_ptr<Robot>> respawnQueue;
    random_device rd;
    mt19937 gen;
    uniform_int_distribution<> xDist;
    uniform_int_distribution<> yDist;

    mutex respawnMutex;

    set<pair<int, int>> landmines;
    
    
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
    void processRespawn();
    void addToRespawn(shared_ptr<Robot> robot);



    // Action sequence methods
    void executeRobotTurn(shared_ptr<Robot> robot);
    // void executeAttack(GenericRobot* gr);
    // void executeConserve(GenericRobot* gr);
    // void executeExplore(GenericRobot* gr);

    void replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot);

    bool checkAttackHit(shared_ptr<Robot> attacker, shared_ptr<Robot> target);

    void addLandmine(int x, int y);
    bool checkLandmine(int x, int y) ;

//    void replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot) {
//         auto it = find(robots.begin(), robots.end(), oldBot);
//         if (it != robots.end()) *it = newBot;
//    }

    //GlideBot
    bool isInside(int x, int y) const { return x >= 1 && x <= width && y >= 1 && y <= height; }
};

#endif