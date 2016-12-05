#include "MeleeAttack.h"

int MeleeAttack::getRange(const Character &source) const {
    return 1;
}

int MeleeAttack::getMinRange(const Character &source) const {
    return 1;
}

MeleeAttack::MeleeAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator, const int spellDenominator) : BasicAttack(
        atkNumerator, atkDenominator, spellNumerator, spellDenominator, "Melee") {}
