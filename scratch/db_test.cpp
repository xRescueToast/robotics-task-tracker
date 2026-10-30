#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <string>

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

int main() {
    string connStr = loadConnString("tracker.env");
    if (connStr.empty()) {
        cout << "Could not read TRACKER_DB from tracker.env\n";
        return 1;
    }

    try {
        pqxx::connection conn(connStr);
        pqxx::work tx(conn);
        int count = tx.query_value<int>("SELECT COUNT(*) FROM tasks");
        cout << "Connected! tasks table has " << count << " rows\n";
    } catch (const exception& e) {
        cout << "Database error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}