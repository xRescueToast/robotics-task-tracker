#pragma once

#include <string>
#include <vector>
#include "models.h"

//which part of a task the console edit changed, so main only saves that part
enum class EditKind {None, Name, Points, Score};

//what editTask changed, indexes are -1 when nothing was edited
struct EditResult {
    int taskIndex = -1;
    EditKind kind = EditKind::None;
    int recordIndex = -1;
};

std::string missionLabel(const Mission& mission);
bool addMission(std::vector<Mission>& missions);
void viewMissions(const std::vector<Mission>& missions);
int selectMission(const std::vector<Mission>& missions);
int selectMissionToDelete(const std::vector<Mission>& missions);
void addTask(std::vector<Task>& tasks);
void viewTasks(const std::vector<Task>& tasks);
EditResult editTask(std::vector<Task>& tasks);
int addScoreRecord(std::vector<Task>& tasks);
int selectTaskToDelete(const std::vector<Task>& tasks);
