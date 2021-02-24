#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<string> parse_line(const string& line) {
    int left = -1;
    int right = 0;
    int len = line.length();
    int i = 0;
    vector<string> strs;

    while(i <= len) {
        if( i == len || line.at(i) == ','){
          strs.emplace_back(line.substr(left, right-left));  
          left = -1;      
        }
        else if (line.at(i) != ' '){
          if (left == -1){
            left = i;
          }
          right = i+1;
        }
        
        i++;
    }
    return strs;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "file name is not specified\n";
        return -1;
    }

    string name = argv[1];

    fstream ost;
    ost.open(name, fstream::in);
    if(!ost.is_open()) {
        cout << "failed to open file: " + name << endl;
        return -2;
    }

    string reading;
    vector<string> fields;
    int i = 0;

    while(!getline(ost, reading).eof()) {
        cout << "line " << i << ":";
        fields = parse_line(reading);
        for( int k = 0; k < fields.size(); k++) {
          cout << fields[k] + ",";
        }
        cout << endl; 
        i++;
    }
    
    ost.close();
}