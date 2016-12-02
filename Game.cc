#include "Game.h"

#include <iostream>

using namespace std;

void Game::setInitialStage(int stage) {
    initialStage = stage;
}

void Game::addStage(int group, const std::shared_ptr<Stage> &stage) {
    auto result = stages.emplace(stage->id, stage);
    groups[group].push_back(result.first);
}

void Game::removeState(int id) {
    stages.erase(id);
}

void Game::removeStates(int group) {
    for(auto it:groups[group]) {
        stages.erase(it);
    }
    groups.erase(group);
}

//void Game::addEntityClass(int id, std::shared_ptr<Entity> &entity) {
//
//}

//State &Game::getState() {
//    return state;
//}

void Game::start() {
    for(int id {initialStage}; id >= 0; id = stages[id]->run(*this)){}
}

Game::Game(const std::shared_ptr<State> &state) : state(state) {}

int Game::getId() {
    return ids++;
}

int Game::nextId() {
    return ids;
}

int Game::ids {0};
