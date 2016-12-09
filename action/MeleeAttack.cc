#include "MeleeAttack.h"

int MeleeAttack::get_range(const Character &source) const {
    return 1;
}

int MeleeAttack::get_min_range(const Character &source) const {
    return 1;
}

MeleeAttack::MeleeAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator, const int spell_denominator) : BasicAttack(
        atk_numerator, atk_denominator, spell_numerator, spell_denominator, "Melee") {}
