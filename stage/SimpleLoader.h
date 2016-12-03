#pragma once

#include "Stage.h"
#include "Dungeon.h"

#include <vector>
#include <string>

class SimpleLoader : public Stage {
public:
    SimpleLoader(const std::shared_ptr<State> &state, const std::vector<std::string> &files);

private:
    int run(Game &game) override;

    const std::vector<std::string> files;

    std::map<char, int>  parseMapping(std::istream& s);
    std::shared_ptr<Dungeon> parseDungeon(const std::map<char, int> &mapping, std::istream &s, int id);

};


