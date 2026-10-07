#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct Task{
    string name;
    double expectedScore;
    double actualScore;
};


//create task
void addTask(vector<Task>& tasks){
    Task t;
    bool validScore = false;

    cout << "Enter task name: ";
    getline(cin, t.name);

    //validate expected score input
    while (!validScore){
        cout << "Enter expected score: ";

        if (cin >> t.expectedScore){
            if (t.expectedScore >= 0 && t.expectedScore <= 1000){
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

    validScore = false;

    while (!validScore){
        cout << "Enter actual score: ";

        if (cin >> t.actualScore){
            if (t.actualScore >= 0 && t.actualScore <= 1000){
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
    /*
    cout << "Enter expected score: ";
    while (!(cin >> t.expectedScore)){
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Enter expected score: ";
    }

    cout<< "enter actual score: ";
    while (!(cin >> t.actualScore)){
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(1000, '\n');

        cout << "Enter actual score: ";
    }
    */

    cin.ignore();

    tasks.push_back(t);
}

//loop through tasks and display their details to the user
void viewTasks(const vector<Task>& tasks){
    for (const Task& task : tasks){
        //print task data
        cout << "Task details:\n";
        cout << "name: " << task.name << "\n";
        cout << "expected score: " << task.expectedScore << "\n";
        cout << "actual score: " << task.actualScore << "\n";

        //calculate the guidance score for each referenced task
        double guidanceScore = task.actualScore - task.expectedScore;
        cout << "Guidance Score: " << guidanceScore << "\n";
        cout << "-----------------------------\n";
    }
}

int main(){
    vector<Task> tasks;
    bool active = true;
    while(active){
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Exit\n";
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
                active = false;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}