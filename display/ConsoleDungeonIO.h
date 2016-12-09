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
    class out_buff : public std::stringbuf {
    public:
        out_buff(ConsoleDungeonIO *console_dungeon_i_o);

        ConsoleDungeonIO *console_dungeon_i_o;

    protected:
        int sync() override;

    };

    ConsoleDungeonIO();

    const ActionAndTarget get_action(const std::shared_ptr<Character> &character, const std::vector<ActionAndRange> &actions, const std::shared_ptr<State> &state) override;

    void engage() override;

    void disengage() override;

protected:
    void entity_moved(const std::shared_ptr<Entity> &entity, const Position old_pos) override;

    void entity_added(const std::shared_ptr<Entity> &entity) override;

    void entity_removed(const std::shared_ptr<Entity> &entity) override;

    void entity_attacked(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int damage) override;

    void entity_missed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target) override;

    void entity_healed(const std::shared_ptr<Character> &source, const std::shared_ptr<Entity> &target, const int heal) override;

    void cell_changed(const Position position) override;

private:
    friend class out_buff;

    enum MODE {
        COMMAND,
        INTERACTIVE
    };
    MODE mode {COMMAND};

    const short BASIC_COLOR = 1;
    const short HIGHLIGHT_COLOR = 2;
    void set_walkable_cell(WINDOW *win, bool highlight);
    void set_standard_cell(WINDOW *win, bool highlight);


    WINDOW *dungeon_window;
    WINDOW *message_window;
    WINDOW *input_window;
    WINDOW *player_window;
    WINDOW *inventory_window;

    PANEL *dungeon_panel;
    PANEL *message_panel;
    PANEL *input_panel;
    PANEL *player_panel;
    PANEL *inventory_panel;


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
    //    |            #########          dungeon_window           ####+...............| |
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
    //                player_window              |          messages_window
    //                                          |
    //                                          |
    //                                     input_window


    void draw_cell(const Position position);
    void draw_cell(const Position position, const bool highlight);
    void draw_entity(const std::shared_ptr<Entity> &entity);
    void draw_entity(const std::shared_ptr<Entity> &entity, const bool highlight);
    void post_message(std::string s);
    void update_h_u_d(const std::shared_ptr<Character> &character);

    void print_mode(Action::Type action_type);
    void* get_option(std::vector<ITEM*> items);
    void show_info(WINDOW* win, Item *item);
    std::shared_ptr<Item> select_inventory(const std::shared_ptr<Character> &character);

    struct SortPositionByX {
        bool operator()(const Position &lhs, const Position &rhs) const;
    };

    std::vector<std::set<Position, SortPositionByX>> rows;
    std::vector<std::set<Position>> cols;
    Position current_selection;
    Position delta {0,0};

    void engage_selection(const std::vector<Position> &range, const Position origin);
    void disengage_selection();
    void selection_up();
    void selection_down();
    void selection_left();
    void selection_right();
    Position get_selection(const Position origin);

    void update_display();
};

extern ConsoleDungeonIO::out_buff cdbuff;
extern std::ostream cdout;