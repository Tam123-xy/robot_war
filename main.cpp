#include <iostream>
#include <fstream>
#include <vector>
#include <variant>
#include <string>
#include <sstream>
#include <random>
#include "battlefield.h"
using namespace std;
template <typename T>

// function prototype
void loop_vector(const vector<T>& v );
void One_Spaces_only(string& line); // make sure only ONE space in between word to word
void set(const string& line, int& M, int& N, int& steps, int& num_robots, vector<string>& name, vector<int>& initial_x, vector<int>& initial_y);
string extractWord(const string& line, const int&substr, int& i);
void ramdom_num(const string& xy, const int& MN, vector<int>& v );


int main(){
    ifstream file("set.txt");  // open file for reading
    string line;
    vector<string> name;
    vector<int> initial_x;
    vector<int> initial_y;
    int M, N, steps, num_robots=0;

    // loop line by line
    while (getline(file, line)) {

        One_Spaces_only(line);
        set(line, M, N, steps, num_robots, name, initial_x, initial_y );
    }

    if(num_robots < name.size()){
        cout << "You've set out of number of robots, please check you set.txt file again."<<endl;
        exit(-1);
    }

    cout<< "'"<<M<<"'"<<endl;
    cout<< "'"<<N<<"'"<<endl;
    cout<< "'"<<steps<<"'"<<endl;
    cout<< "'"<<num_robots<<"'"<<endl;
    loop_vector(name);
    loop_vector(initial_x);
    loop_vector(initial_y);

    file.close();
}

template <typename T>
void loop_vector(const vector<T>& v ){
    for (const auto& item : v) {
        cout << "'" << item << "'" << endl;
    }
}

void set(const string& line, int& M, int& N, int& steps, int& num_robots, vector<string>& name, vector<int>& initial_x, vector<int>& initial_y){

    if (line.substr(0, 9) == "M by N : ") {

        int i;
        M = stoi(extractWord(line,9, i));
        N = stoi(line.substr(i));

    } 
    
    else if (line.substr(0, 7) == "steps: "){
        steps = stoi(line.substr(7));
    }

    else if (line.substr(0, 8) == "robots: "){
        num_robots = stoi(line.substr(8));
    }

    else if (line.substr(0, 13) == "GenericRobot "){
        int i;
        name.push_back(extractWord(line,13,i));
        string x = extractWord(line,i,i);
        string y = line.substr(i);

        ramdom_num(x,M,initial_x);
        ramdom_num(y,N,initial_y);

    }

}

void One_Spaces_only(string& line) {
    istringstream iss(line);
    string word, result;

    while (iss >> word) {
        if (!result.empty()) {
            result += " ";
        }
        result += word;
    }

    line = result; 
}

string extractWord(const string& line,const int&substr, int& i){
    string word;

        for (char c : line.substr(substr)) {
            if (c == ' ') {
                break;
            }
            else{
                word += c;
            }
        }

        i = substr + word.size() + 1;

        return word;
}

void ramdom_num(const string& xy, const int& MN, vector<int>& v ){
    if(xy=="random"){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, MN);
        v.push_back(dist(gen));
    }
    else{
        v.push_back(stoi(xy));
    }
}