#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 10 

static double TARGET_FRAME_TIME = 1.0 / FPS;

#define SQUARE_SIZE 40
#define WIDTH (1280 / SQUARE_SIZE)
#define HEIGHT (720 / SQUARE_SIZE)

#define grid_at(grid, i, j) (grid)[(int)(i)*WIDTH + (int)(j)]

static uint8_t grid[(WIDTH * HEIGHT)];
static bool found_dest = false;
static bool started = true;
 
typedef struct {
    int x, y;
} Vector2;


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
typedef enum {
    RED,
    GREEN,
    BLUE,

    WHITE,
    BLACK

} Color;

void scc(int code)
{
    if (code < 0) {
        SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}


void scp(void* p)
{
    if (p == NULL) {
        SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

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

void draw_rect(SDL_Renderer *renderer, SDL_Rect rect, Color color)
{   
    scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF));
    // Draw Color must already be set
    switch (color)
    {
        case RED:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0XFF);
            break;
        case GREEN:
            SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
            break;
        
        case BLUE:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0xFF, 0xFF);
            break;

        case WHITE:
            SDL_SetRenderDrawColor(renderer, 0xFF, 0XFF, 0xFF, 0xFF);

        default:
            break;

    }
    scc(SDL_RenderDrawRect(renderer, &rect));
    scc(SDL_RenderFillRect(renderer, &rect));
    scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF));
}

void draw_cell(SDL_Renderer *renderer, Vector2 pos, Color color)
{
    SDL_Rect rect = { pos.y * SQUARE_SIZE, pos.x * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
    draw_rect(renderer, rect, color);
}

void draw_grid(SDL_Renderer *renderer)
{
    for (size_t i = 0; i < HEIGHT; ++i)
    {
        for (size_t j = 0; j < WIDTH; ++j)
        {
            const uint8_t val = grid_at(grid, i, j);
            Vector2 pos = {i, j};
            switch(val)
            {
                case 0:
                    break;
                case 1:
                    draw_cell(renderer, pos, (Color){RED});
                    break;
                case 2:
                    draw_cell(renderer, pos, (Color){WHITE});
                    break;
                default:
                    break;
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    for (size_t i = 0; i < SCREEN_WIDTH; i += SQUARE_SIZE)
    {
        scc(SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT));
    }

    for (size_t i = 0; i < SCREEN_HEIGHT; i += SQUARE_SIZE)
    {
        scc(SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i));
    }


    scc(SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1));
    scc(SDL_RenderDrawLine(renderer, SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT));
}


int main(void)
{
    grid_at(grid, 5,5) = 2;
    scc(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL succesfully initialized!");

    SDL_Window *window = SDL_CreateWindow("Hello World!", 10, 10, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    scp(window);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    scp(renderer);

    const Vector2 start = { 1, 1};
    const Vector2 end = {10, 10};
    const Cell start_cell = { .parent = start, .pos = start };

    List open_list = {0};
    List closed_list = {0};
    Vector2 result[WIDTH*HEIGHT];
    
    append(&open_list, start_cell);

    SDL_Event event;
    bool running = true;
    Uint64 lastTime = SDL_GetPerformanceCounter();
    double deltaTime = 0;
    
    while (running)
    {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        
                        case SDLK_RETURN:
                            started = !started;
                            break;

                        case SDLK_r:
                            {
                                open_list.len = 0;
                                closed_list.len = 0;
                                append(&open_list, start_cell);
                                found_dest = false;
                                for (size_t i = 0; i < WIDTH*HEIGHT; ++i) if (grid[i] == 1) grid[i] = 0;
                                break;
                            }

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        scc(SDL_RenderClear(renderer));

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
                    draw_cell(renderer, it, (Color){ GREEN } );
                }
            }
            else
            {
                trace_route(closed_list, result, start);
            }
        }
        draw_cell(renderer, start, (Color) { BLUE });
        draw_cell(renderer, end, (Color) { BLUE });
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        draw_grid(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderPresent(renderer);

        double frameTime = (double)(SDL_GetPerformanceCounter() - currentTime) / SDL_GetPerformanceFrequency();
        if (frameTime < TARGET_FRAME_TIME) {
            SDL_Delay((TARGET_FRAME_TIME - frameTime) * 1000);
        }
    }

    SDL_DestroyWindow(window);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Window destroyed!");
    SDL_Quit();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL succesfully closed!");
    return 0;
}
