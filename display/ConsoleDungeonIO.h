#pragma once


#include "DungeonRenderer.h"
#include "../controller/Controller.h"
#include "../action/Action.h"

#include <sstream>


typedef struct _win_st WINDOW;
typedef struct panel PANEL;
typedef struct tagITEM ITEM;

class ConsoleDungeonIO: public DungeonRenderer, public Controller {
public:

    // TODO for debug
    class outBuff : public std::stringbuf {
    public:
        outBuff(ConsoleDungeonIO *consoleDungeonIO);

        ConsoleDungeonIO *consoleDungeonIO;

    protected:
        int sync() override;

    };

    ConsoleDungeonIO();

    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;

    void engage() override;

    void disengage() override;

protected:
    void entityMoved(const std::shared_ptr<Entity> &entity, const Position oldPos) override;

    void entityAdded(const std::shared_ptr<Entity> &entity) override;

    void entityRemoved(const std::shared_ptr<Entity> &entity) override;

    void entityAttacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) override;

    void entityMissed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) override;

    void entityHealed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) override;

    void cellChanged(const Position position) override;

private:
    friend class outBuff;

    enum MODE {
        COMMAND,
        INTERACTIVE
    };
    MODE mode {COMMAND};

    const short BASIC_COLOR = 1;
    const short HIGHLIGHT_COLOR = 2;
    void setWalkableCell(WINDOW *win, bool highlight);
    void setStandardCell(WINDOW *win, bool highlight);


    WINDOW *dungeonWindow;
    WINDOW *messageWindow;
    WINDOW *inputWindow;
    WINDOW *playerWindow;
    WINDOW *inventoryWindow;

    PANEL *dungeonPanel;
    PANEL *messagePanel;
    PANEL *inputPanel;
    PANEL *playerPanel;
    PANEL *inventoryPanel;


    //    |-----------------------------------------------------------------------------|
    //    |                                                                             |
    //    | |--------------------------|        |-----------------------|               |
    //    | |.......@..5.......M.......|        |..............9D.......|               |
    //    | |.........3................+########+.......E...............|-------|       |
    //    | |........L.................|   #    |..................O........9D..|--|    |
    //    | |....O............1........|   #    |..................................|--| |
    //    | |----------+---------------|   #    |----+----------------|...............| |
    //    |            #                 #############                |........W......| |
    //    |            #                 #     |-----+------|         |1..............| |
    //    |            #                                              |...............| |
    //    |            #########          dungeonWindow           ####+...............| |
    //    |            #                                              |...............| |
    //    |            #                 #     |-----+------|   #     |--------+------| |
    //    |  |---------+-----------|     #           #          #              #        |
    //    |  |.....\..2........1...|     #           #          #         |----+------| |
    //    |  |.....................|     ########################         |...........| |
    //    |  |.........1.......2...|     #           #                    |0..........| |
    //    |  |...L.................|     #    |------+--------------------|...........| |
    //    |  |.....................|     #    |.................4.....................| |
    //    |  |.....................+##########+..........H.................H..........| |
    //    |  |..7.......6..........|          |................7...................7..| |
    //    |  |---------------------|          |---------------------------------------| |
    //    |                                                                             |
    //    |-----------------------------------------------------------------------------|
    //                                          |
    //                                          |
    //                playerWindow              |          messagesWindow
    //                                          |
    //                                          |
    //                                     inputWindow


    void drawCell(const Position position);
    void drawCell(const Position position, const bool highlight);
    void drawEntity(const std::shared_ptr<Entity> &entity);
    void drawEntity(const std::shared_ptr<Entity> &entity, const bool highlight);
    void postMessage(std::string s);
    void updateHUD(const std::shared_ptr<Character> &character);

    void printMode(Action::Type actionType);
    void* getOption(std::vector<ITEM*> items);
    void showInfo(WINDOW* win, Item *item);
    std::shared_ptr<Item> selectInventory(const std::shared_ptr<Character> &character);

    struct SortPositionByX {
        bool operator()(const Position &lhs, const Position &rhs) const;
    };

    std::vector<std::set<Position, SortPositionByX>> rows;
    std::vector<std::set<Position>> cols;
    Position currentSelection;
    Position delta {0,0};

    void engageSelection(const std::vector<Position> &range, const Position origin);
    void disengageSelection();
    void selectionUp();
    void selectionDown();
    void selectionLeft();
    void selectionRight();
    Position getSelection(const Position origin);

    void updateDisplay();
};

extern ConsoleDungeonIO::outBuff cdbuff;
extern std::ostream cdout;