#pragma once

#include <vector>
#include "Action.h"
#include "../CellType.h"

class WalkAction : public Action {
public:
    WalkAction(const std::vector<int> &walkable);

    int getRange(const Character &source) const override;

    int getMinRange(const Character &source) const override;
    bool canTarget(const Position position, const std::shared_ptr<State> &state) override;
private:
    const std::vector<int > walkable;
};



