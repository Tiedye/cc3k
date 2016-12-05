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
				cout << "(" << pair->shortcut <<") "<< pair.second->name <<", ";
			}
			cout << "(Rand) Random" << endl;
		} else if (cmd == "stats"|| cmd =="Stats"){
			cin >> cmd;
			for(auto& pair:state->library.getRaces()) {
				if(pair.name == cmd  || pair.shortcut == cmd){
					cout << pair.description << endl;
					break;
				}
			}
		} else {
			for(auto& pair:state->library.getRaces()) {
				if(pair.name == cmd  || pair.shortcut == cmd){
					state->player->addFeatureSet(*state->library.getRaceSet(state->library.getRaces().begin()->first));
					break;
				}
			}
		}
	}
    return next;
}

Menu::Menu(const shared_ptr<State> &state, const int id, const int next) : Stage(state, id), next(next) {}
