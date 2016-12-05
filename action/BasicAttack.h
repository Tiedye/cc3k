#pragma once

#include "Action.h"

class BasicAttack : public Action {
public:
    BasicAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator, const int spellDenominator, const std::string name);

    int getAmount(Character &source) const override;

private:
    const int atkNumerator;
    const int atkDenominator;
    const int spellNumerator;
    const int spellDenominator;
};



