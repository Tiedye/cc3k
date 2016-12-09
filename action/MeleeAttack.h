#pragma once

#include "BasicAttack.h"

class MeleeAttack : public BasicAttack {
public:
    MeleeAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator, const int spell_denominator);

    int get_min_range(const Character &source) const override;

    int get_range(const Character &source) const override;
};



