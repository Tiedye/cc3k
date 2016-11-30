#include "Stat.h"

void Stat::update() {
    if (bases.size()) {
        base = bases.rbegin()->second;
    } else {
        base = 0;
    }
    value = (base + shift) * multiplier / divider;
    if (value < 0) value = 0;
}
Stat::Stat() {}
Stat::Stat(int base) {
    bases.emplace(-1, base);
}

Stat::Stat(const Stat &other) :
        value{other.value},
        bases{other.bases},
        base{other.base},
        shift{other.shift},
        multiplier{other.multiplier},
        divider{other.divider}{}
