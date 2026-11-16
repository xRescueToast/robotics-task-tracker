#pragma once

#include <string>
#include <vector>

struct Season {
    int id = 0;
    std::string name;
};

struct ScoreRecord {
    int id = 0;
    std::string date;
    int attempts = 0;
    int successes = 0;

    double reliability() const {
        if (attempts == 0)
            return 0.0;
        return static_cast<double>(successes) / attempts;
    }
};

struct Task {
    int id = 0;
    std::string name;
    int points = 0;
    std::vector<ScoreRecord> scoreHistory;

    double expectedScore(const ScoreRecord& record) const {
        return record.reliability() * points;
    }
};

struct Mission {
    int id = 0;
    std::string name;
    std::vector<Task> tasks;
};
