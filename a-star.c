#include <stdio.h>
#include <stdint.h>

#define SQUARE_SIZE 40
#define WIDTH (120 / SQUARE_SIZE)
#define HEIGHT (80 / SQUARE_SIZE)

static uint8_t grid[(WIDTH * HEIGHT)];

int main(void)
{
    printf("Hello World\n");
    for (int i = 0; i < WIDTH; i ++)
    {
        for(int j = 0; j < HEIGHT; j++)
        {
            printf("%d ", grid[i*WIDTH+j]);
        }
        printf("\n");
    }
    return 0;
}
