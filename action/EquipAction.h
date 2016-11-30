#pragma once

#include "Action.h"

class EquipAction : public Action {
public:
    EquipAction();

    int getRange(Character &source) const override;
};



