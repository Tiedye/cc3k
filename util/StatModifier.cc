#include "StatModifier.h"

StatModifier::StatModifier(StatType stat, StatModifier::Type type, int_least32_t amount) : stat{stat}, type{type}, amount{amount} {

}

StatModifier::StatModifier(StatType stat, StatModifier::Type type, int_least16_t one, int_least16_t two): stat{stat}, type{type}, base{one}, priority{two} {

}