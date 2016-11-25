#pragma once

#include "Action.h"

class WalkAction : public Action {
public:
    WalkAction();

    int getRange(Character &source) const override;

    int getMinRange(Character &source) const override;
};



