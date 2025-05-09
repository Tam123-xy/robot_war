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

void One_Spaces_only(string& line);
string extractWord(const string& line, const int& substr, int& i);
void random_num(const string& value, int max, vector<int>& result);

int main() {
    ifstream file("set.txt");
    string line;
    vector<string> names;
    vector<int> initial_x;
    vector<int> initial_y;
    int M, N, steps = 0, num_robots = 0;
    srand(time(0));

    while (getline(file, line)) {
        One_Spaces_only(line);

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

            random_num(x, M - 1, initial_x);
            random_num(y, N - 1, initial_y);
        }
    }

    
    Battlefield field(M, N);

    if (num_robots != names.size()) {
        cout << "Robot count mismatch! Check your set.txt.\n";
        return 1;
    }

    for (int i = 0; i < num_robots; ++i) {
        auto r = std::make_shared<GenericRobot>(names[i], initial_x[i], initial_y[i], M, N, &field);
        field.addRobot(r);
    }

    for (int i = 0; i < steps; ++i) {
        cout << "\n--- Turn " << i + 1 << " ---\n";
        field.simulateTurn();
        
        if (field.isEmpty()) {
            cout << "All robots are destroyed. Simulation ends.\n";
            break;
        }
    
        field.display();
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

void random_num(const string& value, int max, vector<int>& result) {
    if (value == "random") {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, max);
        result.push_back(dist(gen));
    } else {
        result.push_back(stoi(value));
    }
}
