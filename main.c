
#include "raylib.h"

#include "game.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    initGame();

    InitWindow(screenWidth, screenHeight, "Breakout!");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(DARKGRAY);

            updateGame();
            drawGame(10, 10);
        EndDrawing();
    }

    return 0;
}