#pragma once


#include "Stage.h"

class RandomLevelLoader : public Stage {
public:
    RandomLevelLoader(const std::shared_ptr<State> &state, const std::string &config_file);
    int run(Game &game) override;
private:
    const std::string config_file;

    class SpawnSet {
    public:
        SpawnSet();
        int amount {0};

        void add_group(int weight, const std::vector<int> &items);
        std::vector<int> get_group(int selection);
        int get_total_weight() const;
    private:
        int total_weight {0};
        std::multimap<int, std::vector<int>> groups;
    };

    struct ProbabilitySet {
        ProbabilitySet();

        int chm = {5};
        int chs = {1};
        int cwm = {11};
        int cws = {4};

        int ccm = {6};
        int ccs = {6};

        int vhlm = {10};
        int vhls = {10};
        int hhlm = {30};
        int hhls = {10};

        std::vector<SpawnSet> spawn_sets;
    };

    struct DungeonObject {
        DungeonObject();
        ProbabilitySet probability_set;
        int next {-1};
        int id;
        int width{50};
        int height{20};
    };

    void parse_probability_set(std::istream &s);
    void parse_dungeon(std::istream &s);
    SpawnSet parse_spawn_set(std::istream &s);

    std::map<int, ProbabilitySet> probability_sets;
    std::map<int, DungeonObject> dungeon_objects;

    std::shared_ptr<Dungeon> gen_dungeon(const DungeonObject &dungeon_object);

    std::map<int, int> id_mapping;
};


