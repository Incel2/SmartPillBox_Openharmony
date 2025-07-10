#ifndef SELECTOR_H
#define SELECTOR_H

#include "menu.h"

typedef struct {
    Menu* currentMenu;
} Selector;

void Selector_init(Selector* selector, Menu* menu);
void Selector_move(Selector* selector, int direction);  // 移动选择器
void Selector_render(Selector* selector);              // 渲染选择器
void Selector_select(Selector* selector);              // 选择当前菜单项
void Selector_back(Selector* selector);                // 返回上一级菜单

#endif // SELECTOR_H
