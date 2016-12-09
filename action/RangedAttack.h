#pragma once

#include "BasicAttack.h"

class RangedAttack : public BasicAttack {
public:
    RangedAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator, const int spell_denominator,
                 const int range);

    int get_min_range(const Character &source) const override;

    int get_range(const Character &source) const override;

private:
    const int range;
};



