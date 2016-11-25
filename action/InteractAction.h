#pragma once

#include "Action.h"

class InteractAction: public Action {
public:
    InteractAction();

    int getRange(Character &source) const override;
};



