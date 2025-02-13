#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define SQUARE_SIZE 40
#define WIDTH (1280 / SQUARE_SIZE)
#define HEIGHT (720 / SQUARE_SIZE)
#define n_count 8

#define grid_at(grid, i, j) (grid)[(i)*WIDTH + (j)]

static uint8_t grid[(WIDTH * HEIGHT)];
 

typedef struct {
    int x;
    int y;
} Pos;

static Pos n_coords[n_count] = { {-1, -1}, { -1, 0 }, { -1, 1 }, { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 0 }, { 1, 1 } };
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
            for (size_t j = i; j < list->len; j++) list->items[j] = list->items[j+1]; 
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
    if (pos.x < 0 || pos.y < 0 || pos.x >= HEIGHT || pos.y >= WIDTH || grid_at(grid, pos.x, pos.y) == 2) return true;
    return false;
}

bool is_dest(Pos pos, Pos dest)
{
    return (pos.x == dest.x && pos.y == dest.y);
}

void trace_route(List closed_list, Pos result[], Pos start)
{
    Cell curr_cell = closed_list.items[closed_list.len -1];

    size_t count = 0;
    result[count++] = curr_cell.pos; 
    while (!is_dest(curr_cell.pos, start))
    {
        for (size_t i = 0; i < closed_list.len; ++i)
        {
            Cell it = closed_list.items[i];
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

void a_star(Pos start, Pos end)
{
    Cell start_cell = { .parent = start, .pos = start };
    List open_list = {0};
    List closed_list = {0};
    Pos result[WIDTH*HEIGHT];
    
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

        }
        append(&closed_list, q);
        if (found_dest) break;
    }
    trace_route(closed_list, result, start);
}


int main(void)
{
    Pos start = {9, 28};
    Pos end = {3, 0};
    grid_at(grid, start.x, start.y) = 5;
    grid_at(grid, end.x, end.y) = 4;
    grid_at(grid, 8, 27) = 2;
    grid_at(grid, 9, 27) = 2;
    printf("%d %d\n", end.x, end.y);
    a_star(start, end);
    for (int i = 0; i < HEIGHT; i ++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            printf("%d ", grid[i*WIDTH+j]);
        }
        printf("\n");
    }
    
    return 0;
}
