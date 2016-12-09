#pragma once

#include "Action.h"

class UnequipAction : public Action {
public:
    UnequipAction();

    int get_range(const Character &source) const override;
};



