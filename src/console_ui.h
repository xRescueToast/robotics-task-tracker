#pragma once

#include <vector>
#include "task.h"

void addTask(std::vector<Task>& tasks);
void viewTasks(const std::vector<Task>& tasks);
editResult editTask(std::vector<Task>& tasks);
int addScoreRecord(std::vector<Task>& tasks);
int selectTaskToDelete(const std::vector<Task>& tasks);