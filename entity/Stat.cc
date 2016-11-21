#include "Stat.h"

void Stat::update() {
    base = bases.rbegin()->second;
    value = (base + shift) * multiplier / divider;
    if (value < 0) value = 0;
}
