#include "RangedAttack.h"

int RangedAttack::getRange(Character &source) const {
    return range;
}

RangedAttack::RangedAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator,
                           const int spellDenominator, const int range) : BasicAttack(atkNumerator, atkDenominator,
                                                                                      spellNumerator, spellDenominator),
                                                                          range(range) {}

int RangedAttack::getMinRange(Character &source) const {
    return 1;
}
