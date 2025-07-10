#ifndef MENU_H
#define MENU_H


//#include "Item.h"
#include "Camera.h"

#define MAX_ITEMS 10

// struct Item; // 前向声明，避免循环包含
struct Menu; // 前向声明，避免循环包含

typedef struct Item{
    const char* name;  // 菜单项名称
    uint8_t id;        // 菜单项ID
    const uint8_t* bmp_data;  // 菜单项对应的BMP图片数据
    struct Menu* subMenu;// 所属菜单指针
    uint8_t x_position;  // 菜单项的水平位置
    uint8_t y_position;  // 菜单项的垂直位置
    // uint8_t _reserved; // 填充，确保对齐
    
} Item;

typedef struct Menu{
    Item* items[MAX_ITEMS]; // 菜单项数组
    uint8_t itemCount;      // 菜单项数量
    uint8_t currentIndex;   // 当前选中的菜单项
    uint8_t _reserved[2]; // 手动补齐
    struct Menu* parentMenu; // 父菜单指针
} Menu;

void Menu_init(Menu* menu, Menu* parentMenu);
void Menu_addItem(Menu* menu,Item* item);
void Menu_render(Menu* menu);
void Menu_moveSelector(Menu* menu, int direction);
Item* Menu_getSelectedItem(Menu* menu);
void Menu_renderWithCamera(Menu* menu, Camera* camera);
void renderTimeSubMenu(void);
void renderSuccessMessage(void);
void renderRemindMessage();
void renderAllMedicines();
void renderVolumeMenu(uint8_t current_volume);
void renderBrightnessMenu(uint8_t current_volume);

#endif // MENU_H