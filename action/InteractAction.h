#pragma once

#include "Action.h"

class InteractAction: public Action {
public:
    InteractAction();

    int getRange(const Character &source) const override;
};



