#pragma once

#include "Action.h"

class BasicAttack : public Action {
public:
    BasicAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator, const int spell_denominator, const std::string name);

    int get_amount(Character &source) const override;

private:
    const int atk_numerator;
    const int atk_denominator;
    const int spell_numerator;
    const int spell_denominator;
};



