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

    std::vector<std::shared_ptr<Entity>> &aiStartEntityList(int id);
    std::vector<int> &aiStartIntegerList(int id);
    std::vector<float> &aiStartFloatList(int id);
    std::vector<double> &aiStartDoubleList(int id);
    std::vector<Position> &aiStartPositionList(int id);

    std::vector<std::shared_ptr<Entity>> &aiGetEntityList(int id);
    std::vector<int> &aiGetIntegerList(int id);
//    std::vector<float> &aiGetFloatList(int id);
//    std::vector<double> &aiGetDoubleList(int id);
    std::vector<Position> &aiGetPositionList(int id);

    std::shared_ptr<Entity> &aiGetEntity(int id);
    int &aiGetInteger(int id);
//    float &aiGetFloat(int id);
//    double &aiGetDouble(int id);
    Position &aiGetPosition(int id);

    static int aiReserveId();
    static int aiReserveId(std::string name);
    static int aiGetId(std::string name);
    static int aiReservedId(std::string name);
private:
    static int freeId;
    static std::map<std::string, int> nameIds;

    struct Data {
        Data();
        std::shared_ptr<Entity> entity;
        int integerValue;
        //float floatValue;
        //double doubleValue;
        Position position;
    };

    std::map<int, Data> data;
    std::map<int, std::vector<std::shared_ptr<Entity>>> entityListData;
    std::map<int, std::vector<int>> intListData;
    std::map<int, std::vector<float>> floatListData;
    std::map<int, std::vector<double>> doubleListData;
    std::map<int, std::vector<Position>> positionListData;

};

