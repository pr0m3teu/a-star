#include <stdio.h>
#include <math.h>
#include <raylib.h>
#include <stdint.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 60

#define SQUARE_SIZE 40
#define WIDTH (1280 / SQUARE_SIZE)
#define HEIGHT (720 / SQUARE_SIZE)
#define grid_at(grid, i, j) (grid)[(int)(i)*WIDTH + (int)(j)]

#define draw_cell(i, j, color) DrawRectangleV((Vector2) { (int)(j) * SQUARE_SIZE, (int)(i) * SQUARE_SIZE }, (Vector2) { SQUARE_SIZE, SQUARE_SIZE }, (color))

static uint8_t grid[(WIDTH * HEIGHT)];
static bool found_dest = false;
static bool started = false;
 
#define n_count 8 // don't change
static Vector2 n_coords[n_count] = { {-1, -1}, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
 
typedef struct {
    Vector2 parent;
    Vector2 pos;
    int f, g, h;
} Cell;

typedef struct {
    size_t len;
    Cell items[WIDTH*HEIGHT];
} List;

// List ops
int append(List* list, Cell item)
{
    list->items[list->len] = item;
    list->len++;
    return list->len;
}

void pop(List *list, Vector2 parent, int f)
{
    for(size_t i = 0; i < list->len; ++i)
    {
        const Cell it = list->items[i];
        if (it.f == f && it.parent.x == parent.x && it.parent.y == parent.y)
        {
            for (size_t j = i; j < list->len; j++) list->items[j] = list->items[j+1]; 
            list->len--;
        }
    }
}

int get_dist(Vector2 first, Vector2 second)
{
    return ((second.x - first.x)*(second.x - first.x) + (second.y - first.y)*(second.y - first.y));
}

bool invalid_pos(Vector2 pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= HEIGHT || pos.y >= WIDTH || grid_at(grid, pos.x, pos.y) == 2) return true;
    return false;
}

bool is_dest(Vector2 pos, Vector2 dest)
{
    return (pos.x == dest.x && pos.y == dest.y);
}

void trace_route(List closed_list, Vector2 result[], Vector2 start)
{
    Cell curr_cell = closed_list.items[closed_list.len -1];

    size_t count = 0;
    result[count++] = curr_cell.pos; 
    while (!is_dest(curr_cell.pos, start))
    {
        for (size_t i = 0; i < closed_list.len; ++i)
        {
            const Cell it = closed_list.items[i];
            if(is_dest(it.pos, curr_cell.parent)) 
            {
                result[count++] = it.pos;
                grid_at(grid, curr_cell.pos.x, curr_cell.pos.y) = 1;
                curr_cell = it;
                break;
            }
            
        }
    }
}

void draw_grid()
{
    for (size_t i = 0; i < HEIGHT; ++i)
    {
        for (size_t j = 0; j < WIDTH; ++j)
        {
            const uint8_t val = grid_at(grid, i, j);
            switch(val)
            {
                case 0:
                    break;
                case 1:
                    draw_cell(i, j, RED);
                    break;
                case 2:
                    draw_cell(i, j, RAYWHITE);
                    break;
                default:
                    break;
            }
        }
    }

    DrawLineV((Vector2){1, 0}, (Vector2){1, SCREEN_HEIGHT}, RAYWHITE);
    DrawLineV((Vector2){0, 1}, (Vector2){SCREEN_WIDTH, 0}, RAYWHITE);
    for (int i = SQUARE_SIZE; i <= SCREEN_WIDTH; i += SQUARE_SIZE)
    {
        DrawLineV((Vector2){i, 0}, (Vector2){i, SCREEN_HEIGHT}, RAYWHITE);
    }

    for (int i = SQUARE_SIZE; i <= SCREEN_HEIGHT; i += SQUARE_SIZE)
    {
        DrawLineV((Vector2){0, i}, (Vector2){SCREEN_WIDTH, i}, RAYWHITE);
    }
}

int main(void)
{
   
    const Vector2 start = { 5, 30};
    const Vector2 end = {16, 1};
    const Cell start_cell = { .parent = start, .pos = start };

    List open_list = {0};
    List closed_list = {0};
    Vector2 result[WIDTH*HEIGHT];
    
    append(&open_list, start_cell);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "A* Visualizer");
    SetTargetFPS(FPS);
    while(!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ENTER)) started = !started;
        if (IsKeyPressed(KEY_R)) 
        {
            open_list.len = 0;
            closed_list.len = 0;
            append(&open_list, start_cell);
            found_dest = false;
            for (size_t i = 0; i < WIDTH*HEIGHT; ++i) if (grid[i] == 1) grid[i] = 0;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse_pos = GetMousePosition();
            mouse_pos.x /= (float)SQUARE_SIZE;
            mouse_pos.y /= (float)SQUARE_SIZE;

            mouse_pos.x = floorf(mouse_pos.x);
            mouse_pos.y = floorf(mouse_pos.y);

            if (grid_at(grid, mouse_pos.y, mouse_pos.x) == 0) 
            {
                grid_at(grid, mouse_pos.y, mouse_pos.x) = 2;
            }
            else 
            {
                grid_at(grid, mouse_pos.y, mouse_pos.x) = 0;
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);            
            if (started)
            {
                if (!found_dest && open_list.len != 0)
                {
                    Cell q = open_list.items[0];
                    for (size_t i = 1; i < open_list.len; ++i)
                    {   
                        const Cell it = open_list.items[i]; 
                        if (it.f < q.f) q = it;    
                    }
                    pop(&open_list, q.parent, q.f);

                    for (size_t i = 0; i < n_count; ++i)
                    {
                        Cell child = { 
                            .parent = q.pos,
                            .pos = { .x = q.pos.x + n_coords[i].x, .y = q.pos.y + n_coords[i].y },
                        };

                        if (invalid_pos(child.pos)) continue;

                        if (is_dest(child.pos, end)) 
                        {
                            found_dest = true;
                            break;
                        }

                        child.h = get_dist(child.pos, end);
                        child.g = q.f;
                        child.f = child.g + child.h;

                        bool found = false; 
                        for (size_t j = 0; j < open_list.len; j++)
                        {
                            const Cell it = open_list.items[j];
                            if (it.pos.x == child.pos.x && it.pos.y == child.pos.y && it.f < child.f)
                            {
                                found = true;
                                break;
                            }
                        }

                        for (size_t j = 0; j < closed_list.len; j++)
                        {
                            const Cell it = closed_list.items[j];
                            if (it.pos.x == child.pos.x && it.pos.y == child.pos.y && it.f < child.f)
                            {
                                found = true;
                                break;
                            }

                        }
                        if (found) continue;

                        append(&open_list, child);

                    }
                    append(&closed_list, q);
                    for (size_t i = 0; i < closed_list.len; ++i)
                    {
                        const Vector2 it = closed_list.items[i].pos;
                        draw_cell(it.x, it.y, GREEN);
                    }
                }
                else
                {
                    trace_route(closed_list, result, start);
                }
            }

            draw_cell(start.x, start.y, BLUE);
            draw_cell(end.x, end.y, BLUE);
            draw_grid();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
