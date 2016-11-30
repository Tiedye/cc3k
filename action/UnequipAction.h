#pragma once

#include "Action.h"

class UnequipAction : public Action {
public:
    UnequipAction();

    int getRange(Character &source) const override;
};



