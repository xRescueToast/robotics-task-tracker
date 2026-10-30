#include <iostream>
#include "task.h"
#include "console_ui.h"

using namespace std;

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