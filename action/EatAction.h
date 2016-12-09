#pragma once

#include "Action.h"

class EatAction : public Action {
public:
    EatAction();

    int get_range(const Character &source) const override;
};



