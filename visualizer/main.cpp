#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);
            

        EndDrawing();
    }

    CloseWindow();
    return 0;
}