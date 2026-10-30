#pragma once

#include <vector>
#include "task.h"

void addTask(std::vector<Task>& tasks);
void viewTasks(const std::vector<Task>& tasks);
void editTask(std::vector<Task>& tasks);
void addScoreRecord(std::vector<Task>& tasks);