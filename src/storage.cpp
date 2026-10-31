#include <pqxx/pqxx>
#include <fstream>
#include "storage.h"

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

vector<Task> loadTasks(const string& connStr) {
    vector<Task> tasks;

    //opens a connection to the server using the settings from tracker.env
    pqxx::connection conn(connStr);

    //start a transaction -> libpqxx needs one to run a query
    pqxx::work tx(conn);

    //asks the database for the id and name of every task, oldest first
    pqxx::result rows = tx.exec("SELECT id, name FROM tasks ORDER BY id");

    //iterate through the result one row at a time
    for (const auto& row : rows) {
        Task t;

        //copy the database's id and name for this row into the task struct
        t.id = row["id"].as<int>();
        t.name = row["name"].as<string>();

        //load the task's score records
        pqxx::result scoreRows = tx.exec(
            "SELECT id, recorded_date, expected_score, actual_score "
            "FROM score_records WHERE task_id = " + pqxx::to_string(t.id) +
            " ORDER BY id");

        for (const auto& s : scoreRows) {
            scoreRecord sr;
            sr.id = s["id"].as<int>();
            sr.date = s["recorded_date"].as<string>();
            sr.expectedScore = s["expected_score"].as<double>();
            sr.actualScore = s["actual_score"].as<double>();
            t.scoreHistory.push_back(sr);
        }

        //add the finished task to the list
        tasks.push_back(t);
    }
    return tasks;
}