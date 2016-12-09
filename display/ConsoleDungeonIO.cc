#include "ConsoleDungeonIO.h"

#include <curses.h>
#include <panel.h>
#include <menu.h>

#include <sstream>
#include <algorithm>
#include <iostream>

#include "../stage/Dungeon.h"
#include "../loader/Loader.h"

//#define NODISP

using namespace std;

ConsoleDungeonIO::ConsoleDungeonIO() {

}

Position get_direction(string dir, const Position position) {
    if (dir == "n" || dir == "no") { return position + Position(-1, 0); }
    else if (dir == "s" || dir == "so") { return position + Position(1, 0); }
    else if (dir == "w" || dir == "we") { return position + Position(0, -1); }
    else if (dir == "e" || dir == "ea") { return position + Position(0, 1); }
    else if (dir == "nw") { return position + Position(-1, -1); }
    else if (dir == "sw") { return position + Position(1, -1); }
    else if (dir == "se") { return position + Position(1, 1); }
    else if (dir == "ne") { return position + Position(-1, 1); }
    else { return {-1, -1}; }
}

const Controller::ActionAndTarget
ConsoleDungeonIO::get_action(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    update_h_u_d(character);
    update_display();
    // toggle between two control paths, command mode, and interactive mode
    Controller::ActionAndTarget action_and_target;
    bool got_action{false};
    while (!got_action) {
        if (mode == COMMAND) {

//            ostringstream msg;
//            msg << character->get_position() << endl;
//            for (auto p:actions[1].range) {
//                msg << p << ", ";
//            }
//            post_message(msg.str());
//            update_display();

            echo();
            curs_set(1);
            char raw_input[80];
            wmove(input_window, 0, 0);
            wgetnstr(input_window, raw_input, 80);
            wclear(input_window);
            update_display();
            string whole_command{raw_input};
            curs_set(0);
            noecho();

#ifdef NODISP
            auto& command_stream = cin;
#else
            istringstream command_stream{whole_command};
#endif
            string cmd;
            command_stream >> cmd;
            if (cmd == "v") {
                mode = INTERACTIVE;
                continue;
            } else if (cmd == "i") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::INTERACT; });
                string dir;
                if (command_stream >> dir) {
                    Position dest = get_direction(dir, character->get_position());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        action_and_target.action = selection->action;
                        action_and_target.target = dest;
                        got_action = true;
                    }
                }
            } else if (cmd == "a") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::ATTACK; });
                string dir;
                if (command_stream >> dir) {
                    Position dest = get_direction(dir, character->get_position());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        action_and_target.action = selection->action;
                        action_and_target.target = dest;
                        got_action = true;
                    }
                }
            } else if (cmd == "u") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::CONSUME; });
                string dir;
                if (command_stream >> dir) {
                    Position dest = get_direction(dir, character->get_position());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        action_and_target.action = selection->action;
                        action_and_target.target = dest;
                        got_action = true;
                    }
                }
            } else {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::MOVE; });
                Position dest = get_direction(cmd, character->get_position());
                if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                    action_and_target.action = selection->action;
                    action_and_target.target = dest;
                    got_action = true;
                }
            }

            if (!got_action) {
                cdout << "No cmd: " << whole_command << endl;
            }


        } else if (mode == INTERACTIVE) {
            noecho();
            auto current_selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::MOVE; });
            if (current_selection == actions.end()) current_selection = actions.begin();
            int key;
            bool selecting_cell{false};
            while (!got_action && mode == INTERACTIVE) {
                print_mode(current_selection->action->action_type);
                update_display();
                if (selecting_cell && (current_selection->action->targets & Action::EXTERNAL) == 0) {
                    disengage_selection();
                    selecting_cell = false;
                }
                if (!selecting_cell && current_selection->range.size() && (current_selection->action->targets & Action::EXTERNAL) != 0) {
                    engage_selection(current_selection->range, character->get_position());
                    update_display();
                    selecting_cell = true;
                }
                key = getch();
                switch (key) {
                    case 27:
                        action_and_target.action = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->action_type == Action::PASS; })->action;
                        got_action = true;
                        break;
                    case KEY_UP:
                        if (selecting_cell) selection_up();
                        break;
                    case KEY_DOWN:
                        if (selecting_cell) selection_down();
                        break;
                    case KEY_LEFT:
                        if (selecting_cell) selection_left();
                        break;
                    case KEY_RIGHT:
                        if (selecting_cell) selection_right();
                        break;
                    case '\t': {
                        shared_ptr<Item> selection = select_inventory(character);
                        if (selection && (current_selection->action->targets & Action::INVENTORY)) {
                            action_and_target.action = current_selection->action;
                            action_and_target.target_entity = selection;
                            if (selecting_cell) disengage_selection();
                            got_action = true;
                        }
                        break;
                    }
                    case '\n':
                    case ' ': {
                        if (selecting_cell) {
                            action_and_target.action = current_selection->action;
                            action_and_target.target = get_selection(character->get_position());
                            disengage_selection();
                            got_action = true;
                            break;
                        }
                    }
                    default: {
                        Action::Type action_type = Action::PASS;
                        switch (key) {
                            case 'a':
                                action_type = Action::ATTACK;
                                break;
                            case 'm':
                                action_type = Action::MOVE;
                                break;
                            case 'h':
                                action_type = Action::AID;
                                break;
                            case 'e':
                                action_type = Action::EQUIP;
                                break;
                            case 'u':
                                action_type = Action::UNEQUIP;
                                break;
                            case 'c':
                                action_type = Action::CONSUME;
                                break;
                            case 'p':
                                action_type = Action::INTERACT;
                                break;
                            default:
                                break;
                        }
                        if (action_type != Action::PASS) {
                            long num = count_if(actions.begin(), actions.end(), [action_type](ActionAndRange ar) { return ar.action->action_type == action_type; });
                            if (num) {
                                if (num == 1) {
                                    current_selection = find_if(actions.begin(), actions.end(), [action_type](ActionAndRange ar) { return ar.action->action_type == action_type; });
                                } else {
                                    vector<ITEM *> items;
                                    for (size_t i = {0}; i < actions.size(); ++i) {
                                        if (actions[i].action->action_type == action_type) {
                                            ITEM *item = new_item(actions[i].action->name.c_str(), "");
                                            set_item_userptr(item, (void *) (i + 1));
                                            items.push_back(item);
                                        }
                                    }
                                    current_selection = actions.begin() + (size_t(get_option(items)) - 1);
                                }
                                selecting_cell = false;
                                disengage_selection();
                            }
                        }
                        break;
                    }
                }
                update_display();
            }

        }
    }

    // TODO all user input during dungeon here
    return action_and_target;
}

void ConsoleDungeonIO::engage() {
    rows.resize(dungeon->height);
    cols.resize(dungeon->width);
#ifndef NODISP
    initscr();
#endif
    cdbuff.console_dungeon_i_o = this;
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(BASIC_COLOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(HIGHLIGHT_COLOR, COLOR_YELLOW, COLOR_WHITE);
    curs_set(0);

    mvvline(dungeon->height, dungeon->width / 3, ACS_VLINE, 5);
    dungeon_window = newwin(dungeon->height, dungeon->width, 0, 0);
    //box(dungeon_window, 0,0);
    player_window = newwin(5, dungeon->width / 3, dungeon->height, 0);
    //box(player_window, 0,0);
    message_window = newwin(6, dungeon->width * 2 / 3, dungeon->height, dungeon->width / 3 + 1);
    //box(message_window, 0,0);
    input_window = newwin(1, dungeon->width, dungeon->height + 5, 0);
    //box(input_window, 0,0);
    //inventory_window = newwin(dungeon->width - 6, dungeon->height - 6, 3, 3);

    scrollok(message_window, true);
    waddstr(message_window, "\n\n\n\n\n");

    dungeon_panel = new_panel(dungeon_window);
    message_panel = new_panel(message_window);
    input_panel = new_panel(input_window);
    player_panel = new_panel(player_window);
    //inventory_panel = new_panel(inventory_window);

    //hide_panel(inventory_panel);

    for (int y = 0; y < dungeon->height; ++y)
        for (int x = 0; x < dungeon->width; ++x) {
            draw_cell({y, x});
        }
    update_display();
}

void ConsoleDungeonIO::disengage() {
    cdbuff.console_dungeon_i_o = nullptr;
    endwin();
}

void ConsoleDungeonIO::entity_moved(const std::shared_ptr<Entity> &entity, const Position old_pos) {
    draw_cell(old_pos);
    draw_entity(entity);
    update_display();
}

void ConsoleDungeonIO::entity_added(const std::shared_ptr<Entity> &entity) {
    draw_entity(entity);
    update_display();
}

void ConsoleDungeonIO::entity_removed(const std::shared_ptr<Entity> &entity) {
    draw_cell(entity->get_position());
    entity->remove_listener(shared_from_this());
    update_display();
}

void ConsoleDungeonIO::entity_attacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) {
    ostringstream msg;
    msg << source->get_name() << " attacked " << target->get_name() << " for " << damage << " damage";
    post_message(msg.str());
    update_display();
}

void ConsoleDungeonIO::entity_missed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) {
    ostringstream msg;
    msg << source->get_name() << " tried to attack " << target->get_name();
    post_message(msg.str());
    update_display();
}

void ConsoleDungeonIO::entity_healed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) {
    ostringstream msg;
    msg << source->get_name() << " healed " << target->get_name() << " for " << heal << " health";
    post_message(msg.str());
    update_display();
}

void ConsoleDungeonIO::cell_changed(const Position position) {
    draw_cell(position);
    update_display();
}

void ConsoleDungeonIO::draw_cell(const Position position) {
    draw_cell(position, false);
}

void ConsoleDungeonIO::draw_cell(const Position position, const bool highlight) {
    static const chtype wall_out[] = {// a b l r
            ACS_DIAMOND,  // 0 0 0 0
            ACS_VLINE,    // 1 0 0 0
            ACS_VLINE,    // 0 1 0 0
            ACS_VLINE,    // 1 1 0 0
            ACS_HLINE,    // 0 0 1 0
            ACS_LRCORNER, // 1 0 1 0
            ACS_URCORNER, // 0 1 1 0
            ACS_RTEE,     // 1 1 1 0
            ACS_HLINE,    // 0 0 0 1
            ACS_LLCORNER, // 1 0 0 1
            ACS_ULCORNER, // 0 1 0 1
            ACS_LTEE,     // 1 1 0 1
            ACS_HLINE,    // 0 0 1 1
            ACS_BTEE,     // 1 0 1 1
            ACS_TTEE,     // 0 1 1 1
            ACS_PLUS,     // 1 1 1 1
    };
    switch (dungeon->get_cell_type(position)) {
        case EMPTY: {
            set_standard_cell(dungeon_window, highlight);
            mvwaddch(dungeon_window, position.y, position.x, ' ');
            break;
        }
        case WALL: {
            CellType ct{dungeon->get_cell_type({position.y - 1, position.x})};
            bool wall_above{ct == WALL || ct == OPEN_DOOR || ct == CLOSED_DOOR};
            ct = dungeon->get_cell_type({position.y + 1, position.x});
            bool wall_below{ct == WALL || ct == OPEN_DOOR || ct == CLOSED_DOOR};
            ct = dungeon->get_cell_type({position.y, position.x - 1});
            bool wall_left{ct == WALL || ct == OPEN_DOOR || ct == CLOSED_DOOR};
            ct = dungeon->get_cell_type({position.y, position.x + 1});
            bool wall_right{ct == WALL || ct == OPEN_DOOR || ct == CLOSED_DOOR};
            set_standard_cell(dungeon_window, highlight);
            int wall_index{wall_above | wall_below << 1 | wall_left << 2 | wall_right << 3};
            mvwaddch(dungeon_window, position.y, position.x, wall_out[wall_index]);
            break;
        }
        case FLOOR: {
            set_walkable_cell(dungeon_window, highlight);
            mvwaddch(dungeon_window, position.y, position.x, ' ');
            break;
        }
        case HALL: {
            set_walkable_cell(dungeon_window, highlight);
            mvwaddch(dungeon_window, position.y, position.x, ACS_CKBOARD);
            break;
        }
        case OPEN_DOOR: {
            set_walkable_cell(dungeon_window, highlight);
            mvwaddch(dungeon_window, position.y, position.x, '+');
            break;
        }
        case CLOSED_DOOR:
            break;
    }
    auto entity = dungeon->get_entity_at(position);
    if (entity) {
        draw_entity(entity, highlight);
    }
}

void ConsoleDungeonIO::draw_entity(const std::shared_ptr<Entity> &entity) {
    draw_entity(entity, false);
}

void ConsoleDungeonIO::draw_entity(const std::shared_ptr<Entity> &entity, const bool highlight) {
    switch (dungeon->get_cell_type(entity->get_position())) {
        case EMPTY:
        case WALL:
            set_standard_cell(dungeon_window, highlight);
            break;
        case FLOOR:
        case HALL:
        case OPEN_DOOR:
            set_walkable_cell(dungeon_window, highlight);
            break;
        case CLOSED_DOOR:
            break;
    }
    mvwaddch(dungeon_window, entity->get_position().y, entity->get_position().x, entity->representation);
}

void ConsoleDungeonIO::set_walkable_cell(WINDOW *win, bool highlight) {
    wattrset(win, A_BOLD | (highlight ? COLOR_PAIR(HIGHLIGHT_COLOR) : COLOR_PAIR(BASIC_COLOR)) | A_STANDOUT);
}

void ConsoleDungeonIO::set_standard_cell(WINDOW *win, bool highlight) {
    wattrset(win, (highlight ? COLOR_PAIR(HIGHLIGHT_COLOR) : COLOR_PAIR(BASIC_COLOR)) | A_STANDOUT);
}

void ConsoleDungeonIO::post_message(std::string s) {
    waddstr(message_window, s.c_str());
    waddch(message_window, '\n');
}

void ConsoleDungeonIO::update_display() {
    update_panels();
    doupdate();
}

void ConsoleDungeonIO::update_h_u_d(const std::shared_ptr<Character> &character) {
    ostringstream out;
    out << "HP:   " << character->get_health() << endl;
    out << "Atk:  " << character->get_attack_strength() << endl;
    out << "Def:  " << character->get_defence_strength() << endl;
    out << "Gold: " << character->current_gold() << endl;
    mvwaddstr(player_window, 0, 0, out.str().c_str());
}

void ConsoleDungeonIO::engage_selection(const std::vector<Position> &range, const Position origin) {
    for (auto &row:rows) row.clear();
    for (auto &col:cols) col.clear();
    Position min{range.front()};
    Position max{range.front()};
    int height{int(rows.size())};
    int width{int(cols.size())};
    for (auto pos:range) {
        if (pos.y < min.y) min.y = pos.y;
        if (pos.y > max.y) max.y = pos.y;
        if (pos.x < min.x) min.x = pos.x;
        if (pos.x > max.x) max.x = pos.x;
        rows[pos.y].insert(pos);
        cols[pos.x].insert(pos);
    }
    for (int row{0}; row < height; ++row) {
        auto &current_row = rows[row];
        int row_size{int(current_row.size())};
        if (row_size) {
            int row_min_x{current_row.begin()->x};
            int row_max_x{current_row.rbegin()->x};
            if (row_min_x > min.x) {
                for (int dist{1};; ++dist) {
                    if (row + dist < height) {
                        auto &other_row = rows[row + dist];
                        for (Position p{row + dist, row_min_x - 1}; p.x >= min.x; --p.x) {
                            if (other_row.find(p) != other_row.end()) {
                                current_row.insert(p);
                                break;
                            }
                        }
                        if (int(current_row.size()) != row_size) break;
                    }
                    if (row - dist > 0) {
                        auto &other_row = rows[row - dist];
                        for (Position p{row - dist, row_min_x - 1}; p.x >= min.x; --p.x) {
                            if (other_row.find(p) != other_row.end()) {
                                current_row.insert(p);
                                break;
                            }
                        }
                        if (int(current_row.size()) != row_size) break;
                    }
                }
            }
            row_size = int(current_row.size());
            if (row_max_x < max.x) {
                for (int dist{1};; ++dist) {
                    if (row + dist < height) {
                        auto &other_row = rows[row + dist];
                        for (Position p{row + dist, row_max_x + 1}; p.x <= max.x; ++p.x) {
                            if (other_row.find(p) != other_row.end()) {
                                current_row.insert(p);
                                break;
                            }
                        }
                        if (int(current_row.size()) != row_size) break;
                    }
                    if (row - dist > 0) {
                        auto &other_row = rows[row - dist];
                        for (Position p{row - dist, row_max_x + 1}; p.x <= max.x; ++p.x) {
                            if (other_row.find(p) != other_row.end()) {
                                current_row.insert(p);
                                break;
                            }
                        }
                        if (int(current_row.size()) != row_size) break;
                    }
                }
            }
        }
    }
    for (int col{0}; col < width; ++col) {
        auto &current_col = cols[col];
        int col_size{int(current_col.size())};
        if (col_size) {
            int col_min_y{current_col.begin()->y};
            int col_max_y{current_col.rbegin()->y};
            if (col_min_y > min.y) {
                for (int dist{1};; ++dist) {
                    if (col + dist < width) {
                        auto &other_col = cols[col + dist];
                        for (Position p{col_min_y - 1, col + dist}; p.y >= min.y; --p.y) {
                            if (other_col.find(p) != other_col.end()) {
                                current_col.insert(p);
                                break;
                            }
                        }
                        if (int(current_col.size()) != col_size) break;
                    }
                    if (col - dist > 0) {
                        auto &other_col = cols[col - dist];
                        for (Position p{col_min_y - 1, col - dist}; p.y >= min.y; --p.y) {
                            if (other_col.find(p) != other_col.end()) {
                                current_col.insert(p);
                                break;
                            }
                        }
                        if (int(current_col.size()) != col_size) break;
                    }
                }
            }
            col_size = int(current_col.size());
            if (col_max_y < max.y) {
                for (int dist{1};; ++dist) {
                    if (col + dist < width) {
                        auto &other_col = cols[col + dist];
                        for (Position p{col_max_y + 1, col + dist}; p.y <= max.y; ++p.y) {
                            if (other_col.find(p) != other_col.end()) {
                                current_col.insert(p);
                                break;
                            }
                        }
                        if (int(current_col.size()) != col_size) break;
                    }
                    if (col - dist > 0) {
                        auto &other_col = cols[col - dist];
                        for (Position p{col_max_y + 1, col - dist}; p.y <= max.y; ++p.y) {
                            if (other_col.find(p) != other_col.end()) {
                                current_col.insert(p);
                                break;
                            }
                        }
                        if (int(current_col.size()) != col_size) break;
                    }
                }
            }
        }
    }

    current_selection = {-1, -1};
    if (find(range.begin(), range.end(), origin + delta) != range.end()) {
        current_selection = origin + delta;
    } else {
        Position etas[]{
                {-1, 1},
                {0,  1},
                {1,  1},
                {-1, -1},
                {0,  -1},
                {1,  -1},
                {1,  0},
                {-1, 0}
        };
        for (auto eta:etas) {
            if (find(range.begin(), range.end(), origin + delta + eta) != range.end()) {
                current_selection = origin + delta + eta;
                break;
            }
        }
    }
    if (current_selection == Position{-1, -1}) current_selection = range.front();
    draw_cell(current_selection, true);
}

void ConsoleDungeonIO::disengage_selection() {
    draw_cell(current_selection);
}

void ConsoleDungeonIO::selection_up() {
    auto current_y = find(cols[current_selection.x].begin(), cols[current_selection.x].end(), current_selection);
    if (current_y != cols[current_selection.x].begin()) {
        --current_y;
        draw_cell(current_selection);
        current_selection = *current_y;
        draw_cell(current_selection, true);
    }
}

void ConsoleDungeonIO::selection_down() {
    auto current_y = find(cols[current_selection.x].begin(), cols[current_selection.x].end(), current_selection);
    ++current_y;
    if (current_y != cols[current_selection.x].end()) {
        draw_cell(current_selection);
        current_selection = *current_y;
        draw_cell(current_selection, true);
    }
}

void ConsoleDungeonIO::selection_left() {
    auto current_x = find(rows[current_selection.y].begin(), rows[current_selection.y].end(), current_selection);
    if (current_x != rows[current_selection.y].begin()) {
        --current_x;
        draw_cell(current_selection);
        current_selection = *current_x;
        draw_cell(current_selection, true);
    }
}

void ConsoleDungeonIO::selection_right() {
    auto current_x = find(rows[current_selection.y].begin(), rows[current_selection.y].end(), current_selection);
    ++current_x;
    if (current_x != rows[current_selection.y].end()) {
        draw_cell(current_selection);
        current_selection = *current_x;
        draw_cell(current_selection, true);
    }
}

Position ConsoleDungeonIO::get_selection(const Position origin) {
    delta = current_selection - origin;
    return current_selection;
}

void *ConsoleDungeonIO::get_option(std::vector<ITEM *> items) {
    items.push_back(NULL);
    MENU *menu = new_menu(items.data());
    WINDOW *menu_window = newwin(10, 40, dungeon->height / 2 - 5, dungeon->width / 2 - 20);
    PANEL *menu_panel = new_panel(menu_window);
    set_menu_win(menu, menu_window);
    set_menu_sub(menu, derwin(menu_window, 8, 38, 1, 1));
    set_menu_format(menu, 8, 1);
    set_menu_mark(menu, " * ");
    box(menu_window, 0, 0);
    post_menu(menu);
    update_display();
    int key;
    void *result = nullptr;
    while (result == nullptr) {
        key = getch();
        switch (key) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case KEY_NPAGE:
                menu_driver(menu, REQ_SCR_DPAGE);
                break;
            case KEY_PPAGE:
                menu_driver(menu, REQ_SCR_UPAGE);
                break;
            case '\n':
                result = item_userptr(current_item(menu));
                break;
            default:
                break;
        }
        update_display();
    }
    unpost_menu(menu);
    free_menu(menu);
    for (auto item:items) {
        if (item) free_item(item);
    }
    del_panel(menu_panel);
    delwin(menu_window);
    update_display();
    return result;
}

void ConsoleDungeonIO::show_info(WINDOW *win, Item *item) {
    ostringstream out;
    out << item->get_name() << endl << endl;
    out << "Value: " << item->get_value() << endl;
    if (item->equippable()) {
        out << "Equippable" << endl;
        if (item->equipped_slot() != -1) {
            out << "Equipped to slot: " << (dungeon->get_state()->loader->get_name(item->equipped_slot())) << endl;
        }
    }
    wclear(win);
    mvwaddstr(win, 0, 0, out.str().c_str());
    wrefresh(win);
    update_display();
}

std::shared_ptr<Item> ConsoleDungeonIO::select_inventory(const std::shared_ptr<Character> &character) {

    std::vector<ITEM *> items;
    for (auto &item:character->get_inventory()) {
        ITEM *item_ptr = new_item(item->get_name().c_str(), "");
        set_item_userptr(item_ptr, item.get());
        items.push_back(item_ptr);
    }
    items.push_back(NULL);
    MENU *menu = new_menu(items.data());
    WINDOW *window = newwin(dungeon->height, dungeon->width - 6, 3, 3);
    PANEL *window_panel = new_panel(window);
    WINDOW *info_window = derwin(window, dungeon->height - 2, (dungeon->width - 8) / 2, 1, (dungeon->width - 6) / 2);
    set_menu_win(menu, window);
    set_menu_sub(menu, derwin(window, dungeon->height - 2, (dungeon->width - 8) / 2 - 1, 1, 1));
    set_menu_format(menu, dungeon->height - 3, 1);
    set_menu_mark(menu, " * ");
    box(window, 0, 0);
    mvwvline(window, 1, (dungeon->width - 6) / 2 - 1, ACS_VLINE, dungeon->height - 2);
    post_menu(menu);
    update_display();

    int key;
    Item *result = nullptr;
    bool has_result = false;
    if (current_item(menu)) {
        show_info(info_window, (Item *) item_userptr(current_item(menu)));
    }
    while (!has_result) {
        key = getch();
        switch (key) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                show_info(info_window, (Item *) item_userptr(current_item(menu)));
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                show_info(info_window, (Item *) item_userptr(current_item(menu)));
                break;
            case KEY_NPAGE:
                menu_driver(menu, REQ_SCR_DPAGE);
                show_info(info_window, (Item *) item_userptr(current_item(menu)));
                break;
            case KEY_PPAGE:
                menu_driver(menu, REQ_SCR_UPAGE);
                show_info(info_window, (Item *) item_userptr(current_item(menu)));
                break;
            case '\t':
                has_result = true;
                break;
            case '\n':
                has_result = true;
                result = (Item *) item_userptr(current_item(menu));
                break;
            default:
                break;
        }
        update_display();
    }
    unpost_menu(menu);
    free_menu(menu);
    for (auto item:items) {
        if (item) free_item(item);
    }
    del_panel(window_panel);
    delwin(window);
    update_display();
    if (result) {
        return result->shared_from_base<Item>();
    } else {
        return nullptr;
    }
}

void ConsoleDungeonIO::print_mode(Action::Type action_type) {
    ostringstream out;
    out << "Mode: ";
    switch (action_type) {
        case Action::PASS:
            out << "Pass";
            break;
        case Action::MOVE:
            out << "Move";
            break;
        case Action::CONSUME:
            out << "Consume";
            break;
        case Action::EQUIP:
            out << "Equip";
            break;
        case Action::UNEQUIP:
            out << "Unequip";
            break;
        case Action::INTERACT:
            out << "Pickup/Interact";
            break;
        case Action::ATTACK:
            out << "Attack";
            break;
        case Action::AID:
            out << "Aid";
            break;
        case Action::EFFECT:
            out << "Effect";
            break;
    }
    wmove(player_window, 4, 0);
    wclrtoeol(player_window);
    mvwaddstr(player_window, 4, 0, out.str().c_str());
}

ConsoleDungeonIO::out_buff::out_buff(ConsoleDungeonIO *console_dungeon_i_o) : console_dungeon_i_o(console_dungeon_i_o) {}

int ConsoleDungeonIO::out_buff::sync() {
    if (console_dungeon_i_o) {
#ifdef NODISP
        cout << str();
#else
        console_dungeon_i_o->post_message(str());
        console_dungeon_i_o->update_display();
#endif
        str("");
    }
    return basic_streambuf::sync();
}

ConsoleDungeonIO::out_buff cdbuff{nullptr};
std::ostream cdout{&cdbuff};

bool ConsoleDungeonIO::SortPositionByX::operator()(const Position &lhs, const Position &rhs) const {
    if (lhs.x < rhs.x)
        return true;
    return rhs.x >= lhs.x && lhs.y < rhs.y;
}
