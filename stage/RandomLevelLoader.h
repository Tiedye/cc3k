#pragma once


#include "Stage.h"

class RandomLevelLoader : public Stage {
public:
    RandomLevelLoader(const std::shared_ptr<State> &state, const std::string &configFile);
    int run(Game &game) override;
private:
    const std::string configFile;

    class SpawnSet {
    public:
        int amount {0};

        void addGroup(int weight, const std::vector<int> &items);
        std::vector<int> getGroup(int selection);
        int getTotalWeight() const;
    private:
        int totalWeight;
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

        std::vector<SpawnSet> spawnSets;
    };

    struct DungeonObject {
        ProbabilitySet probabilitySet;
        int next {-1};
        int id;
        int width{50};
        int height{20};
    };

    void parseProbabilitySet(std::istream &s);
    void parseDungeon(std::istream &s);
    SpawnSet parseSpawnSet(std::istream &s);

    std::map<int, ProbabilitySet> probabilitySets;
    std::map<int, DungeonObject> dungeonObjects;

    std::shared_ptr<Dungeon> genDungeon(const DungeonObject &dungeonObject);

    std::map<int, int> idMapping;
};


