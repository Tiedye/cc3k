#pragma once

#include <map>
#include <memory>
#include <vector>
#include <string>
#include "../util/Position.h"

class Entity;

class HasAIData {
public:
    virtual ~HasAIData();

    std::vector<std::shared_ptr<Entity>> &ai_start_entity_list(int id);
    std::vector<int> &ai_start_integer_list(int id);
    std::vector<float> &ai_start_float_list(int id);
    std::vector<double> &ai_start_double_list(int id);
    std::vector<Position> &ai_start_position_list(int id);

    std::vector<std::shared_ptr<Entity>> &ai_get_entity_list(int id);
    std::vector<int> &ai_get_integer_list(int id);
//    std::vector<float> &ai_get_float_list(int id);
//    std::vector<double> &ai_get_double_list(int id);
    std::vector<Position> &ai_get_position_list(int id);

    std::shared_ptr<Entity> &ai_get_entity(int id);
    int &ai_get_integer(int id);
//    float &ai_get_float(int id);
//    double &ai_get_double(int id);
    Position &ai_get_position(int id);

    static int ai_reserve_id();
    static int ai_reserve_id(std::string name);
    static int ai_get_id(std::string name);
    static int ai_reserved_id(std::string name);
private:
    static int free_id;
    static std::map<std::string, int> name_ids;

    struct Data {
        Data();
        std::shared_ptr<Entity> entity;
        int integer_value;
        //float float_value;
        //double double_value;
        Position position;
    };

    std::map<int, Data> data;
    std::map<int, std::vector<std::shared_ptr<Entity>>> entity_list_data;
    std::map<int, std::vector<int>> int_list_data;
    std::map<int, std::vector<float>> float_list_data;
    std::map<int, std::vector<double>> double_list_data;
    std::map<int, std::vector<Position>> position_list_data;

};

