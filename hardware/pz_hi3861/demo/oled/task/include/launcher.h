// Launcher.h
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "Camera.h"
#include "Menu.h"
#include "Selector.h"

typedef struct {
    Camera camera;
    Menu menu;
    Selector selector;
} Launcher;

void Launcher_init(Launcher* launcher);
void Launcher_render(Launcher* launcher);
void Launcher_moveMenu(Launcher* launcher, uint8_t currentIndex, uint8_t previousIndex);

#endif // LAUNCHER_H
