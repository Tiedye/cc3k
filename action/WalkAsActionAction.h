#pragma once

#include "Action.h"

class WalkAsActionAction:public Action {
public:
    WalkAsActionAction(const std::vector<int> &walkable);

    int getRange(Character &source) const override;

    int getMinRange(Character &source) const override;
    bool canTarget(const Position position, const std::shared_ptr<State> &state) override;
private:
    const std::vector<int > walkable;

};



