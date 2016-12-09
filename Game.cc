#include "Game.h"

#include <iostream>

using namespace std;

void Game::set_initial_stage(int stage) {
    initial_stage = stage;
}

void Game::add_stage(int group, const std::shared_ptr<Stage> &stage) {
    auto result = stages.emplace(stage->id, stage);
    groups[group].push_back(result.first);
}

void Game::remove_state(int id) {
    stages.erase(id);
}

void Game::remove_states(int group) {
    for(auto it:groups[group]) {
        stages.erase(it);
    }
    groups.erase(group);
}

//void Game::add_entity_class(int id, std::shared_ptr<Entity> &entity) {
//
//}

//State &Game::get_state() {
//    return state;
//}

void Game::start() {
    for(int id {initial_stage}; id >= 0; id = stages[id]->run(*this)){}
}

Game::Game(const std::shared_ptr<State> &state) : state(state) {}

int Game::get_id() {
    return ids++;
}

int Game::next_id() {
    return ids;
}

int Game::ids {0};
