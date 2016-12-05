#include "RangedAttack.h"

int RangedAttack::getRange(const Character &source) const {
    return range;
}

RangedAttack::RangedAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator,
                           const int spellDenominator, const int range) : BasicAttack(atkNumerator, atkDenominator, spellNumerator, spellDenominator, "Ranged Attack"),
                                                                          range(range) {}

int RangedAttack::getMinRange(const Character &source) const {
    return 1;
}
