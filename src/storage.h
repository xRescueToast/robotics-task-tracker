#pragma once
#include <string>
#include <vector>
#include "task.h"

std::string loadConnString(const std::string& path);
std::vector<Task> loadTasks(const std::string& connStr);
int saveTask(const std::string& connStr, const std::string& name);
int saveScoreRecord(const std::string& connStr, int taskId, const scoreRecord& record);
void saveTaskWithScores(const std::string& connStr, Task& task);
void saveLatestScoreRecord(const std::string& connStr, Task& task);