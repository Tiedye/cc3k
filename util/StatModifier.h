#pragma once

#include <cstdint>
#include "StatType.h"

struct StatModifier {
    StatType stat;
    enum Type {BASE, ADD, SUBTRACT, MULTIPLY, DIVIDE, MULTDIV};
    Type type;
    union {
        struct {
            std::int_least16_t base;
            std::int_least16_t priority;
        };
        struct {
            std::int_least16_t numerator;
            std::int_least16_t denominator;
        };
        std::int_least32_t amount;
    };
    StatModifier() = default;
    StatModifier(StatType stat, Type type, int_least32_t amount);
    StatModifier(StatType stat, Type type, int_least16_t one, int_least16_t two);
};



