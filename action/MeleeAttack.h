#pragma once

#include "BasicAttack.h"

class MeleeAttack : public BasicAttack {
public:
    using BasicAttack::BasicAttack;

    int getMinRange(Character &source) const override;

    int getRange(Character &source) const override;
};



