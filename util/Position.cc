
#include "Position.h"

Position Position::operator+(const Position &other) {
    return {row + other.row, col + other.col};
}
Position::Position(int row, int col) : row(row), col(col) {}
Position::Position() {}
