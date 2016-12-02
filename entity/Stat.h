#pragma once

#include <set>
#include <utility>

struct Stat {
    Stat(const Stat&other);
    Stat();
    Stat(int base);
    int value {0};
    std::multiset<std::pair<int, int>> bases;  // priority, value
    int base {0};
    int shift {0};
    int multiplier {1};
    int divider {1};
    void update();
};



