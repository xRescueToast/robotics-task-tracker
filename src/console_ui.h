#pragma once

#include <string>
#include <vector>
#include "task.h"

std::string missionLabel(const Mission& mission);
bool addMission(std::vector<Mission>& missions);
void viewMissions(const std::vector<Mission>& missions);
int selectMission(const std::vector<Mission>& missions);
int selectMissionToDelete(const std::vector<Mission>& missions);
void addTask(std::vector<Task>& tasks);
void viewTasks(const std::vector<Task>& tasks);
editResult editTask(std::vector<Task>& tasks);
int addScoreRecord(std::vector<Task>& tasks);
int selectTaskToDelete(const std::vector<Task>& tasks);
