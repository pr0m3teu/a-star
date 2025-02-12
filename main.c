#include <stdio.h>
#include <raylib.h>

#define WIDTH 1280
#define HEIGHT 720
#define SQUARE_SIZE 40 

void draw_grid()
{
    DrawLineV((Vector2){1, 0}, (Vector2){1, HEIGHT}, RAYWHITE);
    DrawLineV((Vector2){0, 1}, (Vector2){WIDTH, 0}, RAYWHITE);
    for (int i = SQUARE_SIZE; i <= WIDTH; i += SQUARE_SIZE)
    {
        DrawLineV((Vector2){i, 0}, (Vector2){i, HEIGHT}, RAYWHITE);
    }

    for (int i = SQUARE_SIZE; i <= HEIGHT; i += SQUARE_SIZE)
    {
        DrawLineV((Vector2){0, i}, (Vector2){WIDTH, i}, RAYWHITE);
    }

}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "A* Visualizer");
    while(!WindowShouldClose())
    {
        BeginDrawing();
            draw_grid();
        EndDrawing();
    }
    CloseWindow();

    return 0;
}
