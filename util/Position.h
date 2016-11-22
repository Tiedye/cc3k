#pragma once


struct Position {
    int row;
    int col;
    Position();
    Position(int row, int col);
    Position(const Position &other) = default;
    Position(Position &&other) = default;
    Position &operator=(const Position &other) = default;
    Position &operator=(Position &&other) = default;
    Position operator+(const Position &other);
};


