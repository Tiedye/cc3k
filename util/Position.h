#pragma once


#include <ostream>

struct Position {
    int y;
    int x;
    Position();
    Position(int row, int col);
    Position(const Position &other) = default;
    Position(Position &&other) = default;
    Position &operator=(const Position &other) = default;
    Position &operator=(Position &&other) = default;

    friend bool operator<(const Position &lhs, const Position &rhs);
    Position operator+(const Position &other) const;
    Position operator-(const Position &other) const;
    bool operator==(const Position &other) const;
    bool operator!=(const Position &other) const;

    void shift_x(int amount);
    void shift_y(int amount);

    friend std::ostream &operator<<(std::ostream &os, const Position &position);
};

std::ostream &operator<<(std::ostream &os, const Position &position);
