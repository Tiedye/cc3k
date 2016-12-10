#include <string>
#include <sstream>
#include <iostream>
#include "Menu.h"
#include "../Game.h"

#include <curses.h>
#include <panel.h>
#include <menu.h>

#include "../util/curses_util.h"

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

    initscr();
    clear();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int height{16};
    int width {60};

    vector<ITEM *> items;
    for (auto& race:state->library.get_races()) {
        ITEM *item = new_item(race->name.c_str(), "");
        set_item_userptr(item, race.get());
        items.push_back(item);
    }
	items.push_back(NULL);
	MENU *menu = new_menu(items.data());
    int screen_height {getmaxy(stdscr)};
    int screen_width {getmaxx(stdscr)};
	WINDOW *menu_window = newwin(height, width,  (screen_height-height)/2, (screen_width-width)/2);
    WINDOW *info_window = derwin(menu_window, height-4, width*3/4-2, 3, width/4+1);
    print_centered(menu_window, "Choose a Race", 1);
	PANEL *menu_panel = new_panel(menu_window);
	set_menu_win(menu, menu_window);
	set_menu_sub(menu, derwin(menu_window, height-4, width/4-1, 3, 1));
	set_menu_format(menu, height-4, 1);
	set_menu_mark(menu, " *");
	box(menu_window, 0, 0);
	post_menu(menu);
	update_panels();
    doupdate();
	int key;
	ITEM *result = nullptr;
    show_info(info_window, (Race *)item_userptr(current_item(menu)));
	while (result == nullptr) {
		key = getch();
		switch (key) {
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
                show_info(info_window, (Race *)item_userptr(current_item(menu)));
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
                show_info(info_window, (Race *)item_userptr(current_item(menu)));
				break;
			case KEY_NPAGE:
				menu_driver(menu, REQ_SCR_DPAGE);
                show_info(info_window, (Race *)item_userptr(current_item(menu)));
				break;
			case KEY_PPAGE:
				menu_driver(menu, REQ_SCR_UPAGE);
                show_info(info_window, (Race *)item_userptr(current_item(menu)));
				break;
			case '\n':
				result = current_item(menu);
				break;
			default:
				break;
		}
        update_panels();
        doupdate();
	}
    state->player->add_feature_set(*((Race*)item_userptr(result))->feature_set);
    state->player->create();
	unpost_menu(menu);
	free_menu(menu);
	for (auto item:items) {
		if (item) free_item(item);
	}
	del_panel(menu_panel);
	delwin(menu_window);
    update_panels();
    doupdate();

	return next;
}

Menu::Menu(const shared_ptr<State> &state) : Stage(state, Game::get_id()), next(Game::next_id()) {}

void Menu::show_info(WINDOW *win, Race *race) {
    ostringstream out;
    out << race->name << endl << endl;
    out << race->description << endl;
    wclear(win);
    mvwaddstr(win, 0, 0, out.str().c_str());
    wrefresh(win);
    update_panels();
    doupdate();
}
