#pragma once

#include "Action.h"

class UnequipAction : public Action {
public:
    UnequipAction();

    int getRange(const Character &source) const override;
};



