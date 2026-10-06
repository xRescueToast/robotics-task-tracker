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

    cout << "Enter task name: ";
    getline(cin, t.name);

    cout << "Enter expected score: ";
    cin >> t.expectedScore;

    cout << "Enter actual score: ";
    cin >> t.actualScore;

    cin.ignore();

    tasks.push_back(t);
}

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
        cin >> choice;
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