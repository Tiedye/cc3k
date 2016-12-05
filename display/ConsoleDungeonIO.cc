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

Position getDirection(string dir, const Position position) {
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
ConsoleDungeonIO::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    updateHUD(character);
    updateDisplay();
    // toggle between two control paths, command mode, and interactive mode
    Controller::ActionAndTarget actionAndTarget;
    bool gotAction{false};
    while (!gotAction) {
        if (mode == COMMAND) {

//            ostringstream msg;
//            msg << character->getPosition() << endl;
//            for (auto p:actions[1].range) {
//                msg << p << ", ";
//            }
//            postMessage(msg.str());
//            updateDisplay();

            echo();
            curs_set(1);
            char raw_input[80];
            wmove(inputWindow, 0, 0);
            wgetnstr(inputWindow, raw_input, 80);
            wclear(inputWindow);
            updateDisplay();
            string wholeCommand{raw_input};
            curs_set(0);
            noecho();

#ifdef NODISP
            auto& commandStream = cin;
#else
            istringstream commandStream{wholeCommand};
#endif
            string cmd;
            commandStream >> cmd;
            if (cmd == "v") {
                mode = INTERACTIVE;
                continue;
            } else if (cmd == "i") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::INTERACT; });
                string dir;
                if (commandStream >> dir) {
                    Position dest = getDirection(dir, character->getPosition());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        actionAndTarget.action = selection->action;
                        actionAndTarget.target = dest;
                        gotAction = true;
                    }
                }
            } else if (cmd == "a") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::ATTACK; });
                string dir;
                if (commandStream >> dir) {
                    Position dest = getDirection(dir, character->getPosition());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        actionAndTarget.action = selection->action;
                        actionAndTarget.target = dest;
                        gotAction = true;
                    }
                }
            } else if (cmd == "u") {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::CONSUME; });
                string dir;
                if (commandStream >> dir) {
                    Position dest = getDirection(dir, character->getPosition());
                    if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                        actionAndTarget.action = selection->action;
                        actionAndTarget.target = dest;
                        gotAction = true;
                    }
                }
            } else {
                auto selection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::MOVE; });
                Position dest = getDirection(cmd, character->getPosition());
                if (find(selection->range.begin(), selection->range.end(), dest) != selection->range.end()) {
                    actionAndTarget.action = selection->action;
                    actionAndTarget.target = dest;
                    gotAction = true;
                }
            }

            if (!gotAction) {
                cdout << "No cmd: " << wholeCommand << endl;
            }


        } else if (mode == INTERACTIVE) {
            noecho();
            auto currentSelection = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::MOVE; });
            if (currentSelection == actions.end()) currentSelection  = actions.begin();
            int key;
            bool selectingCell {false};
            while (!gotAction && mode == INTERACTIVE) {
                printMode(currentSelection->action->actionType);
                updateDisplay();
                if (selectingCell && (currentSelection->action->targets & Action::EXTERNAL) == 0) {
                    disengageSelection();
                    selectingCell = false;
                }
                if (!selectingCell && currentSelection->range.size() && (currentSelection->action->targets & Action::EXTERNAL) != 0) {
                    engageSelection(currentSelection->range, character->getPosition());
                    updateDisplay();
                    selectingCell = true;
                }
                key = getch();
                switch (key) {
                    case 27:
                        actionAndTarget.action = find_if(actions.begin(), actions.end(), [](ActionAndRange ar) { return ar.action->actionType == Action::PASS; })->action;
                        gotAction = true;
                        break;
                    case KEY_UP:
                        if (selectingCell) selectionUp();
                        break;
                    case KEY_DOWN:
                        if (selectingCell) selectionDown();
                        break;
                    case KEY_LEFT:
                        if (selectingCell) selectionLeft();
                        break;
                    case KEY_RIGHT:
                        if (selectingCell) selectionRight();
                        break;
                    case '\t': {
                        shared_ptr<Item> selection = selectInventory(character);
                        if (selection && (currentSelection->action->targets & Action::INVENTORY)) {
                            actionAndTarget.action = currentSelection->action;
                            actionAndTarget.targetEntity = selection;
                            if (selectingCell) disengageSelection();
                            gotAction = true;
                        }
                        break;
                    }
                    case '\n':
                    case ' ':{
                        if (selectingCell) {
                            actionAndTarget.action = currentSelection->action;
                            actionAndTarget.target = getSelection(character->getPosition());
                            disengageSelection();
                            gotAction = true;
                            break;
                        }
                    }
                    default: {
                        Action::Type actionType = Action::PASS;
                        switch (key) {
                            case 'a':
                                actionType = Action::ATTACK;
                                break;
                            case 'm':
                                actionType = Action::MOVE;
                                break;
                            case 'h':
                                actionType = Action::AID;
                                break;
                            case 'e':
                                actionType = Action::EQUIP;
                                break;
                            case 'u':
                                actionType = Action::UNEQUIP;
                                break;
                            case 'c':
                                actionType = Action::CONSUME;
                                break;
                            case 'p':
                                actionType = Action::INTERACT;
                                break;
                            default:
                                break;
                        }
                        if (actionType != Action::PASS) {
                            long num = count_if(actions.begin(), actions.end(), [actionType](ActionAndRange ar) { return ar.action->actionType == actionType; });
                            if (num) {
                                if (num == 1) {
                                    currentSelection = find_if(actions.begin(), actions.end(), [actionType](ActionAndRange ar) { return ar.action->actionType == actionType; });
                                } else {
                                    vector<ITEM *> items;
                                    for (size_t i = {0}; i < actions.size(); ++i) {
                                        if (actions[i].action->actionType == actionType) {
                                            ITEM *newItem = new_item(actions[i].action->name.data(), "");
                                            set_item_userptr(newItem, (void *) i);
                                            items.push_back(newItem);
                                        }
                                    }
                                    currentSelection = actions.begin() + size_t(getOption(items));
                                }
                                selectingCell = false;
                                disengageSelection();
                            }
                        }
                        break;
                    }
                }
                updateDisplay();
            }

        }
    }

    // TODO all user input during dungeon here
    return actionAndTarget;
}

void ConsoleDungeonIO::engage() {
    rows.resize(dungeon->height);
    cols.resize(dungeon->width);
#ifndef NODISP
    initscr();
#endif
    cdbuff.consoleDungeonIO = this;
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(BASIC_COLOR, COLOR_BLACK, COLOR_WHITE);
    init_pair(HIGHLIGHT_COLOR, COLOR_YELLOW, COLOR_WHITE);
    curs_set(0);

    mvvline(dungeon->height, dungeon->width / 3, ACS_VLINE, 5);
    dungeonWindow = newwin(dungeon->height, dungeon->width, 0, 0);
    //box(dungeonWindow, 0,0);
    playerWindow = newwin(5, dungeon->width / 3, dungeon->height, 0);
    //box(playerWindow, 0,0);
    messageWindow = newwin(6, dungeon->width * 2 / 3, dungeon->height, dungeon->width / 3 + 1);
    //box(messageWindow, 0,0);
    inputWindow = newwin(1, dungeon->width, dungeon->height + 5, 0);
    //box(inputWindow, 0,0);
    //inventoryWindow = newwin(dungeon->width - 6, dungeon->height - 6, 3, 3);

    scrollok(messageWindow, true);
    waddstr(messageWindow, "\n\n\n\n\n");

    dungeonPanel = new_panel(dungeonWindow);
    messagePanel = new_panel(messageWindow);
    inputPanel = new_panel(inputWindow);
    playerPanel = new_panel(playerWindow);
    //inventoryPanel = new_panel(inventoryWindow);

    //hide_panel(inventoryPanel);

    for (int y = 0; y < dungeon->height; ++y)
        for (int x = 0; x < dungeon->width; ++x) {
            drawCell({y, x});
        }
    updateDisplay();
}

void ConsoleDungeonIO::disengage() {
    cdbuff.consoleDungeonIO = nullptr;
    endwin();
}

void ConsoleDungeonIO::entityMoved(const std::shared_ptr<Entity> &entity, const Position oldPos) {
    drawCell(oldPos);
    drawEntity(entity);
    updateDisplay();
}

void ConsoleDungeonIO::entityAdded(const std::shared_ptr<Entity> &entity) {
    drawEntity(entity);
    updateDisplay();
}

void ConsoleDungeonIO::entityRemoved(const std::shared_ptr<Entity> &entity) {
    drawCell(entity->getPosition());
    entity->removeListener(shared_from_this());
    updateDisplay();
}

void ConsoleDungeonIO::entityAttacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) {
    ostringstream msg;
    msg << source->getName() << " attacked " << target->getName() << " for " << damage << " damage";
    postMessage(msg.str());
    updateDisplay();
}

void ConsoleDungeonIO::entityMissed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) {
    ostringstream msg;
    msg << source->getName() << " tried to attack " << target->getName();
    postMessage(msg.str());
    updateDisplay();
}

void ConsoleDungeonIO::entityHealed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) {
    ostringstream msg;
    msg << source->getName() << " healed " << target->getName() << " for " << heal << " health";
    postMessage(msg.str());
    updateDisplay();
}

void ConsoleDungeonIO::cellChanged(const Position position) {
    drawCell(position);
    updateDisplay();
}

void ConsoleDungeonIO::drawCell(const Position position) {
    drawCell(position, false);
}

void ConsoleDungeonIO::drawCell(const Position position, const bool highlight) {
    static const chtype wallOut[] = {// a b l r
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
    switch (dungeon->getCellType(position)) {
        case EMPTY: {
            setStandardCell(dungeonWindow, highlight);
            mvwaddch(dungeonWindow, position.y, position.x, ' ');
            break;
        }
        case WALL: {
            bool wallAbove{dungeon->getCellType({position.y - 1, position.x}) == WALL};
            bool wallBelow{dungeon->getCellType({position.y + 1, position.x}) == WALL};
            bool wallLeft{dungeon->getCellType({position.y, position.x - 1}) == WALL};
            bool wallRight{dungeon->getCellType({position.y, position.x + 1}) == WALL};
            setStandardCell(dungeonWindow, highlight);
            int wallIndex{wallAbove | wallBelow << 1 | wallLeft << 2 | wallRight << 3};
            mvwaddch(dungeonWindow, position.y, position.x, wallOut[wallIndex]);
            break;
        }
        case FLOOR: {
            setWalkableCell(dungeonWindow, highlight);
            mvwaddch(dungeonWindow, position.y, position.x, ' ');
            break;
        }
        case HALL: {
            setWalkableCell(dungeonWindow, highlight);
            mvwaddch(dungeonWindow, position.y, position.x, ACS_CKBOARD);
            break;
        }
        case OPEN_DOOR: {
            setWalkableCell(dungeonWindow, highlight);
            mvwaddch(dungeonWindow, position.y, position.x, '+');
            break;
        }
        case CLOSED_DOOR:
            break;
    }
    auto entity = dungeon->getEntityAt(position);
    if (entity) {
        drawEntity(entity, highlight);
    }
}

void ConsoleDungeonIO::drawEntity(const std::shared_ptr<Entity> &entity) {
    drawEntity(entity, false);
}

void ConsoleDungeonIO::drawEntity(const std::shared_ptr<Entity> &entity, const bool highlight) {
    switch (dungeon->getCellType(entity->getPosition())) {
        case EMPTY:
        case WALL:
            setStandardCell(dungeonWindow, highlight);
            break;
        case FLOOR:
        case HALL:
        case OPEN_DOOR:
            setWalkableCell(dungeonWindow, highlight);
            break;
        case CLOSED_DOOR:
            break;
    }
    mvwaddch(dungeonWindow, entity->getPosition().y, entity->getPosition().x, entity->representation);
}

void ConsoleDungeonIO::setWalkableCell(WINDOW *win, bool highlight) {
    wattrset(win, A_BOLD | (highlight ? COLOR_PAIR(HIGHLIGHT_COLOR) : COLOR_PAIR(BASIC_COLOR)) | A_STANDOUT);
}

void ConsoleDungeonIO::setStandardCell(WINDOW *win, bool highlight) {
    wattrset(win, (highlight ? COLOR_PAIR(HIGHLIGHT_COLOR) : COLOR_PAIR(BASIC_COLOR)) | A_STANDOUT);
}

void ConsoleDungeonIO::postMessage(std::string s) {
    waddstr(messageWindow, s.data());
    waddch(messageWindow, '\n');
}

void ConsoleDungeonIO::updateDisplay() {
    update_panels();
    doupdate();
}

void ConsoleDungeonIO::updateHUD(const std::shared_ptr<Character> &character) {
    ostringstream out;
    out << "HP:   " << character->getHealth() << endl;
    out << "Atk:  " << character->getAttackStrength() << endl;
    out << "Def:  " << character->getDefenceStrength() << endl;
    out << "Gold: " << character->currentGold() << endl;
    mvwaddstr(playerWindow, 0, 0, out.str().data());
}

void ConsoleDungeonIO::engageSelection(const std::vector<Position> &range, const Position origin) {
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
        auto &currentRow = rows[row];
        int rowSize{int(currentRow.size())};
        if (rowSize) {
            int rowMinX{currentRow.begin()->x};
            int rowMaxX{currentRow.rbegin()->x};
            if (rowMinX > min.x) {
                for (int dist{1};; ++dist) {
                    if (row + dist < height) {
                        auto &otherRow = rows[row + dist];
                        for (Position p{row + dist, rowMinX - 1}; p.x >= min.x; --p.x) {
                            if (otherRow.find(p) != otherRow.end()) {
                                currentRow.insert(p);
                                break;
                            }
                        }
                        if (int(currentRow.size()) != rowSize) break;
                    }
                    if (row - dist > 0) {
                        auto &otherRow = rows[row - dist];
                        for (Position p{row - dist, rowMinX - 1}; p.x >= min.x; --p.x) {
                            if (otherRow.find(p) != otherRow.end()) {
                                currentRow.insert(p);
                                break;
                            }
                        }
                        if (int(currentRow.size()) != rowSize) break;
                    }
                }
            }
            rowSize = int(currentRow.size());
            if (rowMaxX < max.x) {
                for (int dist{1};; ++dist) {
                    if (row + dist < height) {
                        auto &otherRow = rows[row + dist];
                        for (Position p{row + dist, rowMaxX + 1}; p.x <= max.x; ++p.x) {
                            if (otherRow.find(p) != otherRow.end()) {
                                currentRow.insert(p);
                                break;
                            }
                        }
                        if (int(currentRow.size()) != rowSize) break;
                    }
                    if (row - dist > 0) {
                        auto &otherRow = rows[row - dist];
                        for (Position p{row - dist, rowMaxX + 1}; p.x <= max.x; ++p.x) {
                            if (otherRow.find(p) != otherRow.end()) {
                                currentRow.insert(p);
                                break;
                            }
                        }
                        if (int(currentRow.size()) != rowSize) break;
                    }
                }
            }
        }
    }
    for (int col{0}; col < width; ++col) {
        auto &currentCol = cols[col];
        int colSize{int(currentCol.size())};
        if (colSize) {
            int colMinY{currentCol.begin()->y};
            int colMaxY{currentCol.rbegin()->y};
            if (colMinY > min.y) {
                for (int dist{1};; ++dist) {
                    if (col + dist < width) {
                        auto &otherCol = cols[col + dist];
                        for (Position p{colMinY - 1, col + dist}; p.y >= min.y; --p.y) {
                            if (otherCol.find(p) != otherCol.end()) {
                                currentCol.insert(p);
                                break;
                            }
                        }
                        if (int(currentCol.size()) != colSize) break;
                    }
                    if (col - dist > 0) {
                        auto &otherCol = cols[col - dist];
                        for (Position p{colMinY - 1, col - dist}; p.y >= min.y; --p.y) {
                            if (otherCol.find(p) != otherCol.end()) {
                                currentCol.insert(p);
                                break;
                            }
                        }
                        if (int(currentCol.size()) != colSize) break;
                    }
                }
            }
            colSize = int(currentCol.size());
            if (colMaxY < max.y) {
                for (int dist{1};; ++dist) {
                    if (col + dist < width) {
                        auto &otherCol = cols[col + dist];
                        for (Position p{colMaxY + 1, col + dist}; p.y <= max.y; ++p.y) {
                            if (otherCol.find(p) != otherCol.end()) {
                                currentCol.insert(p);
                                break;
                            }
                        }
                        if (int(currentCol.size()) != colSize) break;
                    }
                    if (col - dist > 0) {
                        auto &otherCol = cols[col - dist];
                        for (Position p{colMaxY + 1, col - dist}; p.y <= max.y; ++p.y) {
                            if (otherCol.find(p) != otherCol.end()) {
                                currentCol.insert(p);
                                break;
                            }
                        }
                        if (int(currentCol.size()) != colSize) break;
                    }
                }
            }
        }
    }

    currentSelection = {-1, -1};
    if (find(range.begin(), range.end(), origin + delta) != range.end()) {
        currentSelection = origin + delta;
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
                currentSelection = origin + delta + eta;
                break;
            }
        }
    }
    if (currentSelection == Position{-1, -1}) currentSelection = range.front();
    drawCell(currentSelection, true);
}

void ConsoleDungeonIO::disengageSelection() {
    drawCell(currentSelection);
}

void ConsoleDungeonIO::selectionUp() {
    auto currentY = find(cols[currentSelection.x].begin(), cols[currentSelection.x].end(), currentSelection);
    if (currentY != cols[currentSelection.x].begin()) {
        --currentY;
        drawCell(currentSelection);
        currentSelection = *currentY;
        drawCell(currentSelection, true);
    }
}

void ConsoleDungeonIO::selectionDown() {
    auto currentY = find(cols[currentSelection.x].begin(), cols[currentSelection.x].end(), currentSelection);
    ++currentY;
    if (currentY != cols[currentSelection.x].end()) {
        drawCell(currentSelection);
        currentSelection = *currentY;
        drawCell(currentSelection, true);
    }
}

void ConsoleDungeonIO::selectionLeft() {
    auto currentX = find(rows[currentSelection.y].begin(), rows[currentSelection.y].end(), currentSelection);
    if (currentX != rows[currentSelection.y].begin()) {
        --currentX;
        drawCell(currentSelection);
        currentSelection = *currentX;
        drawCell(currentSelection, true);
    }
}

void ConsoleDungeonIO::selectionRight() {
    auto currentX = find(rows[currentSelection.y].begin(), rows[currentSelection.y].end(), currentSelection);
    ++currentX;
    if (currentX != rows[currentSelection.y].end()) {
        drawCell(currentSelection);
        currentSelection = *currentX;
        drawCell(currentSelection, true);
    }
}

Position ConsoleDungeonIO::getSelection(const Position origin) {
    delta = currentSelection - origin;
    return currentSelection;
}

void *ConsoleDungeonIO::getOption(std::vector<ITEM *> items) {
    items.push_back(NULL);
    MENU *menu = new_menu(items.data());
    WINDOW *menuWindow = newwin(10, 40, dungeon->height / 2 - 5, dungeon->width / 2 - 20);
    PANEL *menuPanel = new_panel(menuWindow);
    set_menu_win(menu, menuWindow);
    set_menu_sub(menu, derwin(menuWindow, 8, 38, 1, 1));
    set_menu_format(menu, 8, 1);
    set_menu_mark(menu, " * ");
    box(menuWindow, 0, 0);
    post_menu(menu);
    updateDisplay();
    int key;
    void *result = nullptr;
    while (result == nullptr) {
        key = wgetch(menuWindow);
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
        updateDisplay();
    }
    unpost_menu(menu);
    free_menu(menu);
    for(auto item:items) {
        if (item) free_item(item);
    }
    del_panel(menuPanel);
    delwin(menuWindow);
    updateDisplay();
    return nullptr;
}

void ConsoleDungeonIO::showInfo(WINDOW* win, Item *item) {
    ostringstream out;
    out << item->getName() << endl << endl;
    out << "Value: " << item->getValue() << endl;
    if (item->equippable()) {
        out << "Equippable" << endl;
        if (item->equippedSlot() != -1) {
            out << "Equipped to slot: " << (dungeon->getState()->loader->getName(item->equippedSlot())) << endl;
        }
    }
    wclear(win);
    mvwaddstr(win, 0, 0, out.str().data());
    wrefresh(win);
    updateDisplay();
}

std::shared_ptr<Item> ConsoleDungeonIO::selectInventory(const std::shared_ptr<Character> &character) {

    std::vector<ITEM*> items;
    for(auto &item:character->getInventory()) {
        ITEM*newItem = new_item(item->getName().data(), "");
        set_item_userptr(newItem, item.get());
        items.push_back(newItem);
    }
    items.push_back(NULL);
    MENU *menu = new_menu(items.data());
    WINDOW *window = newwin(dungeon->height, dungeon->width - 6, 3, 3);
    PANEL *windowPanel = new_panel(window);
    WINDOW *infoWindow = derwin(window, dungeon->height - 2, (dungeon->width - 8)/2, 1, (dungeon->width - 6)/2);
    set_menu_win(menu, window);
    set_menu_sub(menu, derwin(window, dungeon->height - 2, (dungeon->width - 8)/2 - 1, 1, 1));
    set_menu_format(menu, dungeon->height - 3, 1);
    set_menu_mark(menu, " * ");
    box(window, 0, 0);
    mvwvline(window, 1, (dungeon->width - 6) /2 - 1, ACS_VLINE, dungeon->height - 2);
    post_menu(menu);
    updateDisplay();

    int key;
    Item* result = nullptr;
    bool hasResult = false;
    if(current_item(menu)) {
        showInfo(infoWindow, (Item*)item_userptr(current_item(menu)));
    }
    while (!hasResult) {
        key = getch();
        switch (key) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                showInfo(infoWindow, (Item*)item_userptr(current_item(menu)));
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                showInfo(infoWindow, (Item*)item_userptr(current_item(menu)));
                break;
            case KEY_NPAGE:
                menu_driver(menu, REQ_SCR_DPAGE);
                showInfo(infoWindow, (Item*)item_userptr(current_item(menu)));
                break;
            case KEY_PPAGE:
                menu_driver(menu, REQ_SCR_UPAGE);
                showInfo(infoWindow, (Item*)item_userptr(current_item(menu)));
                break;
            case '\t':
                hasResult = true;
                break;
            case '\n':
                hasResult = true;
                result = (Item*)item_userptr(current_item(menu));
                break;
            default:
                break;
        }
        updateDisplay();
    }
    unpost_menu(menu);
    free_menu(menu);
    for(auto item:items) {
        if (item) free_item(item);
    }
    del_panel(windowPanel);
    delwin(window);
    updateDisplay();
    if (result) {
        return result->shared_from_base<Item>();
    } else {
        return nullptr;
    }
}

void ConsoleDungeonIO::printMode(Action::Type actionType) {
    ostringstream out;
    out << "Mode: ";
    switch (actionType) {
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
    wmove(playerWindow, 4, 0);
    wclrtoeol(playerWindow);
    mvwaddstr(playerWindow, 4, 0, out.str().data());
}

ConsoleDungeonIO::outBuff::outBuff(ConsoleDungeonIO *consoleDungeonIO) : consoleDungeonIO(consoleDungeonIO) {}

int ConsoleDungeonIO::outBuff::sync() {
    if (consoleDungeonIO) {
#ifdef NODISP
        cout << str();
#else
        consoleDungeonIO->postMessage(str());
        consoleDungeonIO->updateDisplay();
#endif
        str("");
    }
    return basic_streambuf::sync();
}

ConsoleDungeonIO::outBuff cdbuff{nullptr};
std::ostream cdout{&cdbuff};

bool ConsoleDungeonIO::SortPositionByX::operator()(const Position &lhs, const Position &rhs) const {
    if (lhs.x < rhs.x)
        return true;
    if (rhs.x < lhs.x)
        return false;
    return lhs.y < rhs.y;
}
