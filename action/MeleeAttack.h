#pragma once

#include "BasicAttack.h"

class MeleeAttack : public BasicAttack {
public:
    MeleeAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator, const int spellDenominator);

    int getMinRange(const Character &source) const override;

    int getRange(const Character &source) const override;
};



