#pragma once

#include <vector>
#include "Action.h"
#include "../CellType.h"

class WalkAction : public Action {
public:
    WalkAction(const std::vector<int> &walkable);

    int get_range(const Character &source) const override;

    int get_min_range(const Character &source) const override;
    bool can_target(const Position position, const std::shared_ptr<State> &state) override;
private:
    const std::vector<int > walkable;
};



