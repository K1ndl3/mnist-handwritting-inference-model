#include "DrawingGrid.hpp"
#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <iostream>

void DrawingGrid::draw(int offsetX, int offsetY) {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            unsigned char intensity = static_cast<unsigned char>(_grid[row][col] * 255);
            Color cellColor = {intensity, intensity, intensity, 255};
            DrawRectangle(offsetX + col * CELL_SIZE, offsetY + row * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cellColor);
        }
    }
}

void DrawingGrid::stampBrush(int row, int col) {
    for (int row_offset = -1; row_offset <= 1; row_offset++) {
        for (int col_offset = -1; col_offset <= 1; col_offset++) {
            int brush_row = row + row_offset;
            int brush_col = col + col_offset;

            if (brush_row < 0 || brush_row >= NUM_ROWS || brush_col < 0 || brush_col >= NUM_COLS) {
                continue;
            }

            double value = (row_offset == 0 && col_offset == 0) ? 1.0 : 0.45;
            _grid[brush_row][brush_col] = std::max(_grid[brush_row][brush_col], value);
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
        int col_number = (x_pos - offsetX) / CELL_SIZE;
        int row_number = (y_pos - offsetY) / CELL_SIZE;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (!hasLastCell) {
                stampBrush(row_number, col_number);
            } else {
                int row_delta = row_number - lastRow;
                int col_delta = col_number - lastCol;
                int steps = std::max(std::abs(row_delta), std::abs(col_delta));

                for (int step = 0; step <= steps; step++) {
                    double t = (steps == 0) ? 0.0 : static_cast<double>(step) / steps;
                    int brush_row = std::round(lastRow + row_delta * t);
                    int brush_col = std::round(lastCol + col_delta * t);
                    stampBrush(brush_row, brush_col);
                }
            }

            hasLastCell = true;
            lastRow = row_number;
            lastCol = col_number;
            std::cout << "changed pixel\n";
        } else {
            hasLastCell = false;
        }
    } else {
        hasLastCell = false;
    }
}

void DrawingGrid::clear() {
    if (IsKeyDown(KEY_SPACE)) {
        for (int row = 0; row < NUM_ROWS; row++) {
            for (int col = 0; col < NUM_COLS; col++) {
                _grid[row][col] = 0;
            }
        }
        hasLastCell = false;
    }
}

std::vector<double> DrawingGrid::getNormalizedInput() {
    std::vector<double> normalizedInput;
    MATRIX centeredGrid(NUM_ROWS, std::vector<double>(NUM_COLS, 0.0));
    double totalIntensity = 0.0;
    double weightedRow = 0.0;
    double weightedCol = 0.0;

    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            double value = _grid[row][col];
            totalIntensity += value;
            weightedRow += row * value;
            weightedCol += col * value;
        }
    }

    if (totalIntensity == 0.0) {
        return std::vector<double>(NUM_ROWS * NUM_COLS, 0.0);
    }

    int rowShift = std::round((NUM_ROWS - 1) / 2.0 - (weightedRow / totalIntensity));
    int colShift = std::round((NUM_COLS - 1) / 2.0 - (weightedCol / totalIntensity));

    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            int centeredRow = row + rowShift;
            int centeredCol = col + colShift;

            if (centeredRow < 0 || centeredRow >= NUM_ROWS || centeredCol < 0 || centeredCol >= NUM_COLS) {
                continue;
            }

            centeredGrid[centeredRow][centeredCol] = std::max(centeredGrid[centeredRow][centeredCol], _grid[row][col]);
        }
    }

    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            normalizedInput.push_back(centeredGrid[row][col]);
        }
    }
    return normalizedInput;
}