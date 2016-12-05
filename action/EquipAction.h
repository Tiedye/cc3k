#pragma once

#include "Action.h"

class EquipAction : public Action {
public:
    EquipAction();

    int getRange(const Character &source) const override;
};



