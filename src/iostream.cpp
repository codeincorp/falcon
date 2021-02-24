#include <iostream>
#include <fstream>
#include <ostream>
#include <cassert>
#include <string>
#include <error.h>

using namespace std;

int main() {
    string name = "list1.txt";
    string name1 = "list.txt";
    //cin >> name;
    fstream ost;
    //assert(ost.is_open());
    ost.open(name1, fstream::in);
    //assert(!ost.fail());
    string reading;
    while(!getline(ost, reading,',').eof()) {
        cout << reading << endl;
    }
    

    //ost.flush();
    ost.close();

    //if(ost.is_open()) {
      //  cout << "fuck you" << endl;
    //}
}