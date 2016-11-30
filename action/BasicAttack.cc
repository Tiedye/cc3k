#include "BasicAttack.h"

#include "../entity/Entity.h"
#include "../entity/Character.h"

BasicAttack::BasicAttack(const int atkNumerator, const int atkDenominator, const int spellNumerator,
                         const int spellDenominator) : Action(Type::ATTACK, ACTION, EXTERNAL),
                                                       atkNumerator(atkNumerator), atkDenominator(atkDenominator),
                                                       spellNumerator(spellNumerator),
                                                       spellDenominator(spellDenominator) {}

int BasicAttack::getAmount(Character &source) const {
    return source.getAttackStrength() * atkNumerator / atkDenominator +
           source.getSpellStrength() * spellNumerator / spellDenominator;
}
