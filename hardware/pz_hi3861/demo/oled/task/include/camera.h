// Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
typedef struct {
    int64_t x_position;  // 水平位置
    int64_t y_position;  // 垂直位置
    int width;       // 视口的宽度
    int height;      // 视口的高度
} Camera;

void Camera_init(Camera* camera, int width, int height);
void Camera_move(Camera* camera, int64_t x_offset, int64_t y_offset);
// void Camera_animateTo(Camera* camera, int target_x, int target_y, int speed);

#endif // CAMERA_H
