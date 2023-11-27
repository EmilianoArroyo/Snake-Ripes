#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SW0 (0x01)
#define SW1 (0x02)
#define SW2 (0x04)
#define SW3 (0x08)
#define SW4 (0x10)
#define SW5 (0x20)
#define SW6 (0x40)
#define SW7 (0x80)

#define LED_MATRIX_0_SIZE	(0xdac)
#define LED_MATRIX_0_WIDTH	(0x23)
#define LED_MATRIX_0_HEIGHT	(0x19)

volatile unsigned int * led_base = (volatile unsigned int *)LED_MATRIX_0_BASE;

volatile unsigned int * d_pad_up = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int * d_pad_do = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int * d_pad_le = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int * d_pad_ri = (volatile unsigned int *)D_PAD_0_RIGHT;


#define MAX_SNAKE_SIZE (LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT)

int game = 1;
int head = 1;
int tail = 0;

int randcounter = 50;

int size = 2;

int new_head_index;
int new_head_index1;

int dx = 1;
int dy = 0;

volatile unsigned int snakeLEDs[MAX_SNAKE_SIZE];

void initSnake() {
    printf("inicia initSnake");
    // Inicializar la serpiente en algún lugar cerca del centro de la matriz de LED
    // La serpiente está representada por un bloque 2x2 LEDs
    snakeLEDs[0] = (LED_MATRIX_0_WIDTH * 13) + 15; // Primer LED
    snakeLEDs[1] = (LED_MATRIX_0_WIDTH * 13) + 16;     // Segundo LED a la derecha del primero
    snakeLEDs[2] = (LED_MATRIX_0_WIDTH * 14) + 15; // Tercer LED debajo del primero
    snakeLEDs[3] = (LED_MATRIX_0_WIDTH * 14) + 16;     // Cuarto LED a la derecha del tercero

    // Encender los LEDs en las posiciones iniciales de la serpiente a color rojo
    for (int i = 0; i < 4; i++) {
        *(led_base + snakeLEDs[i]) = 0xFF0000;
    }

    // Actualizar la cabeza y la cola de la serpiente
    head = 1; // La cabeza es el último LED en el array
    tail = 2; // La cola es el primer LED en el array
}


int moveSnake() {
    printf("\ninicia moveSnake");
    
    // Calcula las nuevas posiciones de la cabeza asumiendo que la serpiente tiene una forma de 2x2
    int new_head_indices[4];
    if (dy == 1) { // Movimiento hacia abajo
        new_head_indices[0] = snakeLEDs[2] + LED_MATRIX_0_WIDTH; // Nueva posición del LED inferior izquierdo
        new_head_indices[1] = snakeLEDs[3] + LED_MATRIX_0_WIDTH; // Nueva posición del LED inferior derecho
    } else if (dy == -1) { // Movimiento hacia arriba
        new_head_indices[0] = snakeLEDs[0] - LED_MATRIX_0_WIDTH; // Nueva posición del LED superior izquierdo
        new_head_indices[1] = snakeLEDs[1] - LED_MATRIX_0_WIDTH; // Nueva posición del LED superior derecho
    } else if (dx == 1) { // Movimiento hacia la derecha
        new_head_indices[0] = snakeLEDs[1] + 1; // Nueva posición del LED superior derecho
        new_head_indices[1] = snakeLEDs[3] + 1; // Nueva posición del LED inferior derecho
    } else if (dx == -1) { // Movimiento hacia la izquierda
        new_head_indices[0] = snakeLEDs[0] - 1; // Nueva posición del LED superior izquierdo
        new_head_indices[1] = snakeLEDs[2] - 1; // Nueva posición del LED inferior izquierdo
    }

    // Verificar colisiones con los bordes o con la serpiente misma para los nuevos índices de la cabeza
    for (int i = 0; i < 2; i++) { // Solo necesitamos verificar las dos nuevas posiciones de la cabeza
        if (*(led_base + new_head_indices[i]) == 0xFFFFFF || // colisión con el borde
            *(led_base + new_head_indices[i]) == 0xFF0000) { // colisión con la serpiente
            return 0; // Fin del juego
        }
    }

    // Apagar los LEDs de la cola
    *(led_base + snakeLEDs[0]) = 0x0; // Apagar el LED superior izquierdo de la cola
    *(led_base + snakeLEDs[2]) = 0x0; // Apagar el LED inferior izquierdo de la cola

    // Mover la serpiente
    snakeLEDs[0] = snakeLEDs[1]; // Mover el LED superior derecho a la posición del LED superior izquierdo
    snakeLEDs[2] = snakeLEDs[3]; // Mover el LED inferior derecho a la posición del LED inferior izquierdo
    snakeLEDs[1] = new_head_indices[0]; // Actualizar la nueva posición del LED superior derecho
    snakeLEDs[3] = new_head_indices[1]; // Actualizar la nueva posición del LED inferior derecho

    // Encender los nuevos LEDs de la cabeza
    *(led_base + new_head_indices[0]) = 0xFF0000;
    *(led_base + new_head_indices[1]) = 0xFF0000;

    return 1; // Continuar el juego
}



void eatApple() { //no funciona bien
    head = (head + 1) % MAX_SNAKE_SIZE;
    snakeLEDs[head] = new_head_index;
    *(led_base + new_head_index) = 0x00FF00;
}

void main() {
    cleanBoard();
    initSnake();
    printlimits(0xFFFFFF); // Colorea los bordes de blanco
    generateApple();
    while (game){
        game = moveSnake();
        // Implementar una pausa más efectiva si es necesario
        for (int i = 0; i < 70000; i++){
            // Delay loop (podría necesitar ajuste)
        }
        if(*d_pad_up == 1) changeDirection(0, -1);
        if(*d_pad_do == 1) changeDirection(0, 1);
        if(*d_pad_le == 1) changeDirection(-1, 0);
        if(*d_pad_ri == 1) changeDirection(1, 0);
    }
    cleanBoard();
}

void cleanBoard() {
    volatile unsigned int* ptr = led_base;
    for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
        *ptr++ = 0x0;
    }
}

void printlimits(int color) {
    for (int y = 0; y < LED_MATRIX_0_HEIGHT; y++) {
        for (int x = 0; x < LED_MATRIX_0_WIDTH; x++) {
            // el led actual está en el borde?
            if (x == 0 || y == 0 || x == LED_MATRIX_0_WIDTH - 1 || y == LED_MATRIX_0_HEIGHT - 1) {
                *(led_base + y * LED_MATRIX_0_WIDTH + x) = color;
            }
        }
    }
}

void generateApple() {
    int flag = 1;
    randcounter += 5;
    srand(randcounter);
    while (flag) {
        int random_x = 1 + rand() % (LED_MATRIX_0_WIDTH -3);
        int random_y = 1 + rand() % (LED_MATRIX_0_HEIGHT -3);
        int position1 = random_y * LED_MATRIX_0_WIDTH + random_x;
        int position2 = position1 + 1;
        int position3 = position1 + LED_MATRIX_0_WIDTH;
        int position4 = position3 + 1;

        // Verifica si alguna posicion está ocupada por la serpiente o los bordes, si todas estan desocupadas, pone la manzana
        if (*(led_base+position1) == 0x0 && 
            *(led_base+position2) == 0x0 && 
            *(led_base+position3) == 0x0 && 
            *(led_base+position4) == 0x0) {
                
            // colorea manzana 
            *(led_base+position1) = 0x00FF00; 
            *(led_base+position2) = 0x00FF00; 
            *(led_base+position3) = 0x00FF00; 
            *(led_base+position4) = 0x00FF00; 
            flag = 0;
        }
    }
}

void changeDirection(int dex, int dey) {
    dx = dex;
    dy = dey;
}
