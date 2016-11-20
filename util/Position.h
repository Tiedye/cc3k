#pragma once


struct Position {
    int row;
    int col;
    Position operator+(const Position&other);
};


