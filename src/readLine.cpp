#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace codein {

vector<string> parse_line(const string& line) {
    int left = 0;
    int right = 0;
    int len = line.length();
    int i = 0;
    vector<string> strs;

    while (i <= len) {
        if (i == len || line.at(i) == ','){
            strs.emplace_back(line.substr(left, right-left));
            left = 0;
            right = 0;
        }
        else if (line.at(i) != ' ') {
            if (right == 0) {
                left = i;
            }
            right = i+1;
        }
        
        i++;
    }
    return strs;
}

} // namespace codein

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "file name is not specified\n";
        return -1;
    }

    string name = argv[1];
    fstream ost;
    ost.open(name, fstream::in);
    if (!ost.is_open()) {
        cout << "failed to open file: " + name << endl;
        return -2;
    }

    string reading;
    vector<string> fields;
    int i = 1;
    while (!ost.eof()) {
        getline(ost, reading);
        cout << "line " << i << ":";
        fields = codein::parse_line(reading);
        for (int k = 0; k < fields.size() - 1; k++) {
          cout << fields[k] + ",";
        }
        cout << fields[fields.size() - 1] << endl; 
        i++;
    }
    
    ost.close();
}