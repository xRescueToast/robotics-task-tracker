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


//create task
void addTask(vector<Task>& tasks){
    Task t;
    cout << "Enter task name: ";
    getline(cin, t.name);

    t.expectedScore = getValidScore("Enter expected score: ");
    t.actualScore = getValidScore("Enter actual score: ");
    
    cin.ignore(1000, '\n');
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