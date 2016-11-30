#pragma once


#include "Stage.h"

class EndScreen : public Stage {
public:
    EndScreen(const std::shared_ptr<State> &state, const int returnId);

    int run(Game &game) override;

private:
    const int returnId;
};



