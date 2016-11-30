#pragma once

#include "Action.h"

class WalkAsActionAction:public Action {
public:
    WalkAsActionAction();

    int getRange(Character &source) const override;

    int getMinRange(Character &source) const override;

};



