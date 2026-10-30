#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

struct scoreRecord{
    string date;
    double expectedScore;
    double actualScore;
};

struct Task{
    string name;
    vector<scoreRecord> scoreHistory;
};

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

void editScoreRecord(Task& task){
    size_t recordIndex = selectScoreRecord(task);
    scoreRecord& sr = task.scoreHistory[recordIndex];

    cout << "Current expected score: " << sr.expectedScore << "\n";
    sr.expectedScore = getValidScore("Enter new expected score: ");
    cout << "Current actual score: " << sr.actualScore << "\n";
    sr.actualScore = getValidScore("Enter new actual score: ");
}

//edit task information
void editTask(vector<Task>& tasks){
    if (tasks.empty()){
        cout << "No tasks available to edit.\n";
        return;
    }
    else{
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

        if(editChoice == 1){
            editTaskName(tasks[taskIndex]);
        }
        else{
            editScoreRecord(tasks[taskIndex]);
        }
    }

}

//add score record to a task
void addScoreRecord(vector<Task>& tasks){
    if(tasks.empty()){
        cout << "No tasks available to add score records.\n";
        return;
    }
    else{
        //list scores to user along with their index number.
        cout << "Select a task to add a score record:\n";
        size_t taskIndex = selectTask(tasks);
        tasks[taskIndex].scoreHistory.push_back(readScoreRecord());
    }
}

int main(){
    vector<Task> tasks;
    bool active = true;
    while(active){
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Add Score Record\n";
        cout << "4. Edit Task\n";
        cout << "5. Exit\n";
        int choice;
        if(!(cin >> choice)){
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(1000, '\n');

        switch(choice){
            case 1:
                addTask(tasks);
                break;
            case 2:
                viewTasks(tasks);
                break;
            case 3:
                addScoreRecord(tasks);
                break;
            case 4:
                editTask(tasks);
                break;
            case 5:
                active = false;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}