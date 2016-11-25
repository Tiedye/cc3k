#pragma once

#include "BasicAttack.h"

class MeleeAttack : public BasicAttack {
public:
    using BasicAttack::BasicAttack;

    int getMinRange(std::shared_ptr<Character> &source) const override;

    int getRange(std::shared_ptr<Character> &source) const override;
};



