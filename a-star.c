#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define SQUARE_SIZE 40
#define WIDTH (1280 / SQUARE_SIZE)
#define HEIGHT (720 / SQUARE_SIZE)

#define grid_at(grid, i, j) (grid)[(i)*WIDTH + (j)]

static uint8_t grid[(WIDTH * HEIGHT)];
typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    Pos parent;
    Pos pos;
    int f, g, h;
} Cell;

typedef struct {
    size_t len;
    Cell items[WIDTH*HEIGHT];
} List;

int append(List* list, Cell item)
{
    list->items[list->len] = item;
    list->len++;
    return list->len;
}

void pop(List *list, Pos parent, int f)
{
    for(size_t i = 0; i < list->len; ++i)
    {
        Cell it = list->items[i];
        if (it.f == f && it.parent.x == parent.x && it.parent.y == parent.y)
        {
            for (size_t j = i; j < list->len; j++)
            {
                list->items[j] = list->items[j+1]; 
            }
            list->len--;
        }

    }
}

int get_dist(Pos first, Pos second)
{
    return ((second.x - first.x)*(second.x - first.x) + (second.y - first.y)*(second.y - first.y));
}

bool invalid_pos(Pos pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= WIDTH || pos.y >= HEIGHT) return true;
    return false;
}

bool is_dest(Pos pos, Pos dest)
{
    return (pos.x == dest.x && pos.y == dest.y);
}


int main(void)
{
    printf("Hello World\n");
    grid_at(grid, 1, 2) = 10;
    for (int i = 0; i < HEIGHT; i ++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            printf("%d ", grid[i*WIDTH+j]);
        }
        printf("\n");
    }
   
    const size_t n_count = 8;
    Pos n_coords[n_count] = { {-1, -1}, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };

    Pos start_pos = {0};
    Cell start_cell = { .parent = start_pos, .pos = start_pos };
    Pos end_pos = { HEIGHT, WIDTH };

    List open_list = {0};
    List closed_list = {0};
    
    append(&open_list, start_cell);

    bool found_dest = false;
    while(open_list.len != 0)
    {
        Cell q = open_list.items[0];
        for (size_t i = 1; i < open_list.len; ++i)
        {   
            Cell it = open_list.items[i]; 
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

            if (is_dest(child.pos, end_pos)) 
            {
                found_dest = true;
                break;
            }

            child.h = get_dist(child.pos, end_pos);
            child.g = q.f;
            child.f = child.g + child.h;

            bool found = false; 
            for (size_t j = 0; j < open_list.len; j++)
            {
                Cell it = open_list.items[j];
                if (it.pos.x == child.pos.x && it.pos.y == child.pos.y && it.f < child.f)
                {
                    found = true;
                    break;
                }
            }

            for (size_t j = 0; j < closed_list.len; j++)
            {
                Cell it = closed_list.items[j];
                if (it.pos.x == child.pos.x && it.pos.y == child.pos.y && it.f < child.f)
                {
                    found = true;
                    break;
                }

            }
            if (found) continue;

            append(&open_list, child);
            printf("%d\n", open_list.len);

        }
        append(&closed_list, q);
        
        if (found_dest) break;
    }

    for (size_t i = 0; i < closed_list.len; ++i)
    {
        printf("x: %d, y: %d\n ", closed_list.items[i].pos.x, closed_list.items[i].pos.y);
    }

    return 0;
}
