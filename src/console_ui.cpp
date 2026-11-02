#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include "console_ui.h"

using namespace std;

double getValidScore(const string prompt){
    bool validScore = false;
    double score;
    while (!validScore){
        cout << prompt;

        if (cin >> score){
            if (score >= 0 && score <= 1000){
                validScore = true;
            }
            else{
                cout << "Score must be between 0 and 1000.\n";
            }
        }
        else{
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }
    cin.ignore(1000, '\n');
    return score;
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

//helper function that reads in a score record from the user
scoreRecord readScoreRecord(){
    scoreRecord sr;
    sr.expectedScore = getValidScore("Enter expected score: ");
    sr.actualScore = getValidScore("Enter actual score: ");
    sr.date = getCurrentDate();
    return sr;
}

//create task
void addTask(vector<Task>& tasks){
    Task t;
    cout << "Enter task name: ";
    getline(cin, t.name);
    //read in expected and actual scores for the task
    //add the score record to the task's history
    t.scoreHistory.push_back(readScoreRecord());
    tasks.push_back(t);
}

//loop through tasks and display their details to the user
void viewTasks(const vector<Task>& tasks){
    int taskNumber = 1;
    cout << "-----------------------------\n";
    for (const Task& task : tasks){
        //print task data
        cout << "Task " << taskNumber << " details:\n";
        cout << "name: " << task.name << "\n";
        //record counter
        int recordNumber = 1;
        for (const scoreRecord& sr : task.scoreHistory){
            //display the date of the score record and its number in the history
            cout << "Score Record " << recordNumber++ << " (" << sr.date << ")" << ":\n";
            //display the score record
            cout << "\tExpected Score: " << sr.expectedScore << "\n";
            cout << "\tActual Score: " << sr.actualScore << "\n";

            double guidanceScore = sr.actualScore - sr.expectedScore;

            cout << "\tGuidance Score: " << guidanceScore << "\n";
        }
        cout << "-----------------------------\n";
        taskNumber++;
    }
}

size_t selectTask(const vector<Task>& tasks){
    for (size_t i = 0; i < tasks.size(); i++){
        cout << i + 1 << ". " << tasks[i].name << "\n";
    }

    size_t taskNumber;
    bool validIndex = false;
    cout << "Enter the task number: ";
    while(!validIndex){
        if (cin >> taskNumber && taskNumber >= 1 && taskNumber <= tasks.size()){
            validIndex = true;
        }
        else{
            cout << "Invalid input. Please enter a valid task number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
        }
    }
    cin.ignore(1000, '\n');
    return taskNumber - 1;
}

//helper function for edit task that handles changing the name of a task
void editTaskName(Task& task){
    cout << "Current name: " << task.name << "\n";
    cout << "Enter new name: ";
    getline(cin, task.name);
}

size_t selectScoreRecord(const Task& task){
    //list the records belonging to the passed task
    for (size_t i = 0; i < task.scoreHistory.size(); i++){
        const scoreRecord& sr = task.scoreHistory[i];
        cout << i + 1 << ". " << sr.date << " (expected: " << sr.expectedScore << ", actual: " << sr.actualScore << ")\n";
    }

    //prompt user to pick from the listed records
    size_t recordNumber;
    cout << "Enter the record number: ";
    while(!(cin >> recordNumber) || recordNumber < 1 || recordNumber > task.scoreHistory.size()){
        cout << "Invalid input. Please enter a valid record number.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');
    return recordNumber - 1;
}

int editScoreRecord(Task& task){
    size_t recordIndex = selectScoreRecord(task);
    scoreRecord& sr = task.scoreHistory[recordIndex];

    cout << "Current expected score: " << sr.expectedScore << "\n";
    sr.expectedScore = getValidScore("Enter new expected score: ");
    cout << "Current actual score: " << sr.actualScore << "\n";
    sr.actualScore = getValidScore("Enter new actual score: ");

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
    cout << "2. Edit task score record\n";
    cout << "Enter choice: ";

    //handle selection and validate input
    int editChoice;
    while(!(cin >> editChoice) || editChoice < 1 || editChoice > 2){
        cout << "Invalid input, enter 1 or 2\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    //apply the chosen edit and record only what was actually changed in the result
    //a task with no score records leaves the result empty so nothing gets saved
    if(editChoice == 1){
        editTaskName(tasks[taskIndex]);
        result.kind = EditKind::Name;
        result.taskIndex = (int)taskIndex;
    }
    else if(tasks[taskIndex].scoreHistory.empty()){
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
    if(tasks.empty()){
        cout << "No tasks available to add score records.\n";
        return -1;
    }
    else{
        //list scores to user along with their index number.
        cout << "Select a task to add a score record:\n";
        size_t taskIndex = selectTask(tasks);
        tasks[taskIndex].scoreHistory.push_back(readScoreRecord());
        return int(taskIndex);
    }
}

//ask which task to delete and confirm it, returns the index or -1 if nothing should be deleted
int selectTaskToDelete(const vector<Task>& tasks){
    if (tasks.empty()){
        cout << "No tasks available to delete.\n";
        return -1;
    }

    cout << "Select a task to delete:\n";
    size_t taskIndex = selectTask(tasks);

    cout << "Delete \"" << tasks[taskIndex].name << "\" and all of its score records? (y/n): ";
    char answer;
    while (!(cin >> answer) || (answer != 'y' && answer != 'Y' && answer != 'n' && answer != 'N')){
        cout << "Invalid input, enter y or n: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cin.ignore(1000, '\n');

    if (answer == 'n' || answer == 'N'){
        cout << "Delete cancelled.\n";
        return -1;
    }

    return (int)taskIndex;
}