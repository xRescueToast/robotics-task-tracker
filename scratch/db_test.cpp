#include <iostream>
#include "storage.h"

using namespace std;

int main() {
    string connStr = loadConnString("tracker.env");
    if (connStr.empty()) {
        cout << "Could not read TRACKER_DB from tracker.env\n";
        return 1;
    }

    try {
        int id = saveTask(connStr, "Test Task");
        cout << "Saved task with id " << id << "\n";

        scoreRecord r;
        r.date = "2026-10-31";
        r.expectedScore = 10;
        r.actualScore = 8;
        int scoreId = saveScoreRecord(connStr, id, r);
        cout << "Saved score with id " << scoreId << "\n";

        vector<Task> tasks = loadTasks(connStr);
        for (const Task& t : tasks) {
            cout << t.id << ": " << t.name << " (" << t.scoreHistory.size() << " scores)\n";
            for (const scoreRecord& s : t.scoreHistory) {
                cout << "\t" << s.date << " expected " << s.expectedScore
                     << " actual " << s.actualScore << "\n";
            }
        }
    } catch (const exception& e) {
        cout << "Database error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}