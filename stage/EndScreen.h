#pragma once


#include "Stage.h"

class EndScreen : public Stage {
public:
    EndScreen(const std::shared_ptr<State> &state, const int return_id);

    int run(Game &game) override;

private:
    const int return_id;
};



