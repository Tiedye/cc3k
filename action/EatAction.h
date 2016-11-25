#pragma once

#include "Action.h"

class EatAction : public Action {
public:
    EatAction();

    int getRange(Character &source) const override;
};



