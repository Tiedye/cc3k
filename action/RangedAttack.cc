#include "RangedAttack.h"

int RangedAttack::get_range(const Character &source) const {
    return range;
}

RangedAttack::RangedAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator,
                           const int spell_denominator, const int range) : BasicAttack(atk_numerator, atk_denominator, spell_numerator, spell_denominator, "Ranged Attack"),
                                                                          range(range) {}

int RangedAttack::get_min_range(const Character &source) const {
    return 1;
}
