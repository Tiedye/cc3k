#pragma once


struct Position {
    int y;
    int x;
    Position();
    Position(int row, int col);
    Position(const Position &other) = default;
    Position(Position &&other) = default;
    Position &operator=(const Position &other) = default;
    Position &operator=(Position &&other) = default;

    Position operator+(const Position &other) const;
    Position operator-(const Position &other) const;
    bool operator==(const Position &other) const;
    bool operator!=(const Position &other) const;

    void shiftX(int amount);
    void shiftY(int amount);
};


