#include <iostream>
#include <exception>
#include "task.h"
#include "console_ui.h"
#include "storage.h"

using namespace std;

int main(){
    string connStr;
    vector<Mission> missions;
    int seasonId = 1;
    try{
        connStr = loadConnString("tracker.env");
        missions = loadMissions(connStr, seasonId);
    }
    catch(const exception& e){
        cout << "Could not start: " << e.what() << "\n";
        return 1;
    }

    //index of the mission the task options work on, -1 means none selected
    int current = -1;
    Mission noMission;
    int index;
    bool active = true;
    while(active){
        cout << "\nCurrent mission: " << (current >= 0 ? missionLabel(missions[current]) : "none") << "\n";
        cout << "1. Add Mission\n";
        cout << "2. View Missions\n";
        cout << "3. Select Mission\n";
        cout << "4. Delete Mission\n";
        cout << "5. Add Task\n";
        cout << "6. View Tasks\n";
        cout << "7. Add Score Record\n";
        cout << "8. Edit Task\n";
        cout << "9. Delete Task\n";
        cout << "10. Exit\n";
        int choice;
        if(!(cin >> choice)){
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(1000, '\n');

        //task options need a mission to work on
        if(choice >= 5 && choice <= 9 && current < 0){
            cout << "Select or add a mission first.\n";
            continue;
        }
        Mission& mission = (current >= 0) ? missions[current] : noMission;

        try{
            switch(choice){
                case 1:
                    if (addMission(missions)) { saveMission(connStr, seasonId, missions.back()); current = (int)missions.size() - 1; }
                    break;
                case 2:
                    viewMissions(missions);
                    break;
                case 3:
                    index = selectMission(missions);
                    if (index >= 0) current = index;
                    break;
                case 4:
                    index = selectMissionToDelete(missions);
                    if (index >= 0) { removeMission(connStr, missions, index); current = -1; }
                    break;
                case 5:
                    addTask(mission.tasks);
                    saveTaskWithScores(connStr, mission.id, mission.tasks.back());
                    break;
                case 6:
                    viewTasks(mission.tasks);
                    break;
                case 7:
                    index = addScoreRecord(mission.tasks);
                    if (index >= 0) saveLatestScoreRecord(connStr, mission.tasks[index]);
                    break;
                case 8:
                    saveEdit(connStr, mission.tasks, editTask(mission.tasks));
                    break;
                case 9:
                    index = selectTaskToDelete(mission.tasks);
                    if (index >= 0) removeTask(connStr, mission.tasks, index);
                    break;
                case 10:
                    active = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const exception& e){
            cout << "Error: " << e.what() << "\n";
            try{
                missions = loadMissions(connStr, seasonId);
                current = -1;
            }
            catch (const exception&){
                cout << "Could not refresh the list. Restart the app.\n";
            }
        }
    }

    return 0;
}
