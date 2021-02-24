#include <iostream>
#include <fstream>
#include <ostream>
#include <cassert>
#include <string>
#include <error.h>

using namespace std;

int main() {
  //file name
    string name = "list1.txt";
    //string name1 = "list.txt";

    fstream ost;
   
    ost.open(name, fstream::in);
  
    string reading;
    while(!getline(ost, reading,',').eof()) {
        cout << reading << endl;
    }
    
    ost.close();

    
}