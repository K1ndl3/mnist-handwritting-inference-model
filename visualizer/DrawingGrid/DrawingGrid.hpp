#pragma once
#include <vector>
#define NUM_COLS 28
#define NUM_ROWS 28
#define CELL_SIZE 20

using MATRIX = std::vector<std::vector<double>>;

class DrawingGrid {
    
private:
    int width = NUM_ROWS * CELL_SIZE;
    int heigth = NUM_COLS * CELL_SIZE;
    MATRIX _grid;
    bool hasLastCell = false;
    int lastRow = 0;
    int lastCol = 0;

    void stampBrush(int row, int col);
    
public:
    DrawingGrid() {
        _grid = MATRIX(NUM_ROWS, std::vector<double>(NUM_COLS, 0));
    }
    inline MATRIX getGrid() { return _grid; };
    inline int getWidth() { return width; };
    inline int getHeight() {return heigth; };
    std::vector<double> getNormalizedInput();

    void draw(int offsetX, int offsetY);

    void input(int offsetX, int offsetY);

    void clear();
};