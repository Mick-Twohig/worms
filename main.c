#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "worm.h"

#define WIDTH 800
#define HEIGHT 600
#define NUM_WORMS 5

#define DEBUG 1

//void log(const char*str, ...){
//    va_list args;
//    va_start(args, str);
//    if (!DEBUG) return;
//    printf(str, args);
//}

int main(void) {
    srand(time(NULL));

    Worm worms[NUM_WORMS];
    for (size_t i = 0; i < NUM_WORMS; ++i) {
        float x_pos = rand() % WIDTH;
        float y_pos = rand() % HEIGHT;
        worms[i] = create_worm((Vector2) {x_pos, y_pos}, 5 + rand() % 25, 15,
                               (rand() % 360) * 1.0,
                               1 + (rand() % 15) * 1.0,
                               (Color) {rand() % 255,
                                        rand() % 255,
                                        rand() % 255, 125 + (rand() & 255 - 125)});
    }

    SetTargetFPS(30);
    InitWindow(WIDTH, HEIGHT, "Worms");

    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_Q)) break;

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        for (size_t i = 0; i < NUM_WORMS; ++i) {
            if (IsKeyDown(KEY_U)) {
                update_worm(&worms[i], (Vector2){WIDTH, HEIGHT});
            }
            draw_worm(worms[i]);

        }
        EndDrawing();
    }

    CloseWindow();
}
