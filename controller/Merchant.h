#pragma once

#include "Controller.h"

struct HostileCharacter : public Controller
{
	const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
		const std::shared_ptr<State> &state);
};