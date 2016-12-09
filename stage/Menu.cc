#include <string>
#include <iostream>
#include "Menu.h"
#include "../Game.h"

using namespace std;

int Menu::run(Game &game) {
    // reset player
    state->player->clear_inventory();
    state->player->set_score(0);
    state->player->remove_gold(state->player->current_gold());
    state->player->clear_temporary_feature_sets();
    if (last_race != -1) {
        for(auto& race:state->library.get_races()) {
            if (race->id == last_race) {
                state->player->remove_feature_set(*race->feature_set);
            }
        }
    }

    string cmd;
	cout << "Please select a class. For a list of classes, type: list. for info on each class type: stats 'classname'." << endl;
	while(cin >> cmd){
		if(cmd == "list" || cmd == "List"){
			for(auto& pair:state->library.get_races()) {
				cout << "(" << pair->shortcut <<") "<< pair->name <<", ";
			}
			cout << "(Rand) Random" << endl;
		} else if (cmd == "stats"|| cmd =="Stats"){
			cin >> cmd;
			for(auto& pair:state->library.get_races()) {
				if(pair->name == cmd  || pair->shortcut == cmd){
					cout << pair->description << endl;
					break;
				}
			}
		} else if (cmd == "Rand" || cmd == "Random"){
			state->player->add_feature_set(*state->library.get_races()[uniform_int_distribution<>(0, state->library.get_races().size()-1)(state->gen)]->feature_set);
			state->player->create();
			return next;
		} else {
			for(auto& race:state->library.get_races()) {
				if(race->name == cmd  || race->shortcut == cmd){
                    last_race = race->id;
					state->player->add_feature_set(*race->feature_set);
					state->player->create();
					return next;
				}
			}
		}
	}
	return -1;
}

Menu::Menu(const shared_ptr<State> &state) : Stage(state, Game::get_id()), next(Game::next_id()) {}
