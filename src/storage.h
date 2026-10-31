#pragma once
#include <string>
#include <vector>
#include "task.h"

std::string loadConnString(const std::string& path);
std::vector<Task> loadTasks(const std::string& connStr);