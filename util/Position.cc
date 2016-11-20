
#include "Position.h"

Position Position::operator+(const Position &other) {
    return {row + other.row, col + other.col};
}
