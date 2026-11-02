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


//inserts a new task into the database and return the id Postgres gives it
int saveTask(const string& connStr, const string& name) {
    //opens a connection and start a transaction
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    //temporary: find the first season, since every task must belong to one
    int seasonId = tx.query_value<int>("SELECT id FROM seasons ORDER BY id LIMIT 1");

    //insert the task. $1 and $2 are placeholders filled in by pqxx::params,
    //the typed name is never pasted into the SQL (prevents SQL injection).
    //RETURNING id makes Postgres send back the new row's id.
    int newId = tx.query_value<int>(
        "INSERT INTO tasks (season_id, name) VALUES ($1, $2) RETURNING id",
        pqxx::params{seasonId, name});

    //saves the changes; without commit the insert would be rolled back
    tx.commit();

    //give the caller the new task's database id
    return newId;
}

//insert a score record for a task and return the id postgres gives it
int saveScoreRecord(const string& connStr, int taskId, const scoreRecord& record) {
    //open a connection and start a transaction
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    //run the INSERT with placeholders $1-$4, filled in order by pqxx::params
    //taskId links the score to its task, and a bad id makes postgres throw an error
    //RETURNING id sends back the id of the new row
    int newId = tx.query_value<int>(
        "INSERT INTO score_records (task_id, recorded_date, expected_score, actual_score) "
        "VALUES ($1, $2, $3, $4) RETURNING id",
        pqxx::params{taskId, record.date, record.expectedScore, record.actualScore});

    //save the changes, without commit the insert is rolled back
    tx.commit();

    //give the caller the new score record's id
    return newId;
}

//save a new task and all of its score records, storing the new ids in the task
void saveTaskWithScores(const string& connStr, Task& task) {
    task.id = saveTask(connStr, task.name);
    for (scoreRecord& sr : task.scoreHistory) {
        sr.id = saveScoreRecord(connStr, task.id, sr);
    }
}

//save the most recently added score record of a task and store its new id
void saveLatestScoreRecord(const string& connStr, Task& task) {
    scoreRecord& sr = task.scoreHistory.back();
    sr.id = saveScoreRecord(connStr, task.id, sr);
}

//save the task's current name to its row in the database
void updateTaskName(const string& connStr, const Task& task) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("UPDATE tasks SET name = $1 WHERE id = $2",
            pqxx::params{task.name, task.id});

    tx.commit();
}

//save the score record's current values to its row in the database
void updateScoreRecord(const string& connStr, const scoreRecord& record) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("UPDATE score_records SET expected_score = $1, actual_score = $2 WHERE id = $3",
            pqxx::params{record.expectedScore, record.actualScore, record.id});

    tx.commit();
}

//save only what the edit changed, based on what editTask reported
void saveEdit(const string& connStr, const vector<Task>& tasks, const editResult& result) {
    if (result.kind == EditKind::Name) {
        updateTaskName(connStr, tasks[result.taskIndex]);
    }
    else if (result.kind == EditKind::Score) {
        updateScoreRecord(connStr, tasks[result.taskIndex].scoreHistory[result.recordIndex]);
    }
}