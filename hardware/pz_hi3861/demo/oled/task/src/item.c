#include "item.h"
#include "bsp_oled.h"

void Item_init(Item* item, const char* name, uint8_t id, const uint8_t* bmp_data, Menu* subMenu, uint8_t x_position, uint8_t y_position) {
    item->name = name;
    item->id = id;
    item->bmp_data = bmp_data;
    item->x_position = x_position;
    item->y_position = y_position;  // 假设初始位置为 (8, 16)
    item->subMenu = subMenu;
}

void Item_render(Item* item, bool isSelected, uint8_t x_position, uint8_t y_position) {
    if (isSelected) { // 高亮显示
        if(item->bmp_data == NULL) {
            oled_showstring(x_position, y_position, item->name, 16, 0); // 0表示高亮显示
            return; // 如果没有BMP数据，则不渲染
        }
       
        oled_draw_32_BMP(x_position, y_position, item->bmp_data);
        // 这里可以添加高亮效果，比如改变颜色或添加边框
        
        oled_drawRectangle(x_position-3, 8 - 3, x_position + 32 + 2, 8 + 32 + 2 , 1); 
        oled_drawRectangle(x_position-4, 8 -4, x_position + 32 + 3, 8 + 32 + 3 , 1); 
        oled_drawRectangle(x_position-4, 18, x_position - 3, 36 , 0); 
        oled_drawRectangle(x_position + 32 + 2, 18, x_position + 32 + 3, 34 , 0); 
        oled_drawRectangle(x_position + 10, 8 - 4, x_position + 21 , 8 - 3 , 0);
        oled_drawRectangle(x_position + 10, 8 + 32 + 2, x_position + 21 , 8 + 32 + 3 , 0);
        oled_showstring(x_position , y_position + 16, item->name, 16, 1); // 高亮显示时，使用0表示高亮


    } else {
        // 普通显示
        if(item->bmp_data == NULL) {
            oled_showstring(x_position, y_position, item->name, 16, 1); // 1表示普通显示
            return; // 如果没有BMP数据，则不渲染
        }
        oled_draw_32_BMP(x_position, y_position, item->bmp_data);
        // oled_drawRectangle(x_position-2, y_position + 2, x_position + 32 + 2, y_position + 32 + 2 + 4, 0); 
    }
}
