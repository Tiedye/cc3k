#include "HasAIData.h"

using namespace std;

std::vector<std::shared_ptr<Entity>> &HasAIData::aiStartEntityList(int id) {
    auto result = entityListData.emplace(id, vector<shared_ptr<Entity>>());
    return result.first->second;
}

std::vector<int> &HasAIData::aiStartIntegerList(int id) {
    auto result = intListData.emplace(id, vector<int>());
    return result.first->second;
}

std::vector<float> &HasAIData::aiStartFloatList(int id) {
    auto result = floatListData.emplace(id, vector<float>());
    return result.first->second;
}

std::vector<double> &HasAIData::aiStartDoubleList(int id) {
    auto result = doubleListData.emplace(id, vector<double >());
    return result.first->second;
}

std::vector<Position> &HasAIData::aiStartPositionList(int id) {
    auto result = positionListData.emplace(id, vector<Position>());
    return result.first->second;
}

std::vector<std::shared_ptr<Entity>> &HasAIData::aiGetEntityList(int id) {
    return entityListData.at(id);
}

std::vector<int> &HasAIData::aiGetIntegerList(int id) {
    return intListData.at(id);
}

//std::vector<float> &HasAIData::aiGetFloatList(int id) {
//    return floatListData.at(id);
//}
//
//std::vector<double> &HasAIData::aiGetDoubleList(int id) {
//    return doubleListData.at(id);
//}

std::vector<Position> &HasAIData::aiGetPositionList(int id) {
    return positionListData.at(id);
}

std::shared_ptr<Entity> &HasAIData::aiGetEntity(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto addResult = data.emplace(id, Data());
        return addResult.first->second.entity;
    } else {
        return result->second.entity;
    }
}

int &HasAIData::aiGetInteger(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto addResult = data.emplace(id, Data());
        return addResult.first->second.integerValue;
    } else {
        return result->second.integerValue;
    }
}

//float &HasAIData::aiGetFloat(int id) {
//    auto result = data.find(id);
//    if (result == data.end()) {
//        auto addResult = data.emplace(id, {});
//        return addResult.first->second.floatValue;
//    } else {
//        return result->second.floatValue;
//    }
//}
//
//double &HasAIData::aiGetDouble(int id) {
//    auto result = data.find(id);
//    if (result == data.end()) {
//        auto addResult = data.emplace(id, {});
//        return addResult.first->second.doubleValue;
//    } else {
//        return result->second.doubleValue;
//    }
//}

Position &HasAIData::aiGetPosition(int id) {
    auto result = data.find(id);
    if (result == data.end()) {
        auto addResult = data.emplace(id, Data());
        return addResult.first->second.position;
    } else {
        return result->second.position;
    }
}

int HasAIData::aiReserveId() {
    return freeId++;
}

int HasAIData::aiReserveId(std::string name) {
    if (nameIds.count(name)) {
        return -1;
    } else {
        int id {freeId++};
        nameIds[name] = id;
        return id;
    }
}

int HasAIData::aiGetId(std::string name) {
    return nameIds.at(name);
}

int HasAIData::aiReservedId(std::string name) {
    return nameIds.count(name) != 0;
}

HasAIData::~HasAIData() {

}

HasAIData::Data::Data() : entity{nullptr} {}

int HasAIData::freeId {0};
std::map<std::string, int> HasAIData::nameIds;