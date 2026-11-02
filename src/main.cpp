#include <iostream>
#include <exception>
#include "task.h"
#include "console_ui.h"
#include "storage.h"

using namespace std;

int main(){
    string connStr;
    vector<Task> tasks;
    int seasonId = 1;
    try{
        connStr = loadConnString("tracker.env");
        tasks = loadTasks(connStr, seasonId);
    }
    catch(const exception& e){
        cout << "Could not start: " << e.what() << "\n";
        return 1;
    }

    int taskIndex;
    bool active = true;
    while(active){
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Add Score Record\n";
        cout << "4. Edit Task\n";
        cout << "5. Delete Task\n";
        cout << "6. Exit\n";
        int choice;
        if(!(cin >> choice)){
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(1000, '\n');

        try{
            switch(choice){
                case 1:
                    addTask(tasks);
                    saveTaskWithScores(connStr, seasonId, tasks.back());
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
                    taskIndex = selectTaskToDelete(tasks);
                    if (taskIndex >= 0) removeTask(connStr, tasks, taskIndex);
                    break;
                case 6:
                    active = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const exception& e){
            cout << "Error: " << e.what() << "\n";
            try{
                tasks = loadTasks(connStr, seasonId);
            }
            catch (const exception&){
                cout << "Could not refresh the list. Restart the app.\n";
            }
        }
        
    }

    return 0;
}