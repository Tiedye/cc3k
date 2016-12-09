#pragma once

#include "Action.h"

class EquipAction : public Action {
public:
    EquipAction();

    int get_range(const Character &source) const override;
};



