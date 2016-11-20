#pragma once

#include <set>
#include <utility>

struct Stat {
    int value {0};
    std::multiset<std::pair<int, int>> bases;  // priority, value
    int base {0};
    int shift {0};
    int multiplier {0};
    int divider {0};
    void update();
};



