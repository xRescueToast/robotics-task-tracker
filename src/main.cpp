#include <iostream>
#include "task.h"
#include "console_ui.h"
#include "storage.h"

using namespace std;

int main(){
    string connStr = loadConnString("tracker.env");
    vector<Task> tasks = loadTasks(connStr);
    int taskIndex;
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
                saveTaskWithScores(connStr, tasks.back());
                break;
            case 2:
                viewTasks(tasks);
                break;
            case 3:
                taskIndex = addScoreRecord(tasks);
                if (taskIndex >= 0) saveLatestScoreRecord(connStr, tasks[taskIndex]);
                break;
            case 4:
                saveEdit(connStr, tasks, editTask(tasks));
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