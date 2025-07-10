// Camera.c
#include "Camera.h"
#include "task_define.h"

void Camera_init(Camera* camera, int width, int height) {
    camera->x_position = 0;
    camera->y_position = 0;
    camera->width = width;
    camera->height = height;
}

void Camera_move(Camera* camera, int64_t x_offset, int64_t y_offset) {
    camera->x_position += x_offset;
    camera->y_position += y_offset;
    printf("Camera move(%d)\n", x_offset);
    printf("x_position: %ld\n", camera->x_position);
    printf("y_position: %ld\n", camera->y_position);
    // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
    // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
}

