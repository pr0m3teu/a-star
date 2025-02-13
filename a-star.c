#include <stdio.h>
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

int get_dist(Pos first, Pos second)
{
    return ((second.x - first.x)*(second.x - first.x) + (second.y - first.y)*(second.y - first.y));
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

    Cell start_pos = {0};
    Pos end_pos = { HEIGHT, WIDTH };

    List open_list = {0};
    List close_list = {0};
    
    append(&open_list, start_pos);

    while(open_list.len != 0)
    {

    }

    return 0;
}
