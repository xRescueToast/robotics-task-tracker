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
    return score;
}

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

//create task
void addTask(vector<Task>& tasks){
    Task t;
    scoreRecord sr;
    cout << "Enter task name: ";
    getline(cin, t.name);

    //read in expected and actual scores for the task
    sr.expectedScore = getValidScore("Enter expected score: ");
    sr.actualScore = getValidScore("Enter actual score: ");
    sr.date = getCurrentDate();
    //add the score record to the task's history
    t.scoreHistory.push_back(sr);
    
    cin.ignore(1000, '\n');
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
        for (const scoreRecord& sr : task.scoreHistory){
            //display the date of the score record
            cout << "Date: " << sr.date << "\n";
            //display the score record
            cout << "Expected Score: " << sr.expectedScore << "\n";
            cout << "Actual Score: " << sr.actualScore << "\n";

            double guidanceScore = sr.actualScore - sr.expectedScore;

            cout << "Guidance Score: " << guidanceScore << "\n";
        }
        cout << "-----------------------------\n";
        taskNumber++;
    }
}

//edit task information
void editTask(vector<Task>& tasks){
    if (tasks.empty()){
        cout << "No tasks available to edit.\n";
        return;
    }
}

int main(){
    vector<Task> tasks;
    bool active = true;
    while(active){
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Edit Task\n";
        cout << "4. Exit\n";
        int choice;
        if(!(cin >> choice)){
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore();

        switch(choice){
            case 1:
                addTask(tasks);
                break;
            case 2:
                viewTasks(tasks);
                break;
            case 3:
                // Edit Task functionality can be implemented here in the future
                cout << "Edit Task feature is not implemented yet.\n";
                break;
            case 4:
                active = false;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}