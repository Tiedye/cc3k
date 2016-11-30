#pragma once

#include "BasicAttack.h"

class RangedAttack : public BasicAttack {
public:
    RangedAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator, const int spellDenominator,
                 const int range);

    int getMinRange(Character &source) const override;

    int getRange(Character &source) const override;

private:
    const int range;
};



