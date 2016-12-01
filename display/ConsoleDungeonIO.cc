#include "ConsoleDungeonIO.h"

#include <curses.h>
#include <panel.h>

#include <sstream>

#include "../stage/Dungeon.h"

using namespace std;

ConsoleDungeonIO::ConsoleDungeonIO() {

}

const Controller::ActionAndTarget
ConsoleDungeonIO::getAction(const std::shared_ptr<Character> &character, const std::vector<Controller::ActionAndRange> &actions, const std::shared_ptr<State> &state) {
    // toggle between two control paths, command mode, and interactive mode
    bool gotAction {false};
    while (!gotAction) {
        if (mode == COMMAND) {

            echo();


        } else if (mode == INTERACTIVE) {

            noecho();


        }
    }

    // TODO all user input during dungeon here
    return ActionAndTarget();
}

void ConsoleDungeonIO::engage() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(BASIC_COLOR, COLOR_BLACK, COLOR_WHITE);

    dungeonWindow = newwin(dungeon->height, dungeon->width, 0, 0);
    //box(dungeonWindow, 0,0);
    messageWindow = newwin(5, dungeon->width/2-1, dungeon->height+1, dungeon->width/2+1);
    //box(messageWindow, 0,0);
    playerWindow = newwin(5, dungeon->width/2, dungeon->height+1, 0);
    //box(playerWindow, 0,0);
    inputWindow = newwin(1, dungeon->width, dungeon->height+6, 0);
    //box(inputWindow, 0,0);
    //inventoryWindow = newwin(dungeon->width - 6, dungeon->height - 6, 3, 3);

    scrollok(messageWindow, true);
    waddstr(messageWindow, "\n\n\n\n");

    dungeonPanel = new_panel(dungeonWindow);
    messagePanel = new_panel(messageWindow);
    inputPanel = new_panel(inputWindow);
    playerPanel = new_panel(playerWindow);
    //inventoryPanel = new_panel(inventoryWindow);

    //hide_panel(inventoryPanel);

    for(int y = 0; y < dungeon->height; ++y) for (int x = 0; x < dungeon->width; ++x) {
            drawCell({y,x});
        }
    updateDisplay();
}

void ConsoleDungeonIO::disengage() {
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
            setStandardCell(dungeonWindow, false);
            mvwaddch(dungeonWindow, position.y, position.x, ' ');
            break;
        }
        case WALL: {
            bool wallAbove{dungeon->getCellType({position.y - 1, position.x}) == WALL};
            bool wallBelow{dungeon->getCellType({position.y + 1, position.x}) == WALL};
            bool wallLeft{dungeon->getCellType({position.y, position.x - 1}) == WALL};
            bool wallRight{dungeon->getCellType({position.y, position.x + 1}) == WALL};
            setStandardCell(dungeonWindow, false);
            int wallIndex{wallAbove | wallBelow << 1 | wallLeft << 2 | wallRight << 3};
            mvwaddch(dungeonWindow, position.y, position.x, wallOut[wallIndex]);
            break;
        }
        case FLOOR: {
            setWalkableCell(dungeonWindow, false);
            mvwaddch(dungeonWindow, position.y, position.x, ' ');
            break;
        }
        case HALL: {
            setWalkableCell(dungeonWindow, false);
            mvwaddch(dungeonWindow, position.y, position.x, ACS_CKBOARD);
            break;
        }
        case OPEN_DOOR: {
            setWalkableCell(dungeonWindow, false);
            mvwaddch(dungeonWindow, position.y, position.x, '+');
            break;
        }
        case CLOSED_DOOR:break;
    }
    auto entity = dungeon->getEntityAt(position);
    if (entity) {
        drawEntity(entity);
    }
}

void ConsoleDungeonIO::drawEntity(const std::shared_ptr<Entity> &entity) {
    switch (dungeon->getCellType(entity->getPosition())) {
        case EMPTY:
        case WALL:
            setStandardCell(dungeonWindow, false);
            break;
        case FLOOR:
        case HALL:
        case OPEN_DOOR:
            setWalkableCell(dungeonWindow, false);
            break;
        case CLOSED_DOOR:break;
    }
    mvwaddch(dungeonWindow, entity->getPosition().y, entity->getPosition().x, entity->representation);
}

void ConsoleDungeonIO::setWalkableCell(WINDOW *win, bool highlight) {
    wattrset(win, A_BOLD|(highlight ? 0: A_STANDOUT)|COLOR_PAIR(BASIC_COLOR));
}

void ConsoleDungeonIO::setStandardCell(WINDOW *win, bool highlight) {
    wattrset(win, (highlight ? 0: A_STANDOUT)|COLOR_PAIR(BASIC_COLOR));
}

void ConsoleDungeonIO::postMessage(std::string s) {
    waddch(messageWindow, '\n');
    waddstr(messageWindow, s.data());
}
void ConsoleDungeonIO::updateDisplay() {
    update_panels();
    doupdate();
}
