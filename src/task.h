#pragma once

#include <string>
#include <vector>

enum class EditKind {None, Name, Score};

struct editResult {
    int taskIndex = -1;
    EditKind kind = EditKind::None;
    int recordIndex = -1;
};

struct scoreRecord{
    int id = 0;
    std::string date;
    double expectedScore;
    double actualScore;
};

struct Task{
    int id = 0;
    std::string name;
    std::vector<scoreRecord> scoreHistory;
};