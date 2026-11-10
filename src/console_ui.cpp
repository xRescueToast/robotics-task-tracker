#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include "console_ui.h"

using namespace std;

//keep asking until the user types a whole number between min and max
int getValidInt(const string& prompt, int min, int max){
    int value;
    while (true){
        cout << prompt;
        if (cin >> value && value >= min && value <= max){
            cin.ignore(1000, '\n');
            return value;
        }
        cout << "Enter a whole number from " << min << " to " << max << ".\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

//ask a yes/no question and return true for yes
bool confirm(const string& prompt){
    cout << prompt << " (y/n): ";
    char answer;
    while (!(cin >> answer) || (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N')){
        cout << "Invalid input, enter y or n: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');
    return answer == 'y' || answer == 'Y';
}

//helper function that gets the current date in YYYY-MM-DD format
string getCurrentDate(){
    time_t now = time(nullptr);

    tm localTime;

    if (localtime_s(&localTime, &now) != 0)
    {
        return "";
    }

    stringstream dateStream;
    dateStream << put_time(&localTime, "%Y-%m-%d");

    return dateStream.str();
}

//read the attempts and successes for a run, successes can never be more than attempts
void readAttempts(scoreRecord& sr){
    sr.attempts = getValidInt("Enter number of attempts: ", 1, 100);
    sr.successes = getValidInt("Enter number of successes: ", 0, sr.attempts);
}

//helper function that reads in a score record from the user
scoreRecord readScoreRecord(){
    scoreRecord sr;
    readAttempts(sr);
    sr.date = getCurrentDate();
    return sr;
}

//the label shown for a mission, its name already includes the mission number
string missionLabel(const Mission& mission){
    return mission.name;
}

//create mission, returns false if the name is already used so nothing gets saved
bool addMission(vector<Mission>& missions){
    Mission m;
    cout << "Enter mission name: ";
    getline(cin, m.name);
    if (m.name.empty()){
        cout << "Mission name can't be empty.\n";
        return false;
    }
    for (const Mission& existing : missions){
        if (existing.name == m.name){
            cout << "A mission with that name already exists.\n";
            return false;
        }
    }
    missions.push_back(m);
    return true;
}

//print every mission with its list number and task count
void viewMissions(const vector<Mission>& missions){
    if (missions.empty()){
        cout << "No missions yet. Add one first.\n";
        return;
    }
    cout << "Missions:\n";
    for (size_t i = 0; i < missions.size(); i++){
        cout << i + 1 << ". " << missionLabel(missions[i]) << " (" << missions[i].tasks.size() << " tasks)\n";
    }
}

//list the missions and return the chosen index, or -1 if there are none
int selectMission(const vector<Mission>& missions){
    if (missions.empty()){
        cout << "No missions yet. Add one first.\n";
        return -1;
    }
    viewMissions(missions);
    int number = getValidInt("Enter the mission number from the list: ", 1, (int)missions.size());
    return number - 1;
}

//ask which mission to delete and confirm it, returns the index or -1 if nothing should be deleted
int selectMissionToDelete(const vector<Mission>& missions){
    cout << "Select a mission to delete:\n";
    int missionIndex = selectMission(missions);
    if (missionIndex < 0){
        return -1;
    }
    if (!confirm("Delete \"" + missionLabel(missions[missionIndex]) + "\" and all of its tasks?")){
        cout << "Delete cancelled.\n";
        return -1;
    }
    return missionIndex;
}

//create task
void addTask(vector<Task>& tasks){
    Task t;
    cout << "Enter task name: ";
    getline(cin, t.name);
    t.points = getValidInt("Enter possible points: ", 0, 1000);
    //read in the first run of attempts for the task
    t.scoreHistory.push_back(readScoreRecord());
    tasks.push_back(t);
}

//print one score record with its calculated reliability and expected score
void printScoreRecord(const Task& task, const scoreRecord& sr){
    cout << fixed << setprecision(1);
    cout << "\tSuccesses: " << sr.successes << "/" << sr.attempts << "\n";
    cout << "\tReliability: " << sr.reliability() * 100 << "%\n";
    cout << "\tExpected Score: " << task.expectedScore(sr) << " of " << task.points << "\n";
    cout << defaultfloat;
}

//loop through tasks and display their details to the user
void viewTasks(const vector<Task>& tasks){
    if (tasks.empty()){
        cout << "This mission has no tasks yet.\n";
        return;
    }
    int taskNumber = 1;
    cout << "-----------------------------\n";
    for (const Task& task : tasks){
        //print task data
        cout << "Task " << taskNumber << ": " << task.name << " (" << task.points << " points)\n";
        //record counter
        int recordNumber = 1;
        for (const scoreRecord& sr : task.scoreHistory){
            //display the date of the score record and its number in the history
            cout << "Score Record " << recordNumber++ << " (" << sr.date << ")" << ":\n";
            printScoreRecord(task, sr);
        }
        cout << "-----------------------------\n";
        taskNumber++;
    }
}

size_t selectTask(const vector<Task>& tasks){
    for (size_t i = 0; i < tasks.size(); i++){
        cout << i + 1 << ". " << tasks[i].name << "\n";
    }
    int taskNumber = getValidInt("Enter the task number: ", 1, (int)tasks.size());
    return (size_t)(taskNumber - 1);
}

//helper function for edit task that handles changing the name of a task
void editTaskName(Task& task){
    cout << "Current name: " << task.name << "\n";
    cout << "Enter new name: ";
    getline(cin, task.name);
}

//helper function for edit task that handles changing the possible points of a task
void editTaskPoints(Task& task){
    cout << "Current points: " << task.points << "\n";
    task.points = getValidInt("Enter new possible points: ", 0, 1000);
}

size_t selectScoreRecord(const Task& task){
    //list the records belonging to the passed task
    for (size_t i = 0; i < task.scoreHistory.size(); i++){
        const scoreRecord& sr = task.scoreHistory[i];
        cout << i + 1 << ". " << sr.date << " (" << sr.successes << "/" << sr.attempts << ")\n";
    }
    //prompt user to pick from the listed records
    int recordNumber = getValidInt("Enter the record number: ", 1, (int)task.scoreHistory.size());
    return (size_t)(recordNumber - 1);
}

int editScoreRecord(Task& task){
    size_t recordIndex = selectScoreRecord(task);
    scoreRecord& sr = task.scoreHistory[recordIndex];

    cout << "Current: " << sr.successes << "/" << sr.attempts << "\n";
    readAttempts(sr);

    return (int)recordIndex;
}

//edit task information
editResult editTask(vector<Task>& tasks){
    editResult result;
    if (tasks.empty()){
        cout << "No tasks available to edit.\n";
        return result;
    }
    cout << "Select a task to edit:\n";
    size_t taskIndex = selectTask(tasks);

    //list options
    cout << "1. Edit task name\n";
    cout << "2. Edit possible points\n";
    cout << "3. Edit task score record\n";
    int editChoice = getValidInt("Enter choice: ", 1, 3);

    //apply the chosen edit and record only what was actually changed in the result
    //a task with no score records leaves the result empty so nothing gets saved
    if (editChoice == 1){
        editTaskName(tasks[taskIndex]);
        result.kind = EditKind::Name;
        result.taskIndex = (int)taskIndex;
    }
    else if (editChoice == 2){
        editTaskPoints(tasks[taskIndex]);
        result.kind = EditKind::Points;
        result.taskIndex = (int)taskIndex;
    }
    else if (tasks[taskIndex].scoreHistory.empty()){
        cout << "This task has no score records to edit.\n";
    }
    else{
        result.recordIndex = editScoreRecord(tasks[taskIndex]);
        result.taskIndex = (int)taskIndex;
        result.kind = EditKind::Score;
    }
    return result;
}

//add score record to a task
int addScoreRecord(vector<Task>& tasks){
    if (tasks.empty()){
        cout << "No tasks available to add score records.\n";
        return -1;
    }
    //list tasks to user along with their index number
    cout << "Select a task to add a score record:\n";
    size_t taskIndex = selectTask(tasks);
    tasks[taskIndex].scoreHistory.push_back(readScoreRecord());
    return (int)taskIndex;
}

//ask which task to delete and confirm it, returns the index or -1 if nothing should be deleted
int selectTaskToDelete(const vector<Task>& tasks){
    if (tasks.empty()){
        cout << "No tasks available to delete.\n";
        return -1;
    }

    cout << "Select a task to delete:\n";
    size_t taskIndex = selectTask(tasks);

    if (!confirm("Delete \"" + tasks[taskIndex].name + "\" and all of its score records?")){
        cout << "Delete cancelled.\n";
        return -1;
    }

    return (int)taskIndex;
}
