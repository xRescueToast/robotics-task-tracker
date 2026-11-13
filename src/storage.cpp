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

//load every score record for one task, oldest first
static vector<scoreRecord> loadScoreRecords(pqxx::work& tx, int taskId) {
    vector<scoreRecord> records;

    pqxx::result rows = tx.exec(
        "SELECT id, recorded_date, attempts, successes "
        "FROM score_records WHERE task_id = $1 ORDER BY recorded_date, id",
        pqxx::params{taskId});

    for (const auto& row : rows) {
        scoreRecord sr;
        sr.id = row["id"].as<int>();
        sr.date = row["recorded_date"].as<string>();
        sr.attempts = row["attempts"].as<int>();
        sr.successes = row["successes"].as<int>();
        records.push_back(sr);
    }
    return records;
}

//load every task for one mission, with each task's score records
static vector<Task> loadTasksForMission(pqxx::work& tx, int missionId) {
    vector<Task> tasks;

    pqxx::result rows = tx.exec(
        "SELECT id, name, points FROM tasks WHERE mission_id = $1 ORDER BY id",
        pqxx::params{missionId});

    for (const auto& row : rows) {
        Task t;
        t.id = row["id"].as<int>();
        t.name = row["name"].as<string>();
        t.points = row["points"].as<int>();
        t.scoreHistory = loadScoreRecords(tx, t.id);
        tasks.push_back(t);
    }
    return tasks;
}

//load every mission in a season, with their tasks and score records
vector<Mission> loadMissions(const string& connStr, int seasonId) {
    vector<Mission> missions;

    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    pqxx::result rows = tx.exec(
        "SELECT id, name FROM missions WHERE season_id = $1 ORDER BY id",
        pqxx::params{seasonId});

    for (const auto& row : rows) {
        Mission m;
        m.id = row["id"].as<int>();
        m.name = row["name"].as<string>();
        m.tasks = loadTasksForMission(tx, m.id);
        missions.push_back(m);
    }
    return missions;
}

//insert a new mission into a season and store the id postgres gives it
void saveMission(const string& connStr, int seasonId, Mission& mission) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    mission.id = tx.query_value<int>(
        "INSERT INTO missions (season_id, name) VALUES ($1, $2) RETURNING id",
        pqxx::params{seasonId, mission.name});

    tx.commit();
}

//delete one mission by its id, ON DELETE CASCADE removes its tasks and score records
void deleteMission(const string& connStr, int missionId) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("DELETE FROM missions WHERE id = $1", pqxx::params{missionId});

    tx.commit();
}

//delete a mission from the database, then remove it from the in-memory list so both match
void removeMission(const string& connStr, vector<Mission>& missions, int missionIndex) {
    deleteMission(connStr, missions[missionIndex].id);
    missions.erase(missions.begin() + missionIndex);
}

//inserts a new task into a mission and return the id Postgres gives it
int saveTask(const string& connStr, int missionId, const string& name, int points) {
    //opens a connection and start a transaction
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    //insert the task. $1-$3 are placeholders filled in by pqxx::params,
    //the typed name is never pasted into the SQL (prevents SQL injection).
    //RETURNING id makes Postgres send back the new row's id.
    int newId = tx.query_value<int>(
        "INSERT INTO tasks (mission_id, name, points) VALUES ($1, $2, $3) RETURNING id",
        pqxx::params{missionId, name, points});

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
        "INSERT INTO score_records (task_id, recorded_date, attempts, successes) "
        "VALUES ($1, $2, $3, $4) RETURNING id",
        pqxx::params{taskId, record.date, record.attempts, record.successes});

    //save the changes, without commit the insert is rolled back
    tx.commit();

    //give the caller the new score record's id
    return newId;
}

//save a new task and all of its score records, storing the new ids in the task
void saveTaskWithScores(const std::string& connStr, int missionId, Task& task) {
    task.id = saveTask(connStr, missionId, task.name, task.points);
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

//save the task's current possible points to its row in the database
void updateTaskPoints(const string& connStr, const Task& task) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("UPDATE tasks SET points = $1 WHERE id = $2",
            pqxx::params{task.points, task.id});

    tx.commit();
}

//save the score record's current values to its row in the database
void updateScoreRecord(const string& connStr, const scoreRecord& record) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("UPDATE score_records SET recorded_date = $1, attempts = $2, successes = $3 WHERE id = $4",
            pqxx::params{record.date, record.attempts, record.successes, record.id});

    tx.commit();
}

//save only what the edit changed, based on what editTask reported
void saveEdit(const string& connStr, const vector<Task>& tasks, const editResult& result) {
    if (result.kind == EditKind::Name) {
        updateTaskName(connStr, tasks[result.taskIndex]);
    }
    else if (result.kind == EditKind::Points) {
        updateTaskPoints(connStr, tasks[result.taskIndex]);
    }
    else if (result.kind == EditKind::Score) {
        updateScoreRecord(connStr, tasks[result.taskIndex].scoreHistory[result.recordIndex]);
    }
}

//delete one task from the database by its id
void deleteTask(const string& connStr, int taskId) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("DELETE FROM tasks WHERE id = $1", pqxx::params{taskId});

    tx.commit();
}

//delete a task from the database, then remove it from the in-memory list so both match
void removeTask(const string& connStr, vector<Task>& tasks, int taskIndex) {
    deleteTask(connStr, tasks[taskIndex].id);
    tasks.erase(tasks.begin() + taskIndex);
}

//load every season from the database, oldest first
vector<Season> loadSeasons(const string& connStr) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    vector<Season> seasons;
    for (const auto& row : tx.exec("SELECT id, name FROM seasons ORDER BY id")) {
        Season s;
        s.id = row[0].as<int>();
        s.name = row[1].as<string>();
        seasons.push_back(s);
    }

    return seasons;
}

//insert a new season and return its database id
int saveSeason(const string& connStr, const string& name) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    int id = tx.query_value<int>("INSERT INTO seasons (name) VALUES ($1) RETURNING id", pqxx::params{name});

    tx.commit();
    return id;
}

//delete one season from the database by its id
void deleteSeason(const string& connStr, int seasonId) {
    pqxx::connection conn(connStr);
    pqxx::work tx(conn);

    tx.exec("DELETE FROM seasons WHERE id = $1", pqxx::params{seasonId});

    tx.commit();
}
