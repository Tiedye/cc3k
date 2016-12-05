#pragma once

#include "Action.h"

class EatAction : public Action {
public:
    EatAction();

    int getRange(const Character &source) const override;
};



