#pragma once


#include "DungeonRenderer.h"
#include "../controller/Controller.h"

typedef struct _win_st WINDOW;
typedef struct panel PANEL;

class ConsoleDungeonIO: public DungeonRenderer, public Controller {
public:
    ConsoleDungeonIO();

    const ActionAndTarget getAction(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;

    void engage() override;

    void disengage() override;

protected:
    void entityMoved(const std::shared_ptr<Entity> &entity, const Position oldPos) override;

    void entityAdded(const std::shared_ptr<Entity> &entity) override;

    void entityRemoved(const std::shared_ptr<Entity> &entity) override;

    void entityAttacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) override;

    void entityHealed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) override;

    void cellChanged(const Position position) override;

private:
    enum MODE {
        COMMAND,
        INTERACTIVE
    };
    MODE mode {COMMAND};

    const short BASIC_COLOR = 1;
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
    void drawEntity(const std::shared_ptr<Entity> &entity);
    void postMessage(std::string s);

    void updateDisplay();
};



