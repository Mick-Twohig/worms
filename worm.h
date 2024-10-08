#ifndef WORMS_WORM_H
#define WORMS_WORM_H

#include <stdlib.h>
#include <raylib.h>

typedef enum _state {
    AT_REST, MOVING
} worm_state;

enum wall {
    left, top, right, bottom
} wall;


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
    float direction;
    float speed;
    Color body_color;
    worm_state worm_state;
} Worm;

void print_segment_data(Segment);
void print_worm_data(Worm);
void draw_segment(Segment);
Worm create_worm(Vector2 position, size_t num_segments, size_t intersegment_space, float direction, float speed, Color color);
void draw_worm(Worm);
void update_worm(Worm *, Vector2);
#endif
