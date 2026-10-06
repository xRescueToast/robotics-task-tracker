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

int main(){
    vector<Task> tasks;
    char continueAdding = 'y';
    while (continueAdding == 'y' || continueAdding == 'Y'){
        Task t;

        cout << "Enter task name: ";

        getline(cin, t.name);

        cout << "Enter exepected score: ";
        cin >> t.expectedScore;

        cout << "Enter actual score: ";
        cin >> t.actualScore;

        //cin.ignore();

        tasks.push_back(t);

        double guidanceScore = t.actualScore - t.expectedScore;


        //inform the user that the task was successfully added to the vector
        cout << "\nTask added successfully! \n";

        //print the task details
        cout << "Task details:\n";
        cout << "name: " << tasks[0].name << "\n";
        cout << "expected score: " << tasks[0].expectedScore << "\n";
        cout << "actual score: " << tasks[0].actualScore << "\n";
        //print the guidance score of the task, not included in the task struct
        cout << "guidance score: " << guidanceScore << "\n";


        //check if the user wants to add another task
        cout << "Do you want to add another task? (y/n): ";
        cin >> continueAdding; // Read the user's choice
        cin.ignore(); // Ignore any newline character left in the buffer
    }

    return 0;
}