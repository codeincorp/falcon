#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    fstream fs("/home/yesarang/data.csv", ios_base::in);

    string s;
    while (getline(fs, s)) {
        cout << s << endl;
    }
}
