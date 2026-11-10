#pragma once

#include <string>
#include <vector>

enum class EditKind {None, Name, Points, Score};

struct Season{
    int id = 0;
    std::string name;
};

struct editResult {
    int taskIndex = -1;
    EditKind kind = EditKind::None;
    int recordIndex = -1;
};

struct scoreRecord {
    int id = 0;
    std::string date;
    int attempts = 0;
    int successes = 0;

    double reliability() const {
        if (attempts == 0) return 0.0;
        return static_cast<double>(successes) / attempts;
    }
};

struct Task {
    int id = 0;
    std::string name;
    int points = 0;
    std::vector<scoreRecord> scoreHistory;

    double expectedScore(const scoreRecord& record) const {
        return record.reliability() * points;
    }
};

struct Mission {
    int id = 0;
    std::string name;
    std::vector<Task> tasks;
};