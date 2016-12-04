#include "Controller.h"
#include "../action/Action.h"
#include "hostile.h"
#include <cstdlib>
#include <time.h>
#include <list>
#include "../State.h"

const HostileCharacter::ActionAndTarget HostileCharacter::getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions,
                                            const std::shared_ptr<State> &state)
{
	for(std::vector<ActionAndRange>::const_iterator a = actions.begin(); a != actions.end(); ++a)
	{
		std::shared_ptr<Action> action = (*a).action;
		if(action->actionType == Action::ATTACK)
		{
			for (std::vector<Position>::const_iterator r = (*a).range.begin(); r != (*a).range.end(); ++r)
			{
				std::list<std::shared_ptr<Entity>> entities = state->currentDungeon->getEntitiesAt(*r);
				for(std::list<std::shared_ptr<Entity>>::iterator entity = entities.begin(); entity!= entities.end(); entity++)
				{
					if((*entity)->isA(state->loader->getId("player")))
					{
						ActionAndTarget at;
						at.action = action;
						at.target = *r;
						at.targetEntity = *entity;
						return at;
					}
				}
			}
		}
	}
	std::vector<Position> ranges;
	ActionAndTarget at;
	for(std::vector<ActionAndRange>::const_iterator a = actions.begin(); a != actions.end(); ++a)
	{
		std::shared_ptr<Action> action = (*a).action;
		if (action->actionType == Action::MOVE)
		{
			at.action = action;
			for(std::vector<Position>::const_iterator r = (*a).range.begin(); r != (*a).range.end(); ++r)
			{
				ranges.push_back(*r);
			}
		}
	}
	srand (time(NULL));
	int i = rand() % ranges.size();
	at.target = ranges[i];
	return at;
}