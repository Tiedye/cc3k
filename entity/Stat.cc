#include "Stat.h"

void Stat::update() {
    base = bases.rbegin()->second;
    value = (base + shift) * multiplier / divider;
}
