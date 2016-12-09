#include "Dungeon.h"

#include <queue>
#include <algorithm>

#include "../Game.h"
#include "../display/ConsoleDungeonIO.h"


#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

std::list<std::shared_ptr<Entity>> Dungeon::get_entities_at(Position position) {
    return cell_entities[position.x+position.y*width];
}

std::list<std::shared_ptr<Entity>> Dungeon::get_entities_at(Position position, const shared_ptr<Entity> &exclude) {
    auto entity_list = cell_entities[position.x+position.y*width];
    auto found = find(entity_list.begin(), entity_list.end(), exclude);
    if (found != entity_list.end()) entity_list.erase(found);
    return entity_list;
}

int Dungeon::run(Game &game) {
    is_running = true;
    next = default_next;
    state->current_dungeon = shared_from_this();
    state->player->move(spawn_point);
    initialize_entity(state->player);
    state->dungeon_renderer->set_dungeon(shared_from_this());
    state->dungeon_renderer->engage();
    int turn_id {0};
    while (is_running){
        if (state->player->is_dead()) break;
        int current_init {0};
        for (auto entity_iter = entities.begin(); entity_iter != entities.end();) {
            auto entity = *entity_iter;

            entity->start_tracking();
            current_init = entity->get_initiative();

            entity->do_turn(*this, turn_id);

            // handle if iterator invalidated
            if (entity->iterator_invalid()) {
                entity_iter = find_if(entities.begin(), entities.end(),
                                     [turn_id, current_init](const shared_ptr<Entity> searching) {
                                         return searching->last_turn_id() != turn_id &&
                                                searching->get_initiative() >= current_init;
                                     });
            } else {
                ++entity_iter;
            }

            if(!is_running) break;
            //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        ++turn_id;
    }
    state->dungeon_renderer->disengage();
    state->player->remove_from_containers();
    return next;
}

CellType Dungeon::get_cell_type(const Position position) {
    if (on_feild(position)) {
        return cells[position.x + position.y * width];
    } else {
        return EMPTY;
    }
}

void Dungeon::set_cell_type(const Position position, const CellType type) {
    if (on_feild(position)) {
        cells[position.x + position.y * width] = type;
        state->dungeon_renderer->change_cell(position);
    }
}

void Dungeon::initialize_cell(const Position position, const CellType type) {
    if (on_feild(position)) {
        cells[position.x + position.y * width] = type;
    }
}

Dungeon::Dungeon(const shared_ptr<State> &state, int id, int width, int height) : Stage(state, id), width{width}, height{height}, size{width * height}, cells(size), cell_entities(size), range_tracker(size, -1) {}

bool compare_initiative(const shared_ptr<Entity> &a, const shared_ptr<Entity> &b) {
    return a->get_initiative() > b->get_initiative();
}

void Dungeon::add_entity(shared_ptr<Entity> entity) {
    entity->add_listener(shared_from_this());
    entity->add_listener(state->dungeon_renderer);
    entity->trigger(ADDED_TO_FLOOR);
    auto pos = entities.insert(upper_bound(entities.begin(), entities.end(), entity, compare_initiative), entity);
    entity->add_list_reference(entities, pos);
    list<shared_ptr<Entity>> &cell_list = get_cell_list_at(entity->get_position());
    cell_list.push_front(entity);
    entity->add_list_reference(cell_list, cell_list.begin());
    entity->on_floor = true;
    entity->trigger(ADDED_TO_FLOOR_DONE);
}

void Dungeon::initialize_entity(std::shared_ptr<Entity> entity) {
    entity->add_listener(shared_from_this());
    entity->add_listener(state->dungeon_renderer);
    entities.push_front(entity);
    entity->add_list_reference(entities, entities.begin());
    list<shared_ptr<Entity>> &cell_list = get_cell_list_at(entity->get_position());
    cell_list.push_front(entity);
    entity->add_list_reference(cell_list, cell_list.begin());
    entity->on_floor = true;
}


std::list<std::shared_ptr<Entity>> &Dungeon::get_cell_list_at(Position position) {
    return cell_entities[position.x+position.y*width];
}

std::shared_ptr<Entity> Dungeon::get_entity_at(const Position position) {
    shared_ptr<Entity> largest;
    if(!on_feild(position)) return largest;
    for(auto entity:get_entities_at(position)) {
        if (largest) {
            if (entity->get_size() > largest->get_size()) largest = entity;
        } else {
            largest = entity;
        }
    }
    return largest;
}

std::vector<Position>
Dungeon::get_targetable(const Character &source, const std::shared_ptr<Action> &action) {
    vector<Position> targets;
    Action::Range range {action->range};
    int min_range {action->get_min_range(source)};
    int max_range {action->get_range(source)};
    Position position {source.get_position()};
    switch (range) {
        case Action::ANY:
            for(int x {-max_range}; x <= max_range; ++x) {
                for(int y {-max_range}; y <= max_range; ++y) {
                    int dist {x < y ? x < -y ? x : y : x < -y ? y : x}; // LOGIC!
                    if (dist < 0) dist = -dist;
                    if (dist >= min_range) {
                        Position new_target {y+position.y, x+position.x};
                        if (on_feild(new_target) && action->can_target(new_target, state)) targets.push_back(new_target);
                    } else {
                        y += (min_range - 1) * 2; // LOGIC!
                    }
                }
            }
            break;
        case Action::LOS:
            // TODO ViewField implementation
            break;
        case Action::PATH: {
            reset_range();
            queue<Position> to_fill;
            to_fill.push(position);
            at_range(position) = 0;
            while (!to_fill.empty()) {
                Position current_position = to_fill.front();
                to_fill.pop();
                if (at_range(current_position) >= min_range) {
                    if (on_feild(current_position) && action->can_target(current_position, state)) targets.push_back(current_position);
                }
                if (at_range(current_position) == max_range) {
                    continue;
                }
                Position positions[8]{
                        {current_position.y + 1, current_position.x + 1},
                        {current_position.y + 1, current_position.x},
                        {current_position.y + 1, current_position.x - 1},
                        {current_position.y,     current_position.x + 1},
                        {current_position.y,     current_position.x - 1},
                        {current_position.y - 1, current_position.x + 1},
                        {current_position.y - 1, current_position.x},
                        {current_position.y - 1, current_position.x - 1}
                };
                for (Position &pos:positions) {
                    if (on_feild(pos) && at_range(pos) == -1 && (get_cell_type(pos) != WALL && get_cell_type(pos) != EMPTY)) {
                        at_range(pos) = at_range(current_position) + 1;
                        to_fill.push(pos);
                    }
                }
            }
            break;
        }
        case Action::STRICT_PATH: {
            reset_range();
            queue<Position> to_fill;
            to_fill.push(position);
            at_range(position) = 0;
            while (!to_fill.empty()) {
                Position current_position = to_fill.front();
                to_fill.pop();
                if (at_range(current_position) >= min_range) {
                    if (on_feild(current_position) && action->can_target(current_position, state)) targets.push_back(current_position);
                }
                if (at_range(current_position) == max_range) {
                    continue;
                }
                Position positions[8]{
                        {current_position.y + 1, current_position.x + 1},
                        {current_position.y + 1, current_position.x},
                        {current_position.y + 1, current_position.x - 1},
                        {current_position.y,     current_position.x + 1},
                        {current_position.y,     current_position.x - 1},
                        {current_position.y - 1, current_position.x + 1},
                        {current_position.y - 1, current_position.x},
                        {current_position.y - 1, current_position.x - 1}
                };
                for (Position &pos:positions) {
                    auto entity_at = get_entity_at(pos);
                    if (on_feild(pos) && at_range(pos) == -1 && (get_cell_type(pos) != WALL && get_cell_type(pos) != EMPTY) &&
                        (!entity_at || entity_at->get_size() == MINISCULE)) {
                        at_range(pos) = at_range(current_position) + 1;
                        to_fill.push(pos);
                    }
                }
            }
            break;
        }
    }

    return targets;
}


bool Dungeon::on_feild(const Position position) {
    return position.y >= 0 && position.y < height && position.x >= 0 && position.x < width;
}

bool Dungeon::on_feild(int x, int y) {
    return y >= 0 && y < height && x >= 0 && x < width;
}

int &Dungeon::at_range(const Position position) {
    return range_tracker[position.x + position.y*width];
}

int &Dungeon::at_range(int x, int y) {
    return range_tracker[x + y*width];
}

void Dungeon::reset_range() {
    fill(range_tracker.begin(), range_tracker.end(), -1);
}

const shared_ptr<State> Dungeon::get_state() {
    return state;
}

std::vector<std::shared_ptr<Entity>> Dungeon::get_targeted(const shared_ptr<Entity> &from, Position to,
                                                          const shared_ptr<Action> &action) {
    vector<shared_ptr<Entity>> targeted;
    for (Position target:get_targeted_area(from->get_position(), to, action)){
        if (action->aoe) {
            list<shared_ptr<Entity>> at_target;
            if (action->targets & Action::SELF) {
                at_target = get_entities_at(target);
            } else {
                at_target = get_entities_at(target, from);
            }
            targeted.insert(targeted.end(), at_target.begin(), at_target.end());
        } else {
            auto at_target = get_entity_at(target);
            if (at_target && (at_target != from || action->targets & Action::SELF)) {
                targeted.push_back(at_target);
            }
        }
    }
    return targeted;
}

std::vector<Position> Dungeon::get_targeted_area(const Position from, const Position to, const std::shared_ptr<Action> &action) {
    vector<Position> targeted;
    switch (action->shape) {
        case Action::POINT: {
            targeted.push_back(to);
            break;
        }
        case Action::LINE: {
            Position delta = to - from;
            if (delta.x == 0) { // handle vertical case
                if (delta.y > 0) {
                    for (int y = from.y; y <= to.y; ++y) {
                        targeted.emplace_back(y, from.x);
                    }
                } else {
                    for (int y = from.y; y >= to.y; --y) {
                        targeted.emplace_back(y, from.x);
                    }
                }
            } else { // other cases TODO: test this
                float slope = delta.y;
                slope /= delta.x;
                float so_far{0.5};
                int x_dir{delta.x > 0 ? 1 : -1};
                int y_dir{delta.y > 0 ? 1 : -1};
                Position current{from};
                targeted.push_back(current);
                do {
                    if (so_far < 1 && so_far > 0) {
                        so_far += slope;
                        current.shift_x(x_dir);
                    } else if (so_far < 2 && so_far > -1) {
                        so_far += slope - 1;
                        current.shift_x(x_dir);
                        current.shift_y(y_dir);
                    } else {
                        so_far -= 1;
                        current.shift_y(y_dir);
                    }
                    targeted.push_back(current);
                } while (current != to);
            }
            break;
        }
        case Action::SQUARE: {
            int range{action->aoe_size};
            for (int x{-range}; x <= range; ++x) {
                for (int y{-range}; y <= range; ++y) {
                    targeted.emplace_back(y + to.y, x + to.x);
                }
            }
            break;
        }
    }
    return targeted;
}

std::vector<Position> Dungeon::path(const Position from, const Position to, const std::shared_ptr<Action> &action) {
    reset_range();
    queue<Position> to_fill;
    to_fill.push(from);
    at_range(from) = 0;
    while (!to_fill.empty()) {
        Position current_position = to_fill.front();
        to_fill.pop();
        if (current_position == to) {
            break;
        }
        Position positions[8]{
                {current_position.y + 1, current_position.x + 1},
                {current_position.y + 1, current_position.x},
                {current_position.y + 1, current_position.x - 1},
                {current_position.y,     current_position.x + 1},
                {current_position.y,     current_position.x - 1},
                {current_position.y - 1, current_position.x + 1},
                {current_position.y - 1, current_position.x},
                {current_position.y - 1, current_position.x - 1}
        };
        for (Position &pos:positions) {
            auto entity_at = get_entity_at(pos);
            if (on_feild(pos) && at_range(pos) == -1 && (get_cell_type(pos) != WALL && get_cell_type(pos) != EMPTY) &&
                (!entity_at || entity_at->get_size() == MINISCULE)) {
                at_range(pos) = at_range(current_position) + 1;
                to_fill.push(pos);
            }
        }
    }
    if (at_range(to)) {
        vector<Position> path(at_range(to)+1);
        Position current_position = to;
        for (int d = at_range(to); d >= 0; --d) {
            path[d] = to;
            Position positions[8]{
                    {current_position.y + 1, current_position.x + 1},
                    {current_position.y + 1, current_position.x},
                    {current_position.y + 1, current_position.x - 1},
                    {current_position.y,     current_position.x + 1},
                    {current_position.y,     current_position.x - 1},
                    {current_position.y - 1, current_position.x + 1},
                    {current_position.y - 1, current_position.x},
                    {current_position.y - 1, current_position.x - 1}
            };
            for (Position &pos:positions) {
                if (at_range(pos) == d-1) {
                    current_position = pos;
                    break;
                }
            }
        }
        return path;
    } else {
        return {};
    }
}

void Dungeon::finish(int next) {
    is_running = false;
    Dungeon::next = next;
}

const string &Dungeon::get_name() const {
    return name;
}

void Dungeon::set_name(const string &name) {
    Dungeon::name = name;
}


void Dungeon::notify(EventInfo &info) {
    auto entity = info.primary->as_entity();
    auto &old_cell_list = get_cell_list_at(info.event_position);
    auto result = find(old_cell_list.begin(), old_cell_list.end(), entity);
    if(result != old_cell_list.end()) {
        old_cell_list.erase(result);
        entity->remove_list_reference(old_cell_list);
        auto &new_cell_list = get_cell_list_at(entity->get_position());
        new_cell_list.push_front(entity);
        entity->add_list_reference(new_cell_list, new_cell_list.begin());
    }
}

const std::vector<EventType> Dungeon::listening_for() const {
    return {MOVE_DONE};
}