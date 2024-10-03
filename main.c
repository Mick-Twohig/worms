#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define NUM_WORMS 5

#define DEBUG 1

typedef struct Segment {
    Vector2 position;
    float radius;
    float angle;
    Color color;
    float parent_distance; // distance to the segment in front of this one
} Segment;

typedef struct Worm {
    Segment *segments;
    size_t num_segments;
    size_t intersegment_space;
    Vector2 velocity;
    Color body_color;
} Worm;

//void log(const char*str, ...){
//    va_list args;
//    va_start(args, str);
//    if (!DEBUG) return;
//    printf(str, args);
//}

void print_segment_data(Segment *s) {
    printf("<Segment x=%.2f, y=%.2f, r=%.2f, a=%.2f>\n", s->position.x, s->position.y, s->radius, s->angle);
}

void print_worm_data(Worm w) {
    printf("Worm\n");
    for (size_t i = 0; i < w.num_segments; ++i) {
        printf("Segment %zu: ", i);
        Segment *s = w.segments + i;
        print_segment_data(s);

    }
    printf("done\n\n\n");
}

void draw_segment(Segment segment) {
    DrawCircle(segment.position.x, segment.position.y, segment.radius, segment.color);
    DrawLine(segment.position.x,
             segment.position.y,
             segment.position.x + segment.radius * cosf(segment.angle),
             segment.position.y + segment.radius * sinf(segment.angle),
             BLACK);
}

Worm create_worm(Vector2 position, size_t num_segments, size_t intersegment_space, Vector2 velocity, Color body_color) {
    Worm worm;
    float radius = 15.0;
    worm.num_segments = num_segments;
    worm.intersegment_space = intersegment_space;
    worm.velocity = velocity;
    worm.body_color = body_color;
    worm.segments = malloc(num_segments * sizeof(Segment));
    if (!worm.segments) {
        fprintf(stderr, "Foobar in malloc in create_worm");
        exit(1);
    }
    Segment *segment = worm.segments;
    segment->radius = radius;
    segment->angle = atan(velocity.y / velocity.x); // angle is in direction of initial velocity vector
    segment->position = (Vector2) {position.x, position.y};
    segment->color = MAGENTA;

    for (size_t i = 1; i < num_segments; ++i) {
        segment = worm.segments + i;

        float scale = ((float) num_segments - i) / num_segments;
        float distance = scale * worm.intersegment_space;
        segment->parent_distance = distance;
        segment->radius = radius * scale;

        Segment *forwardSegment = segment - 1;


        segment->position = (Vector2) {forwardSegment->position.x - cosf(segment->angle) * distance,
                                       forwardSegment->position.y + sinf(-segment->angle) * distance};

        segment->angle = atan2(forwardSegment->position.y + sinf(-segment->angle) * distance,
                               forwardSegment->position.x - cosf(segment->angle) * distance);

        segment->color = worm.body_color;
        print_segment_data(segment);

    }

    printf("Worm created at location %.2f, %.2f\n", position.x, position.y);

    return worm;
}

void draw_worm(Worm w) {

    Segment *s;
    for (size_t i = 0; i < w.num_segments; ++i) {
        s = w.segments + i;
        draw_segment(*s);
    }
}

void update_worm(Worm *w) {
    Segment *forwardSeg, *rearSeg;
    float distance, delta, angle;
    // First, update the frontmost segment
    forwardSeg = w->segments;

    // follow mouse
//    Vector2 mouse = GetMousePosition();
//    angle = atan2(mouse.y - forwardSeg->position.y, mouse.x - forwardSeg->position.x );
//    distance = Vector2Distance(mouse, forwardSeg->position);
//    forwardSeg->angle = angle;
//    if (distance > 10) {
//        forwardSeg->position.x += cosf(angle) * forwardSeg->radius;
//        forwardSeg->position.y += sinf(angle) * forwardSeg->radius;
//    }

    Vector2 newPos = forwardSeg->position;
    newPos = Vector2Add(newPos, w->velocity);

    // determine the angle we're traveling at
    // extend line from that angle to find intersection with wall
    // if we're close to wall, start turning around
    angle = atan2f(w->velocity.y, w->velocity.x);

    // distance to right wall
    float dr = fabsf((WIDTH - newPos.x) / cosf(angle));
    // distance to top wall
    float dt = fabsf(newPos.y / (cosf(PI / 2 - angle)));
    // distance to left wall
    float dl = fabsf(newPos.x / cosf(angle));
    // distance to bottom wall
    float db = fabsf((HEIGHT - newPos.y) / cosf(PI / 2 - angle));

    enum wall {
        left, top, right, bottom
    } wall;
    float nearest_distance = WIDTH * HEIGHT + 1.0;
    if (dl < nearest_distance) {
        wall = left;
        nearest_distance = dl;
    }
    if (dr < nearest_distance) {
        wall = right;
        nearest_distance = dr;
    }
    if (dt < nearest_distance) {
        wall = top;
        nearest_distance = dt;
    }
    if (db < nearest_distance) {
        wall = bottom;
        nearest_distance = db;
    }

    if (nearest_distance < 40) { //arbitrary
        switch (wall) {
            case top:
                w->velocity.y *= -1;
                break;
            case bottom:
                w->velocity.y *= -1;
                break;
            case left:
                w->velocity.x *= -1;
                break;
            case right:
                w->velocity.x *= -1;
                break;
        }
    }

    // wiggle-wiggle
//    double time = GetTime();
//    float speed = Vector2Length(w->velocity);
//    forwardSeg->angle += 20 * cos(time * 1000);
//    w->velocity.x = speed * cosf(forwardSeg->angle);
//    w->velocity.y = speed * sinf(forwardSeg->angle);

//    printf("%.3f : %.3f\n", time, 20 * cos(time));


    forwardSeg->position = newPos;

    for (size_t i = 1; i < w->num_segments; ++i) {
        forwardSeg = w->segments + i - 1;
        rearSeg = w->segments + i;

        angle = atan2(forwardSeg->position.y - rearSeg->position.y,
                      forwardSeg->position.x - rearSeg->position.x);
        rearSeg->angle = angle;

        distance = Vector2Distance(forwardSeg->position, rearSeg->position);
        //delta = distance - w.intersegment_space ;
        delta = distance - rearSeg->parent_distance;

        rearSeg->position.x += cosf(angle) * delta;
        rearSeg->position.y += sinf(angle) * delta;
        //printf("distance: %.4f, delta: %.4f\n", distance, delta);
    }
}

int main(void) {
    srand(time(NULL));


    Worm worms[NUM_WORMS];
    for (size_t i = 0; i < NUM_WORMS; ++i) {
        float x_pos = rand() % WIDTH;
        float y_pos = rand() % HEIGHT;
        worms[i] = create_worm((Vector2) {x_pos, y_pos}, 5 + rand() % 25, 15,
                               (Vector2) {(rand() % 20) - 10, (rand() % 20) - 10},
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
            update_worm(&worms[i]);
            draw_worm(worms[i]);
        }
        EndDrawing();
    }

    CloseWindow();
}
