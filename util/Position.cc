#include "Position.h"
Position::Position(int row, int col) : y(row), x(col) {}
Position::Position() {}


Position Position::operator+(const Position &other) const {
    return {y + other.y, x + other.x};
}
Position Position::operator-(const Position &other) const {
    return {y - other.y, x - other.x};
}

bool Position::operator==(const Position &other) const {
    return x == other.x && y == other.y;
}

bool Position::operator!=(const Position &other) const {
    return x != other.x || y != other.y;;
}

void Position::shift_x(int amount) {
    x += amount;
}

void Position::shift_y(int amount) {
    y += amount;
}

std::ostream &operator<<(std::ostream &os, const Position &position)  {
    os << "{y: " << position.y << " x: " << position.x << "}";
    return os;
}

bool operator<(const Position &lhs, const Position &rhs) {
    if (lhs.y < rhs.y)
        return true;
    return rhs.y >= lhs.y && lhs.x < rhs.x;
}
