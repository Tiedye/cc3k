#pragma once

#include "Action.h"

class InteractAction: public Action {
public:
    InteractAction();

    int get_range(const Character &source) const override;
};



