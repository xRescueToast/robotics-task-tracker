#include "storage.h"
#include <fstream>

using namespace std;

string loadConnString(const string& path) {
    ifstream file(path);
    string line;
    const string key = "TRACKER_DB=";
    while (getline(file, line)) {
        if (line.rfind(key, 0) == 0) {
            return line.substr(key.size());
        }
    }
    return "";
}