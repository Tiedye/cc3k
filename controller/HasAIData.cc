#include "HasAIData.h"

using namespace std;

std::vector<std::shared_ptr<Entity>> &HasAIData::ai_start_entity_list(int id) {
    auto result = entity_list_data.emplace(id, vector<shared_ptr<Entity>>());
    return result.first->second;
}

std::vector<int> &HasAIData::ai_start_integer_list(int id) {
    auto result = int_list_data.emplace(id, vector<int>());
    return result.first->second;
}

std::vector<float> &HasAIData::ai_start_float_list(int id) {
    auto result = float_list_data.emplace(id, vector<float>());
    return result.first->second;
}

std::vector<double> &HasAIData::ai_start_double_list(int id) {
    auto result = double_list_data.emplace(id, vector<double >());
    return result.first->second;
}

std::vector<Position> &HasAIData::ai_start_position_list(int id) {
    auto result = position_list_data.emplace(id, vector<Position>());
    return result.first->second;
}

std::vector<std::shared_ptr<Entity>> &HasAIData::ai_get_entity_list(int id) {
    return entity_list_data.at(id);
}

std::vector<int> &HasAIData::ai_get_integer_list(int id) {
    return int_list_data.at(id);
}

//std::vector<float> &HasAIData::ai_get_float_list(int id) {
//    return float_list_data.at(id);
//}
//
//std::vector<double> &HasAIData::ai_get_double_list(int id) {
//    return double_list_data.at(id);
//}

std::vector<Position> &HasAIData::ai_get_position_list(int id) {
    return position_list_data.at(id);
}

std::shared_ptr<Entity> &HasAIData::ai_get_entity(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto add_result = data.emplace(id, Data());
        return add_result.first->second.entity;
    } else {
        return result->second.entity;
    }
}

int &HasAIData::ai_get_integer(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto add_result = data.emplace(id, Data());
        return add_result.first->second.integer_value;
    } else {
        return result->second.integer_value;
    }
}

//float &HasAIData::ai_get_float(int id) {
//    auto result = data.find(id);
//    if (result == data.end()) {
//        auto add_result = data.emplace(id, {});
//        return add_result.first->second.float_value;
//    } else {
//        return result->second.float_value;
//    }
//}
//
//double &HasAIData::ai_get_double(int id) {
//    auto result = data.find(id);
//    if (result == data.end()) {
//        auto add_result = data.emplace(id, {});
//        return add_result.first->second.double_value;
//    } else {
//        return result->second.double_value;
//    }
//}

Position &HasAIData::ai_get_position(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto add_result = data.emplace(id, Data());
        return add_result.first->second.position;
    } else {
        return result->second.position;
    }
}

int HasAIData::ai_reserve_id() {
    return free_id++;
}

int HasAIData::ai_reserve_id(std::string name) {
    if (name_ids.count(name)) {
        return -1;
    } else {
        int id {free_id++};
        name_ids[name] = id;
        return id;
    }
}

int HasAIData::ai_get_id(std::string name) {
    return name_ids.at(name);
}

int HasAIData::ai_reserved_id(std::string name) {
    return name_ids.count(name) != 0;
}

HasAIData::~HasAIData() {

}

HasAIData::Data::Data() : entity{nullptr}, integer_value{0}, position{-1,-1} {}

int HasAIData::free_id {0};
std::map<std::string, int> HasAIData::name_ids;