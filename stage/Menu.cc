#include <string>
#include <iostream>
#include "Menu.h"
#include "../Game.h"

using namespace std;

int Menu::run(Game &game) {
    string cmd;
	cout << "Please select a class. For a list of classes, type: list. for info on each class type: stats 'classname'." << endl;
	while(cin >> cmd){
		if(cmd == "list" || cmd == "List"){
			for(auto& pair:state->library.getRaces()) {
				cout << "(" << pair->shortcut <<") "<< pair->name <<", ";
			}
			cout << "(Rand) Random" << endl;
		} else if (cmd == "stats"|| cmd =="Stats"){
			cin >> cmd;
			for(auto& pair:state->library.getRaces()) {
				if(pair->name == cmd  || pair->shortcut == cmd){
					cout << pair->description << endl;
					break;
				}
			}
		} else if (cmd == "Rand" || cmd == "Random"){
			state->player->addFeatureSet(*state->library.getRaces()[uniform_int_distribution<>(0, state->library.getRaces().size()-1)(state->gen)]->featureSet);
			state->player->create();
			return next;
		} else {
			for(auto& pair:state->library.getRaces()) {
				if(pair->name == cmd  || pair->shortcut == cmd){
					state->player->addFeatureSet(*pair->featureSet);
					state->player->create();
					return next;
				}
			}
		}
	}
	return -1;
}

Menu::Menu(const shared_ptr<State> &state, const int id, const int next) : Stage(state, id), next(next) {}
