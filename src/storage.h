#pragma once
#include <string>
#include <vector>
#include "task.h"

std::string loadConnString(const std::string& path);
std::vector<Task> loadTasks(const std::string& connStr, int seasonId);
int saveTask(const std::string& connStr, int seasonId, const std::string& name);
int saveScoreRecord(const std::string& connStr, int taskId, const scoreRecord& record);
void saveTaskWithScores(const std::string& connStr, int seasonId, Task& task);
void saveLatestScoreRecord(const std::string& connStr, Task& task);
void updateTaskName(const std::string& connStr, const Task& task);
void updateScoreRecord(const std::string& connStr, const scoreRecord& record);
void saveEdit(const std::string& connStr, const std::vector<Task>& tasks, const editResult& result);
void deleteTask(const std::string& connStr, int taskId);
void removeTask(const std::string& connStr, std::vector<Task>& tasks, int taskIndex);
std::vector<Season> loadSeasons(const std::string& connStr);
int saveSeason(const std::string& connStr, const std::string& name);
void deleteSeason(const std::string& connStr, int seasonId);