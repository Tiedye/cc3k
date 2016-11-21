#include "Game.h"

void Game::setInitialStage(int stage) {
    currentStage = stages.at(stage);
}

void Game::addStage(int group, Stage *stage) {

}

void Game::removeState(int id) {

}

void Game::removeStates(int group) {

}

void Game::addEntityClass(int id, Entity *entity) {

}

State &Game::getState() {
    return state;
}

void Game::start() {

}
