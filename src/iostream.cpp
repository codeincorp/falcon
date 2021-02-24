#include <iostream>
#include <fstream>
#include <ostream>
#include <cassert>
#include <string>
#include <error.h>
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

int main(string fileName) {
  //file name
    string name = fileName;
    //string name1 = "list.txt";

    fstream ost;
   
    ost.open(name, fstream::in);

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