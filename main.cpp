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

int check_point(const vector<int>& point, int &max_height ); // Check if the set point is not out of bounds
void One_Spaces_only(string& line); // Leave only one space between each word.
string extractWord(const string& line, const int& substr, int& i);
void parse_or_random(const string& value, int max, vector<int>& result); // check if it is a number or what the system generate a random number

int main() {
    ifstream file("set.txt");
    string line;
    vector<string> names;
    vector<string> names;
    vector<int> initial_x;
    vector<int> initial_y;
    int M, N, steps = 0, num_robots = 0;
    srand(time(0));
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
        if (!result.empty()) result += " ";
        result += word;
    }
    line = result;
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
        uniform_int_distribution<> dist(1, max);
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

int check_point(const vector<int>& point, int &max_height ){
    for(int i=0; i< point.size(); i++){
        if(point[i]> max_height|| point[i] <= 0){
            return i;
        }
    }
    return -1;
}
