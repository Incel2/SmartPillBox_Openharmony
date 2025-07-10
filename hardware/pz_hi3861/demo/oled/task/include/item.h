#ifndef ITEM_H
#define ITEM_H


#include "menu.h"  // 引入Menu结构体定义
//#include "task_define.h"

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
#define bool _Bool


//#include"menu.h"

struct Item; // 前向声明，避免循环包含
void Item_init(Item* item, const char* name, uint8_t id, const uint8_t* bmp_data, Menu* subMenu, uint8_t x_position, uint8_t y_position);                                          // 初始化Item
void Item_render(Item* item, bool isSelected, uint8_t x_position, uint8_t y_position);             // 渲染菜单项

#endif // ITEM_H