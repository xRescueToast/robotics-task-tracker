#pragma once

#include <string>
#include <vector>

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