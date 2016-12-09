#include "BasicAttack.h"

#include "../entity/Entity.h"
#include "../entity/Character.h"

BasicAttack::BasicAttack(const int atk_numerator, const int atk_denominator, const int spell_numerator, const int spell_denominator, const std::string name) : Action(Type::ATTACK, name, ACTION, EXTERNAL, PATH, SQUARE, 0, 0),
                                                                                                                                                           atk_numerator(atk_numerator), atk_denominator(atk_denominator),
                                                                                                                                                           spell_numerator(spell_numerator),
                                                                                                                                                           spell_denominator(spell_denominator) {}

int BasicAttack::get_amount(Character &source) const {
    return source.get_attack_strength() * atk_numerator / atk_denominator +
           source.get_spell_strength() * spell_numerator / spell_denominator;
}
