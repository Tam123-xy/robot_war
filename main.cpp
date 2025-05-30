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

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime> 
#include "robots.h"
#include "battlefield.h"
using namespace std;

GenericRobot::GenericRobot(const string& name, int x, int y, int w, int h, Battlefield* bf)
    : Robot(name, x, y, w, h, bf),
      MovingRobot(name, x, y, w, h, bf),
      ShootingRobot(name, x, y, w, h, bf),
      SeeingRobot(name, x, y, w, h, bf),
      ThinkingRobot(name, x, y, w, h, bf),
      battlefield(bf),
      shells(10),
      selfDestructed(false) {
        //cout << "GenericRobot " << name << " created at (" << x << "," << y << ")" << endl;
}

template<typename T>
shared_ptr<T> GenericRobot::createUpgradedBot() {
        auto newBot = make_shared<T>(
            name, 
            getX(), 
            getY(),
            getWidth(),
            getHeight(),
            battlefield
        );
        return newBot;
    }

Battlefield::Battlefield(int w, int h)
    : width(w), height(h), gen(rd()), xDist(1, w), yDist(1, h) {}

shared_ptr<Robot> Battlefield::findRobotAt(int x, int y) {
    for (auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y && robot->alive()) {
            return robot;
        }
    }
    return nullptr;
}

void GenericRobot::think()
{
    cout << name << " is thinking... " << endl;
}

void GenericRobot::look(int dx, int dy)
{
    hasLooked = true;

    int centerX = getX();
    int centerY = getY();

    cout << name << " is now at (" << centerX << "," << centerY << "), looking around ..." << endl;

    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {

            int lookX = centerX + dx;
            int lookY = centerY + dy;

            string status;

            // Robot itself point
            if (dx == 0 && dy == 0)
                continue;

            // Out of bounds
            else if (lookX <= 0 || lookY <= 0 || lookX > battlefield->getWidth() || lookY > battlefield->getHeight())
                continue;

            // Enemy robot
            else if (battlefield->isRobotAt(lookX, lookY))
            {
                auto enemy = battlefield->findRobotAt(lookX, lookY);
                status = enemy->getName();
                lookGot_enemy_point.push_back({lookX, lookY});
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
            }

            // Empty space
            else
            {
                status = "Empty space";
                empty_point.push_back({lookX, lookY});
                cout << "(" + to_string(lookX) + "," + to_string(lookY) + "): " + status << endl;
            }
        }
    }
}

void Robot::destroy()
{
    if (isAlive)
    {
        lives--;

        isAlive = false;
        setPosition(0, 0); // Move to outside battle field

        if (lives > 0)
        {
            cout << name << " is waiting to respawn (Lives remaining: " << lives << "/3)" << endl;
            battlefield->addToRespawn(shared_from_this());
        }
        else
        {
            cout << name << " has no lives remaining! (Lives remaining: " << lives << "/3)" << endl;
        }
    }
}

void Robot::respawn(int x, int y)
{
    if (lives > 0)
    {
        positionX = x;
        positionY = y;
        isAlive = true;
        init_Upgrade();
        cout << name << " respawned at (" << x << "," << y << ") (Lives remaining: " << lives << "/3)" << endl;
    }
}

bool Robot::shouldRespawn() const
{
    return !isAlive && lives > 0;
}

string GenericRobot::getType() const
{
    return "GenericRobot";
}

void GenericRobot::move(int dx, int dy)
{
    hasMoved = true;
    int newX, newY;

    // move -> look, surrounding POINTS --> (is occupied/ move)
    if (!hasLooked)
    {
        bool track_move = true;

        surrouding_point_TARGET(newX, newY);

        if (battlefield->isRobotAt(newX, newY))
        {
            auto enemy = battlefield->findRobotAt(newX, newY);
            cout << name << " cannot move to (" << newX << "," << newY << "). This point is occupied by " << enemy->getName() << "." << endl;
        }
        else
        {
            setPosition(newX, newY);
            cout << name << " moved to (" << newX << "," << newY << ")." << endl;
        }
    }

    // look -> move , empty POINTS --> (no point/ move)
    else if (hasLooked)
    {

        if (empty_point.empty())
        {
            cout << name << " didn't find any empty point to move! " << name << " may be surrounded!" << endl;
            return;
        }

        else
        {
            uniform_int_distribution<> dis(0, empty_point.size() - 1);
            int num = dis(gen);
            newX = empty_point[num].first;
            newY = empty_point[num].second;
        }

        setPosition(newX, newY);
        cout << name << " moved to (" << newX << "," << newY << ")." << endl;
    }

    if (battlefield->checkLandmine(newX, newY))
    {
        if (rand() % 100 < 50)
        {
            cout << name << " triggered a landmine at ("
                 << newX << "," << newY << ")!\n";
            destroy();
        }
        else
        {
            cout << name << " narrowly avoided a landmine at ("
                 << newX << "," << newY << ")!\n";
        }
    }
}

void GenericRobot::fire(int dx, int dy)
{
    if (shells == 0)
    {
        cout << name << " has no shells left! Self-destructing..." << endl;
        selfDestructed = true;
        destroy();
        return;
    }

    int targetX;
    int targetY;

    // fire --> look, surrounding POINTS --> (shot no enemy/ shot enemy)
    if (hasLooked == false)
    {
        surrouding_point_TARGET(targetX, targetY);
    }

    // look --> fire, enemy POINTS --> (NO shot no enemy/ shot enemy)
    // ScoutBot
    else if (hasLooked == true)
    {
        hasFired = true;
        int cout_enemy = lookGot_enemy_point.size();

        // NO shot no enemy, return back
        if (cout_enemy == 0)
        {
            cout << "Preserving shell for next turn since " << name << " didn't find any robots around. (left shells: " << shells << ")" << endl;
            return;
        }

        else if (cout_enemy == 1)
        {
            targetX = lookGot_enemy_point[0].first;
            targetY = lookGot_enemy_point[0].second;

            int curX = getX();
            int curY = getY();

            bool isNearby = false;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = curX + dx;
                    int ny = curY + dy;

                    if (nx == targetX && ny == targetY) {
                        isNearby = true;
                        break;
                    }
                }
                if (isNearby) break;
            }

            if (!isNearby) {
                cout << name << " can't find the previous target after moving. Skipping fire to save shell." << endl;
                return;
            }
        }

        // many enemies, need to check which is the higher level enemy
        else
        {
            shot_higher_enemy(targetX, targetY, lookGot_enemy_point);
            // shot_higher_enemy(targetX, targetY,lookGot_enemy_point,false);
        }
    }

    // shot robot
    if (battlefield->findRobotAt(targetX, targetY))
    {
        auto enemy = battlefield->findRobotAt(targetX, targetY);
        uniform_int_distribution<> dis(0, 99);
        shells--;

        cout << name << " fires " << enemy->getName() << " at (" << targetX << "," << targetY << ")";
        cout << " (Left shells: " << shells << "/10)" << endl;

        if (enemy->isHidden())
        {
            cout << "Attack missed! " << enemy->getName() << " is hidden!" << endl;
            return;
        }

        if (dis(gen) < 70)
        {
            cout << "Target hit! " << enemy->getName() << " has been destroyed! ";
            enemy->destroy();
            chooseUpgrade(); // Upgrade
        }

        else
        {
            cout << " - MISS!" << endl;
        }
    }

    // Shot no enemy
    else
    {
        shells--;
        cout << name << " fires at (" << targetX << "," << targetY << "). But it is an empty space!";
        cout << " (Left shells: " << shells << "/10)" << endl;
    }
}

void GenericRobot::respawn(int x, int y)
{
    Robot::respawn(x, y);
    if (alive())
    {
        shells = 10;
        selfDestructed = false;
        resetTurn();
    }
}

void GenericRobot::destroy()
{
    if (!selfDestructed)
    {
        selfDestructed = true;
        Robot::destroy();
        upgradedAreas.clear();
        upgradeNames.clear();
    }
}

bool GenericRobot::shouldRespawn() const
{
    return !isAlive && lives > 0;
}

int GenericRobot::getX() const
{
    return positionX;
}

int GenericRobot::getY() const
{
    return positionY;
}

void GenericRobot::chooseUpgrade()
{
    if (upgradeCount == 3)
    {
        string sentence = name + " is " + upgradeNames[0];
        int size = upgradeNames.size();
        for (size_t i = 1; size < i; i++)
        {
            sentence += "," + upgradeNames[i];
        }
        sentence += ". Cannot upgrade anymore, max upgrade 3 times";
        cout << sentence << endl;
        return;
    }

    vector<int> availableOptions;
    if (upgradedAreas.find("move") == upgradedAreas.end())
        availableOptions.push_back(0);
    if (upgradedAreas.find("shoot") == upgradedAreas.end())
        availableOptions.push_back(1);
    if (upgradedAreas.find("see") == upgradedAreas.end())
        availableOptions.push_back(2);

    if (availableOptions.empty())
    {
        cout << name << " has no more areas to upgrade!" << endl;
        return;
    }

    int randomIndex = rand() % availableOptions.size();
    int chosenOption = availableOptions[randomIndex];
    chooseUpgrade(chosenOption);
}

void GenericRobot::chooseUpgrade(int upgradeOption)
{
    if (upgradeCount >= 3) {
        cout << name << " cannot upgrade anymore (max 3 upgrades reached)" << endl;
        return;
    }

    const char *area = "";
    switch (upgradeOption)
    {
    case 0:
        area = "move";
        break;
    case 1:
        area = "shoot";
        break;
    case 2:
        area = "see";
        break;
    default:
        cout << "Invalid upgrade option: " << upgradeOption << endl;
        return;
    }

    if (upgradedAreas.find(area) != upgradedAreas.end())
    {
        cout << name << " already upgraded " << area << " area!" << endl;
        return;
    }

    auto self = shared_from_this();
    shared_ptr<GenericRobot> newBot;
    string upgradeName = "";
    
    
    switch (upgradeOption)
    {
    case 0: // Moving upgrade
    {
        int choice = rand() % 3;
        if (choice == 0)
        {
            upgradeName = "HideBot";
            newBot = createUpgradedBot<HideBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "JumpBot";
            newBot = createUpgradedBot<JumpBot>();
        }
        else
        {
            upgradeName = "GlideBot";
            newBot = createUpgradedBot<GlideBot>();
        }
        cout << name << " upgraded movement: " << upgradeName << endl;
    }
    break;

    case 1: // Shooting upgrade
    {
        int choice = rand() % 4;
        if (choice == 0)
        {
            upgradeName = "LongShotBot";
            newBot = createUpgradedBot<LongShotBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "SemiAutoBot";
            newBot = createUpgradedBot<SemiAutoBot>();
        }
        else if (choice == 2)
        {
            upgradeName = "ThirtyShotBot";
            newBot = createUpgradedBot<ThirtyShotBot>();
        }
        else
        {
            upgradeName = "LandmineBot";
            newBot = createUpgradedBot<LandmineBot>();
        }
        cout << name << " upgraded shooting: " << upgradeName << endl;
    }
    break;

    case 2: // Seeing upgrade
    {
        int choice = rand() % 3;
        if (choice == 0)
        {
            upgradeName = "ScoutBot";
            newBot = createUpgradedBot<ScoutBot>();
        }
        else if (choice == 1)
        {
            upgradeName = "TrackBot";
            newBot = createUpgradedBot<TrackBot>();
        }
        else
        {
            upgradeName = "RevealBot";
            newBot = createUpgradedBot<RevealBot>();
        }
        cout << name << " upgraded vision: " << upgradeName << endl;
    }
    break;
    }

    if (newBot)
    {       
        upgradeNames.push_back(upgradeName);
        upgradedAreas.insert(area);
        upgradeCount++;

        // Transfer all state to new robot
        newBot->upgradeNames = this->upgradeNames;
        newBot->upgradedAreas = this->upgradedAreas;
        newBot->upgradeCount = this->upgradeCount;
        newBot->name = this->name;

        battlefield->replaceRobot(self, newBot);

        string sentence = name + " now has upgrades: " + upgradeNames[0];
        int size = upgradeNames.size();
        for (size_t i = 1; i < size; i++)
        {
            sentence += ", " + upgradeNames[i];
        }

        cout << sentence << " (Total: " << upgradeCount << "/3)" << endl;

        if (upgradeCount >= 2)
        {
            newBot->replaceWithCombination(newBot->upgradeNames);
        }
    }
    else
    {
        cout << "Failed to create upgraded robot for " << upgradeName << endl;
    }
}

void GenericRobot::replaceWithCombination(const vector<string> &types)
{
    if (types.size() < 2)
        return;

    auto self = shared_from_this();
    shared_ptr<GenericRobot> newBot;
    string combinationName = "";

    auto hasType = [&](const string &type)
    {
        return find(types.begin(), types.end(), type) != types.end();
    };

    cout << "Attempting combination with " << types.size() << " upgrades: ";
    for (const auto &t : types)
        cout << t << " ";
    cout << endl;

    if (types.size() == 2)
    {
        // Movement + Shooting combinations
        if (hasType("HideBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<HideLongShotBot>();
            combinationName = "HideLongShotBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoBot>();
            combinationName = "HideSemiAutoBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotBot>();
            combinationName = "HideThirtyShotBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<HideLandmineBot>();
            combinationName = "HideLandmineBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<JumpLongShotBot>();
            combinationName = "JumpLongShotBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoBot>();
            combinationName = "JumpSemiAutoBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotBot>();
            combinationName = "JumpThirtyShotBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<JumpLandmineBot>();
            combinationName = "JumpLandmineBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot"))
        {
            newBot = createUpgradedBot<GlideLongShotBot>();
            combinationName = "GlideLongShotBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoBot>();
            combinationName = "GlideSemiAutoBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotBot>();
            combinationName = "GlideThirtyShotBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot"))
        {
            newBot = createUpgradedBot<GlideLandmineBot>();
            combinationName = "GlideLandmineBot";
        }
        // Movement + Vision combinations
        else if (hasType("HideBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideScoutBot>();
            combinationName = "HideScoutBot";
        }
        else if (hasType("HideBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideTrackBot>();
            combinationName = "HideTrackBot";
        }
        else if (hasType("HideBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideRevealBot>();
            combinationName = "HideRevealBot";
        }
        else if (hasType("JumpBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpScoutBot>();
            combinationName = "JumpScoutBot";
        }
        else if (hasType("JumpBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpTrackBot>();
            combinationName = "JumpTrackBot";
        }
        else if (hasType("JumpBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpRevealBot>();
            combinationName = "JumpRevealBot";
        }
        else if (hasType("GlideBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideScoutBot>();
            combinationName = "GlideScoutBot";
        }
        else if (hasType("GlideBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideTrackBot>();
            combinationName = "GlideTrackBot";
        }
        else if (hasType("GlideBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideRevealBot>();
            combinationName = "GlideRevealBot";
        }
        // Shooting + Vision combinations
        else if (hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<LongShotScoutBot>();
            combinationName = "LongShotScoutBot";
        }
        else if (hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<LongShotTrackBot>();
            combinationName = "LongShotTrackBot";
        }
        else if (hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<LongShotRevealBot>();
            combinationName = "LongShotRevealBot";
        }
        else if (hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<SemiAutoScoutBot>();
            combinationName = "SemiAutoScoutBot";
        }
        else if (hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<SemiAutoTrackBot>();
            combinationName = "SemiAutoTrackBot";
        }
        else if (hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<SemiAutoRevealBot>();
            combinationName = "SemiAutoRevealBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<ThirtyShotScoutBot>();
            combinationName = "ThirtyShotScoutBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<ThirtyShotTrackBot>();
            combinationName = "ThirtyShotTrackBot";
        }
        else if (hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<ThirtyShotRevealBot>();
            combinationName = "ThirtyShotRevealBot";
        }
        else if (hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<LandmineScoutBot>();
            combinationName = "LandmineScoutBot";
        }
        else if (hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<LandmineTrackBot>();
            combinationName = "LandmineTrackBot";
        }
        else if (hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<LandmineRevealBot>();
            combinationName = "LandmineRevealBot";
        }
    }
    else if (types.size() == 3)
    {
        if (hasType("HideBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideLongShotScoutBot>();
            combinationName = "HideLongShotScoutBot";
        }
        else if (hasType("HideBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideLongShotTrackBot>();
            combinationName = "HideLongShotTrackBot";
        }
        else if (hasType("HideBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideLongShotRevealBot>();
            combinationName = "HideLongShotRevealBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoScoutBot>();
            combinationName = "HideSemiAutoScoutBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoTrackBot>();
            combinationName = "HideSemiAutoTrackBot";
        }
        else if (hasType("HideBot") && hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideSemiAutoRevealBot>();
            combinationName = "HideSemiAutoRevealBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotScoutBot>();
            combinationName = "HideThirtyShotScoutBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotTrackBot>();
            combinationName = "HideThirtyShotTrackBot";
        }
        else if (hasType("HideBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideThirtyShotRevealBot>();
            combinationName = "HideThirtyShotRevealBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<HideLandmineScoutBot>();
            combinationName = "HideLandmineScoutBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<HideLandmineTrackBot>();
            combinationName = "HideLandmineTrackBot";
        }
        else if (hasType("HideBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<HideLandmineRevealBot>();
            combinationName = "HideLandmineRevealBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpLongShotRevealBot>();
            combinationName = "JumpLongShotScoutBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpLongShotTrackBot>();
            combinationName = "JumpLongShotTrackBot";
        }
        else if (hasType("JumpBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpLongShotRevealBot>();
            combinationName = "JumpLongShotRevealBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoScoutBot>();
            combinationName = "JumpSemiAutoScoutBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoTrackBot>();
            combinationName = "JumpSemiAutoTrackBot";
        }
        else if (hasType("JumpBot") && hasType("SemiAutoBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpSemiAutoRevealBot>();
            combinationName = "JumpSemiAutoRevealBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotScoutBot>();
            combinationName = "JumpThirtyShotScoutBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotTrackBot>();
            combinationName = "JumpThirtyShotTrackBot";
        }
        else if (hasType("JumpBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpThirtyShotRevealBot>();
            combinationName = "JumpThirtyShotRevealBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<JumpLandmineScoutBot>();
            combinationName = "JumpLandmineScoutBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<JumpLandmineTrackBot>();
            combinationName = "JumpLandmineTrackBot";
        }
        else if (hasType("JumpBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<JumpLandmineRevealBot>();
            combinationName = "JumpLandmineRevealBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideLongShotScoutBot>();
            combinationName = "GlideLongShotScoutBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideLongShotTrackBot>();
            combinationName = "GlideLongShotTrackBot";
        }
        else if (hasType("GlideBot") && hasType("LongShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideLongShotRevealBot>();
            combinationName = "GlideLongShotRevealBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoScoutBot>();
            combinationName = "GlideSemiAutoScoutBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideSemiAutoTrackBot>();
            combinationName = "GlideSemiAutoTrackBot";
        }
        else if (hasType("GlideBot") && hasType("SemiAutoBot") && hasType("Reveal"))
        {
            newBot = createUpgradedBot<GlideSemiAutoRevealBot>();
            combinationName = "GlideSemiAutoRevealBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotScoutBot>();
            combinationName = "GlideThirtyShotScoutBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotTrackBot>();
            combinationName = "GlideThirtyShotTrackBot";
        }
        else if (hasType("GlideBot") && hasType("ThirtyShotBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideThirtyShotRevealBot>();
            combinationName = "GlideThirtyShotRevealBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("ScoutBot"))
        {
            newBot = createUpgradedBot<GlideLandmineScoutBot>();
            combinationName = "GlideLandmineScoutBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("TrackBot"))
        {
            newBot = createUpgradedBot<GlideLandmineTrackBot>();
            combinationName = "GlideLandmineTrackBot";
        }
        else if (hasType("GlideBot") && hasType("LandmineBot") && hasType("RevealBot"))
        {
            newBot = createUpgradedBot<GlideLandmineRevealBot>();
            combinationName = "GlideLandmineRevealBot";
        }
    }

    if (newBot && !combinationName.empty())
    {

        newBot->upgradeNames = this->upgradeNames;
        newBot->upgradedAreas = this->upgradedAreas;
        newBot->upgradeCount = this->upgradeCount;
        newBot->name = this->name;

        battlefield->replaceRobot(self, newBot);
        cout << "Successfully combined " << name << " into " << combinationName << "!" << endl;
    }
    else
    {
        cout << name << " upgrade combination not found for: ";
        for (const auto &s : types)
            cout << s << ' ';
        cout << "\nUsing individual upgrades instead." << endl;
    }
    
}

int Battlefield::getWidth() const {
    return width;
}

int Battlefield::getHeight() const {
    return height;
}

bool Battlefield::isRobotAt(int x, int y) const {
    for (const auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y) {
            return true;  // Robot found at the position
        }
    }
    return false;  // No robot at the position
}

void Battlefield::addRobot(shared_ptr<Robot> robot) {
    robots.push_back(robot);
}

bool Battlefield::isEnemyAt(int x, int y) const {
    for (const auto& robot : robots) {
        if (robot->getX() == x && robot->getY() == y && robot->alive()) {
            return true;
        }
    }
    return false;
}

bool Battlefield::isEmpty() const {
    return robots.empty() && respawnQueue.empty();
}

int Battlefield::countAliveRobots() const {
    int count = 0;
    for (const auto& robot : robots) {
        if (robot->alive()) count++;
    }
    return count;
}

Robot* Battlefield::getAliveRobot() const {
    for (const auto& robot : robots) {
        if (robot->alive()) return robot.get();
    }
    return nullptr;
}

int Battlefield::countLiveRobot() const {
    int count = 0;
    for (const auto& robot : robots) {
        if (!robot->alive()){
            count+= robot->getLives();
        }
    }
    return count;
}

void Battlefield::addToRespawn(shared_ptr<Robot> robot) {
        lock_guard<mutex> lock(respawnMutex);
        respawnQueue.push(robot);
}

bool Battlefield::checkAttackHit(shared_ptr<Robot> attacker, 
                               shared_ptr<Robot> target) {
    if (auto hideBot = dynamic_cast<HideBot*>(target.get())) {
        return !hideBot->isHidden();
    }
    return true;
}

void Battlefield::addLandmine(int x, int y) {
        landmines.emplace(x, y);
}

bool Battlefield::checkLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
        landmines.erase(it); 
        if (rand() % 100 < 50) { 
            cout << "Landmine triggered at (" << x << "," << y << ")!\n";
            return true;
        }
    }
    return false;
}

bool Battlefield::isLandmine(int x, int y) {
    auto it = landmines.find({x, y});
    if (it != landmines.end()) {
            return true;
    }
    return false;
}

void Battlefield::simulateTurn() {
    processRespawn();
    bool simulation = true;

    // Shuffle robots for random turn order
    shuffle(robots.begin(), robots.end(), gen);

    // Print order robot
    vector<shared_ptr<Robot>> copy = robots;  
    copy.erase( // Store the robot which are alive in this turn
        remove_if(copy.begin(), copy.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive();
            }),
        copy.end()
    );

    string r_order = "\nRobot's order: " + copy[0] -> getName();

    int size = copy.size();
    for(int i=1; i<size; i++){
        r_order+= " --> " + copy[i]-> getName();
    }
    cout << r_order << endl; 
    // cout << endl;

    for(auto& robot : copy){
        if(robot->alive()){
            executeRobotTurn(robot,copy);
        }

        else if(robot->shouldRespawn()){
            auto it = find(copy.begin(), copy.end(), robot);
            if (it != copy.end()) {
                cout << "\nSkipping " << robot->getName() << " because it died in this turn." << endl;
                cout<<endl;
            }
        }
    }

    // Remove dead robots with no lives left
    robots.erase(
        remove_if(robots.begin(), robots.end(),
            [](const shared_ptr<Robot>& r) { 
                return !r->alive() && r->getLives() <= 0;
            }),
        robots.end()
    );
}

void Battlefield::processRespawn() {

    // Got robot that needs to respawn
    if (!respawnQueue.empty()) {

        // Print respawn order
        queue<shared_ptr<Robot>> tempQueue = respawnQueue; // Copy queue respawnQueue
        auto copy_robot = tempQueue.front();
        tempQueue.pop(); 
        string respawn_order = "Respawn robots queue: " + copy_robot -> getName();
        while (!tempQueue.empty()) {
            shared_ptr<Robot> robot = tempQueue.front();
            respawn_order+= " --> " + robot->getName();
            tempQueue.pop();
        }
        cout << respawn_order << endl; 
        // cout << endl;

        // ProcessRespawn
        auto robot = respawnQueue.front();
        respawnQueue.pop();
        
        // check robot has live
        if (robot->getLives() > 0) {
            int x, y;
            int attempts = 0;
            do {
                x = xDist(gen); // Generate random point x
                y = yDist(gen); // Generate random point x
                if (++attempts > 100) {
                    cout << "Couldn't find empty spot for " << robot->getName() << endl;
                    respawnQueue.push(robot);  // Retry next turn
                    return;
                }
            } while (findRobotAt(x, y));
            
            robot->respawn(x, y);
            // display();
        }
    }

    // lock_guard<mutex> lock(respawnMutex);
    
    // // Got robot that needs to respawn
    // if (!respawnQueue.empty()) {

    //     // Print respawn order
    //     queue<shared_ptr<Robot>> tempQueue = respawnQueue; // Copy queue respawnQueue
    //     auto copy_robot = tempQueue.front();
    //     tempQueue.pop(); 
    //     string respawn_order = "Respawn robots queue: " + copy_robot -> getName();
    //     while (!tempQueue.empty()) {
    //         shared_ptr<Robot> robot = tempQueue.front();
    //         respawn_order+= "--> " + robot->getName();
    //         tempQueue.pop();
    //     }
    //     cout << respawn_order<< endl; 

    //     // ProcessRespawn
    //     auto robot = respawnQueue.front();
    //     respawnQueue.pop();

    //     int remainingLives = robot->getLives();
    //     if (remainingLives <= 0) return; 
        
    //     // check robot has live
    //     if (robot->getLives() > 0) {
    //         int newX, newY;
    //         int attempts = 0;
    //         do {
    //             newX = 1 + rand() % width;  
    //             newY = 1 + rand() % height;  
    //             newX = 1 + rand() % width;  
    //             newY = 1 + rand() % height;  
    //             if (++attempts > 100) {
    //                 cout << "Couldn't find empty spot for " << robot->getName() << endl;
    //                 respawnQueue.push(robot);  // Retry next turn
    //                 return;
    //             }
    //         } while (findRobotAt(newX,newY));
            
    //         auto gr = make_shared<GenericRobot>(
    //             robot->getName(), newX, newY, width, height, this
    //         );
    //         gr->setLives(remainingLives);
            
    //         replaceRobot(robot, gr);
    //         gr->respawn(newX, newY);
            
    //         // display();
    //     }
    // }
}

void Battlefield::executeRobotTurn(shared_ptr<Robot> robot, vector<shared_ptr<Robot>> copy) {
    if (!robot->alive()) return;  // Skip dead robots
    if (auto trackBot = dynamic_cast<TrackBot*>(robot.get())) {
        trackBot->displayTracked();
    }
    robot->resetTurn();

    // Create all possible action permutations
    const vector<vector<string>> actionOrders = {
        {"look", "fire", "move", "think"},
        {"look", "fire", "think", "move"},
        {"look", "move", "fire", "think"},
        {"look", "move", "think", "fire"},
        {"look", "think", "fire", "move"},
        {"look", "think", "move", "fire"},

        {"fire", "look", "move", "think"},
        {"fire", "look", "think", "move"},
        {"fire", "move", "look", "think"},
        {"fire", "move", "think", "look"},
        {"fire", "think", "look", "move"},
        {"fire", "think", "move", "look"},

        {"move", "look", "fire", "think"},
        {"move", "look", "think", "fire"},
        {"move", "fire", "look", "think"},
        {"move", "fire", "think", "look"},
        {"move", "think", "look", "fire"},
        {"move", "think", "fire", "look"},

    };

    // Cout order action
    auto& order = actionOrders[rand() % actionOrders.size()];
    cout << endl << robot->getName() << "'s action order is " << order[0] << " --> "<< order[1] << " --> " << order[2] << " --> " << order[3] << endl;

    for (const auto& action : order){
        int dx,dy;
        if (action == "look") {
            robot->look(dx, dy);
        }

        else if (action == "fire"){
            robot->fire(dx, dy);
        }
        
        else if (action == "move"){
            robot->move(rand() % 3 - 1, rand() % 3 - 1);
            display();

        }
        else{
            robot->think();
        }
    }

    // Handle destruction if out of shells
    if (robot->getShells() <= 0 && !robot->hasSelfDestructed()) {
        robot->destroy();
        if (robot->shouldRespawn()) {
            respawnQueue.push(shared_ptr<Robot>(robot));
        }
    }
}

void Battlefield::replaceRobot(shared_ptr<Robot> oldBot, shared_ptr<Robot> newBot) {
    // Copy important state from old robot to new robot
    newBot->setPosition(oldBot->getX(), oldBot->getY());
    newBot->setLives(oldBot->getLives()); 
    newBot->setShells(oldBot->getShells());
    
    // Find and replace the robot in the vector
    auto it = find(robots.begin(), robots.end(), oldBot);
    if (it != robots.end()) {
        *it = newBot;  // Replace the robot
        // Don't erase it after replacing! This was the bug
        // robots.erase(it);  // Remove this line
        cout << "Robot successfully replaced in battlefield" << endl;
    } else {
        cout << "Warning: Could not find robot to replace in battlefield" << endl;
    }
}

void Battlefield::display() {
    vector<vector<char>> grid(height, vector<char>(height, '.')); 

    for (const auto& robot : robots) {
        if (robot->alive()) {
            grid[robot->getY()-1][robot->getX()-1] = robot->getName()[0];
        }
    }

    cout << "--- Battlefield Status ---\n";
    for (int i = 0 ; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cout << grid[i][j] << ' ';
        }
        cout << endl;
    }

}

int check_point(const vector<int>& point, int &max_height ); // Check if the set point is not out of bounds
void One_Spaces_only(string& line); // Leave only one space between each word.
string extractWord(const string& line, const int& substr, int& i);
void parse_or_random(const string& value, int max, vector<int>& result); // check if it is a number or what the system generate a random number

int main() {
    ifstream file("set.txt");
    string line;
    vector<string> names;
    vector<int> initial_x;
    vector<int> initial_y;
    int M, N, steps = 0, num_robots = 0;
    srand(time(0));

    while (getline(file, line)) {
        One_Spaces_only(line); // Leave only one space between each word.

        if (line.rfind("M by N :", 0) == 0) {
            int i;
            M = stoi(extractWord(line, 9, i)); 
            N = stoi(line.substr(i));          
        } else if (line.rfind("steps:", 0) == 0) {
            steps = stoi(line.substr(7));      
        } else if (line.rfind("robots:", 0) == 0) {
            num_robots = stoi(line.substr(8));
        } else if (line.rfind("GenericRobot", 0) == 0) {
            int i;
            names.push_back(extractWord(line, 13, i)); 
            string x = extractWord(line, i, i);
            string y = line.substr(i);

            
            parse_or_random(x, M, initial_x); // Check if it is a number or what the system generate a random number
            parse_or_random(y, N, initial_y);
            
        }
    }

    // Check if the number of robot is not equal to the number of setting robot
    if (num_robots != names.size()) {
        cout << "Robot count mismatch! Check your set.txt.\n";
        return -1;
    }

    // Check if the set point is not out of bounds
    int x_OutOfBound = check_point(initial_x,M);
    if(x_OutOfBound != -1){
        cout << "You have assigned robot " << names[x_OutOfBound] << " to an out-of-bounds point!";
        return -1;
    }

    // Check if the set point is not out of bounds
    int y_OutOfBound = check_point(initial_y,N);
    if(y_OutOfBound != -1){
        cout << "You have assigned robot " << names[y_OutOfBound] << " to an out-of-bounds point!";
        return -1;
    }

    Battlefield field(M, N);

    for (int i = 0; i < num_robots; ++i) {
        auto r = std::make_shared<GenericRobot>(names[i], initial_x[i], initial_y[i], M, N, &field);
        field.addRobot(r);
    }


    for (int i = 0; i < steps; ++i) {
        cout << "\n--- Turn " << i + 1 << " ---\n";
        field.display();
        field.simulateTurn();
        
        // Field got 0 alive robot && the total live of dead robots is 0
        if (field.isEmpty() && field.countLiveRobot() == 0) {
            cout << "All robots are destroyed. Simulation ends.\n";
            break;
        }

        field.display();
        cout << "\n--- Turn " << i + 1 << " END---\n";

        // Field got 1 alive robot && the total live of dead robots is 0
        if (field.countAliveRobots() == 1 && field.countLiveRobot() ==0) {
            cout << "Simulation ends! ";
            auto winner = field.getAliveRobot();
            cout << "Winner: " << winner->getName() << endl;
            // if (winner) {
            //     cout << "Winner: " << winner->getName() << endl;
            // } else {
            //     cout << "No robot survived." << endl;
            // }
            break;
        }
    }

    return 0;
}

void One_Spaces_only(string& line) {
    istringstream iss(line);
    string word, result;
    while (iss >> word) {
        if (!result.empty()) result += " ";
        result += word;
    }
    line = result;
}

string extractWord(const string& line, const int& substr, int& i) {
    string word;
    for (char c : line.substr(substr)) {
        if (c == ' ') break;
        word += c;
    }
    i = substr + word.size() + 1;
    return word;
}

void parse_or_random(const string& value, int max, vector<int>& result) {
    if (value == "random") {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(1, max);
        result.push_back(dist(gen));
    } else {
        result.push_back(stoi(value));
    }
}

int check_point(const vector<int>& point, int &max_height ){
    for(int i=0; i< point.size(); i++){
        if(point[i]> max_height|| point[i] <= 0){
            return i;
        }
    }
    return -1;
}