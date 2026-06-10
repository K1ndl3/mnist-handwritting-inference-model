#include "DrawingGrid.hpp"
#include <raylib.h>
#include <iostream>
void DrawingGrid::draw(int offsetX, int offsetY) {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            int val = _grid[row][col];
            Color cellColor = (val == 0) ? BLACK : WHITE;
            DrawRectangle(offsetX + row * CELL_SIZE,offsetY + col * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cellColor);
        }
    }
}

void DrawingGrid::input(int offsetX, int offsetY) {
    // Vector2 GetMousePosition(void);

    // bool IsMouseButtonDown(int button);
    int height = DrawingGrid::getHeight();
    int width = DrawingGrid::getWidth();
    int right_bound = offsetX + width;
    int left_bound = offsetX;

    int top_bound = offsetY;
    int bottom_bound = offsetY + height;

    Vector2 mousePos = GetMousePosition();
    auto x_pos = mousePos.x;
    auto y_pos = mousePos.y; 

    if (x_pos <= right_bound && x_pos >= left_bound && y_pos <= bottom_bound && y_pos >= top_bound) {
        // calculate index based on mouse pos
        int row_number = (x_pos - offsetX) / CELL_SIZE;
        int col_number = (y_pos - offsetY) / CELL_SIZE;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            _grid[row_number][col_number] = 1;
            std::cout << "changed pixel\n";
        }
    }
}

void DrawingGrid::clear() {
    if (IsKeyDown(KEY_SPACE)) {
        for (int row = 0; row < NUM_ROWS; row++) {
            for (int col = 0; col < NUM_COLS; col++) {
                _grid[row][col] = 0;
            }
        }
    }
}