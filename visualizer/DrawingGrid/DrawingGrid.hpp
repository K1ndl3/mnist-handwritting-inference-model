#pragma once
#include <vector>
#define NUM_COLS 28
#define NUM_ROWS 28
#define CELL_SIZE 20


class DrawingGrid {
    
private:
    int width = NUM_ROWS * CELL_SIZE;
    int heigth = NUM_COLS * CELL_SIZE;
    std::vector<std::vector<int>> _grid;
    
public:
    DrawingGrid() {
        _grid = std::vector<std::vector<int>>(NUM_ROWS, std::vector<int>(NUM_COLS, 0));
    }

    inline int getWidth() { return width; };
    inline int getHeight() {return heigth; };

    void draw(int offsetX, int offsetY);

    void input(int offsetX, int offsetY);

    void clear();
};