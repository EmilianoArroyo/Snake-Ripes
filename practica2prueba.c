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
int snake_length = 1;

void growSnake() {
    // Crecer la serpiente y actualizar la matriz de LED
    for (int i = snake_length; i > 0; i--) {
        snake_body[i] = snake_body[i - 1];
    }
    snake_body[0] = snake_head;
    snake_length++;

    // Asegúrate de que la nueva cabeza y el nuevo cuerpo se iluminen
    for (int i = 0; i < snake_length; i++) {
        if (snake_body[i] != 0) {
            *(snake_body[i]) = 0xFF0000; // Color rojo para la serpiente
        }
    }
}

void eatApple() {
    if (snake_head == apple) {
        growSnake();
        generateApple();
    }
}

int checkCollision() {
    // Obtén el color en la posición de la cabeza de la serpiente
    int color = *(snake_head);

    // Verifica si el color coincide con el color de colisión (0xFFFFFF)
    if (color == 0xFFFFFF) {
        return 1; // Colisión detectada
    }

    return 0; // No hay colisión
}

void moveSnake() {
    // Mover la serpiente y actualizar la matriz de LED
    // Asegúrate de borrar la cola si la serpiente no crece
    if (*d_pad_up && last_move != 2) {
        moveUp();
        last_move = 1;
    } else if (*d_pad_do && last_move != 1) {
        moveDown();
        last_move = 2;
    } else if (*d_pad_le && last_move != 4) {
        moveLeft();
        last_move = 3;
    } else if (*d_pad_ri && last_move != 3) {
        moveRight();
        last_move = 4;
    }

    // Verificar si la cabeza de la serpiente está en una posición con color blanco
    int head_color = *(snake_head);
    if (head_color == 0xFFFFFF)
    {
        // Apagar todos los LEDs y finalizar el juego
        clearLedMatrix();
        return;
    }

    // Actualizar la matriz de LED para la serpiente
    for (int i = 0; i < snake_length; i++) {
        if (i == 0) {
            // Cabeza de la serpiente
            *(snake_body[i]) = 0xFF0000;
        } else {
            // Cuerpo de la serpiente
            *(snake_body[i]) = 0x7F0000; // Un color rojo más oscuro para el cuerpo
        }
    }

    // Borrar el segmento de cola si la serpiente no crece
    if (snake_length > 1 && snake_body[snake_length] != 0) {
        *(snake_body[snake_length]) = 0x000000; // Apagar el último segmento
    }
}

void printlimits(int color) {
    for (int y = 0; y < led_matrix_height; y++) {
        for (int x = 0; x < led_matrix_width; x++) {
            // el led actual está en el borde?
            if (x == 0 || y == 0 || x == led_matrix_width - 1 || y == led_matrix_height - 1) {
                *(led_base + y * led_matrix_width + x) = color;
            }
        }
    }
}

void main()
{
    clearLedMatrix();
    printlimits(0xFFFFFF);
    initSnake();
    generateApple();

    int game_over = 0; // Variable para controlar si el juego ha terminado

    while (!game_over)
    {
        wait(1500);
        moveSnake();
        eatApple();

        // Verificar si la cabeza de la serpiente está en una posición con color blanco
        int head_color = *(snake_head);
        if (head_color == 0xFFFFFF)
        {
            game_over = 1; // Establecer la variable de juego terminado
        }
    }

    // Aquí puedes realizar cualquier acción que desees cuando el juego termine
    // Por ejemplo, mostrar un mensaje de "Juego Terminado" o reiniciar el juego.
}


void initSnake()
{
    // Calcula la posición central de la matriz
    int mid_y = (led_matrix_height / 2) - 3;
    int mid_x = (led_matrix_width / 2) - 3;

    // Coloca la cabeza de la serpiente en el medio de la matriz
    snake_head = (volatile unsigned int *)(led_base + (led_matrix_width * mid_y) + mid_x);
    *(int *)snake_head = 0xFF0000; // Color rojo para la cabeza de la serpiente

    // Inicializa la longitud de la serpiente y el primer segmento del cuerpo
    snake_length = 1;
    snake_body[0] = snake_head;
}

void generateApple()
{
    int y_rand, x_rand;
    int found_space = 0;

    while (!found_space)
    {
        srand(next);
        y_rand = rand() % led_matrix_height;
        x_rand = rand() % led_matrix_width;
        apple = led_base + (led_matrix_width * y_rand) + x_rand;

        found_space = 1;
        for (int i = 0; i < led_matrix_height * led_matrix_width; i++)
        {
            if (snake_body[i] == apple)
            {
                found_space = 0;
                break;
            }
        }
        next += 1;
    }
    *apple = 0x00FF00;
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
