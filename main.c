#include "ripes_system.h"
#include <stdio.h>

#define led_matrix_height LED_MATRIX_0_HEIGHT
#define led_matrix_width LED_MATRIX_0_WIDTH

// Se almacena la direccion inicio de la matriz
volatile unsigned int *led_base = (volatile unsigned int *)LED_MATRIX_0_BASE;

// Se almacena la direccion de los D-Pads
volatile unsigned int *d_pad_up = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_pad_do = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_pad_le = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_pad_ri = (volatile unsigned int *)D_PAD_0_RIGHT;

volatile unsigned int *snake_body[led_matrix_height * led_matrix_width];
volatile unsigned int *snake_head = 0;
volatile unsigned int *apple = 0;

void srand(unsigned int seed);
int rand();
void wait(int ciclos);
void initSnake();
void clearLedMatrix();
void generateApple();
void moveUp();
void moveDown();
void moveLeft();
void moveRight();

unsigned int next = 1;
int last_move = 0;
int x_position = 0;
int y_position = 0;

void main()
{
    clearLedMatrix();
    initSnake();
    generateApple();

    while (1)
    {
        wait(1500);
    }
}

void initSnake()
{
    snake_head = (volatile unsigned int *)led_base;
    *(int *)snake_head = 0xFF0000;
}

void generateApple()
{
    srand(next);
    int y_rand = rand() % led_matrix_height;
    int x_rand = rand() % led_matrix_width;
    apple = led_base + (led_matrix_width * y_rand) + x_rand;
    *apple = 0x00FF00;
    next += 1;
}

void moveUp()
{
    *snake_head = 0x000000;
    snake_head = snake_head - led_matrix_width;
    *snake_head = 0xFF0000;
}

void moveDown()
{
    *snake_head = 0x000000;
    snake_head = snake_head + led_matrix_width;
    *snake_head = 0xFF0000;
}

void moveLeft()
{
    *snake_head = 0x000000;
    snake_head = snake_head - 1;
    *snake_head = 0xFF0000;
}

void moveRight()
{
    *snake_head = 0x000000;
    snake_head = snake_head + 1;
    *snake_head = 0xFF0000;
}

void moveSnake(int up, int down, int right, int left)
{
    if ((*d_pad_up == up) || (last_move == 1))
    {
        moveUp();
        last_move = 1;
    }
    if ((*d_pad_do == down) || (last_move == 2))
    {
        moveDown();
        last_move = 2;
    }
    if ((*d_pad_le == left) || (last_move == 3))
    {
        moveLeft();
        last_move = 3;
    }
    if ((*d_pad_ri == right) || (last_move == 4))
    {
        moveRight();
        last_move = 4;
    }
}

void clearLedMatrix()
{
    for (int i = 0x0; i < led_matrix_height; i++)
    {
        for (int j = 0x0; j < led_matrix_width; j++)
        {
            *(led_base + (led_matrix_width * i) + j) = 0x000000;
        }
    }
}

void srand(unsigned int seed)
{
    next = seed;
}

int rand()
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void wait(int ciclos)
{
    for (int i = 0; i < ciclos; i++)
    {
    }
}