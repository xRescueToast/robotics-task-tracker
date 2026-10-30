#pragma once

#include <string>
#include <vector>

struct scoreRecord{
    std::string date;
    double expectedScore;
    double actualScore;
};

struct Task{
    std::string name;
    std::vector<scoreRecord> scoreHistory;
};