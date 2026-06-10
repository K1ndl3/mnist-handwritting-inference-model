#include "raylib.h"
#include "DrawingGrid/DrawingGrid.hpp"

#define GRID_X_OFFSET 10
#define GRID_Y_OFFSET 10


int main(void)
{
    DrawingGrid grid;   
    InitWindow(900, 600, "MNIST Handwritting Predictor");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        grid.clear();
        grid.input(GRID_X_OFFSET, GRID_Y_OFFSET);
        grid.DrawingGrid::draw(GRID_X_OFFSET, GRID_Y_OFFSET);
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}