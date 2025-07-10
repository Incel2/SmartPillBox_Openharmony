#include "Menu.h"
#include "bsp_oled.h"
// #include "img.h"
#include "Item.h"
#include "systime_task.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define QUADRANT_WIDTH (SCREEN_WIDTH / 2)
#define QUADRANT_HEIGHT (SCREEN_HEIGHT / 2)

#define BAR_X 14          // 进度条的起始X坐标 (左上角X)
#define BAR_Y 32          // 进度条的起始Y坐标 (左上角Y)
#define BAR_WIDTH 100     // 进度条的宽度
#define BAR_HEIGHT 12     // 进度条的高度


uint8_t menu_array[8 * 48][64];

void Menu_init(Menu* menu,Menu* parentMenu) {
    menu->itemCount = 0;
    menu->currentIndex = 0;
    menu->parentMenu = parentMenu;  // 初始化父菜单指针为NULL
}


void Menu_addItem(Menu* menu, Item* item) {
    if (menu->itemCount < MAX_ITEMS) {
        menu->items[menu->itemCount++] = item;
    }
}

void Menu_render(Menu* menu) {
    oled_clear();  // 清屏
    // uint8_t x_position = 0;
    // uint8_t y_position = 16;
    for (uint8_t i = 0; i < menu->itemCount; i++) {
        bool isSelected = (i == menu->currentIndex);
        Item_render(menu->items[i], isSelected, menu->items[i]->x_position, menu->items[i]->y_position);
        // x_position += 40;  
    }    // oled_draw_32_BMP(110, 50, creeper); 
    // oled_draw_32_BMP(-20, -10, image_32x32_generated); 
    oled_refresh_gram();  // 刷新显示
}

void Menu_moveSelector(Menu* menu, int direction) {
    printf("Moving selector by %d\n ", direction);
    if (menu->currentIndex == 0 && direction < 0) {
        menu->currentIndex = menu->itemCount - 1;  // 循环到最后一项
        printf(" Current index: %d\n", menu->currentIndex);
    }else if (menu->currentIndex + direction >= menu->itemCount) {
        menu->currentIndex = 0;  // 循环到第一项
        printf("left Current index: %d\n", menu->currentIndex);
    }else{
        menu->currentIndex += direction;
    }
}

Item* Menu_getSelectedItem(Menu* menu) {
    return menu->items[menu->currentIndex];
}

void Menu_renderWithCamera(Menu* menu, Camera* camera) {
    // printf("Rendering menu with camera at position (%d, %d)\n", camera->x_position, camera->y_position);
    oled_clear(); 
    for (uint8_t i = 0; i < menu->itemCount; i++) {
        bool isSelected = (i == menu->currentIndex);
        
        if( menu->items[i]->x_position >= camera->x_position + camera->width || i >= 10) {
            break;
        }
        else if (menu->items[i]->x_position + 32 < camera->x_position) {
            continue;
        }
        // 只渲染在视口内的项
        
        if (menu->items[i]->x_position >= camera->x_position - 32 && menu->items[i]->x_position < camera->x_position + camera->width) {
            // printf("i:%d", i);
            Item_render(menu->items[i], isSelected, menu->items[i]->x_position - camera->x_position, menu->items[i]->y_position);
        }
        // printf("Rendering item %d at position (%d, %d)\n", i, menu->items[i]->x_position - camera->x_position, menu->items[i]->y_position);
        // printf("x_position: %d, y_position: %d\n", x_position, y_position);
    }
    oled_refresh_gram();  // 刷新显示
}

void renderTimeSubMenu(void) {

    oled_clear();
    char sysDataStr[20];
    sprintf(sysDataStr, "%04d %02d %02d", systemTime.year, systemTime.month, systemTime.day);
    char sysHourStr[2];
    sprintf(sysHourStr, "%02d", systemTime.hour);
    char sysMinStr[2];
    sprintf(sysMinStr, "%02d", systemTime.minute);
    char sysSecStr[2];
    sprintf(sysSecStr, "%02d", systemTime.second);
    oled_showstring_12(2, 52, sysDataStr, 12, 1);
    oled_show_digit_string(2, 16, sysHourStr, 32);
    oled_show_digit_string(45, 16, sysMinStr, 32);
    oled_show_digit_string(83, 24, sysSecStr, 24);
    oled_draw_bigpoint(41,25,1);
    oled_draw_bigpoint(41,37,1);
    oled_refresh_gram();
}

void renderSuccessMessage(void) {
    oled_clear();  // 清屏

    // 显示 Success! 字符串，字体大小为12，高亮显示
    oled_showstring_12(40, 28, "Success!", 12, 1);  // 居中大致在(32,28)位置

    oled_refresh_gram();  // 刷新显示缓存
}


void render_medicine_in_quadrant(int x_start, int y_start, const char* medicine_name, uint8_t is_enabled,uint8_t dose) {
    char name_buffer[11]; // 64像素宽，6像素/字符 -> ~10个字符。+1 for '\0'
    
    // 1. 准备药名字符串，确保不会超出区域宽度
    // %.10s 会截断过长的字符串为10个字符
    snprintf(name_buffer, sizeof(name_buffer), "%.10s", medicine_name);

    char display_buffer[32];  // 根据需要调整大小
    sprintf(display_buffer, "plz take %d:", dose);

    // 2. 准备状态字符串
    const char* status_str = (is_enabled == 1) ? "ON" : "OFF";
    if(is_enabled == 255){
        status_str = "N/A";
    }

    // 3. 计算显示坐标 (在区域内添加一些边距)
    int line1_y = y_start + 4;         // 第一行 y 坐标
    int line2_y = y_start + 18;        // 第二行 y 坐标 (12px字体高度 + 2px间距)
    int text_x = x_start + 4;          // 两行文字的 x 坐标

    // 4. 在屏幕上显示
    if(mode == MODE2){
        oled_showstring_12(text_x, line1_y, display_buffer, 12, 1);
        oled_showstring_12(text_x, line2_y, name_buffer, 12, 1);
    }else{
        oled_showstring_12(text_x, line1_y, name_buffer, 12, 1);
        oled_showstring_12(text_x, line2_y, status_str, 12, 1);  
    }
}

void renderRemindMessage() {
    oled_clear();  // 清屏

    oled_draw_hline(0, QUADRANT_HEIGHT - 1, SCREEN_WIDTH, 1);

    oled_draw_vline(QUADRANT_WIDTH - 1, 0, SCREEN_HEIGHT, 1);

    // 2. 在每个区域分别渲染药物信息
    // 区域 1: 左上角
    if(remind_medicine == medicine_list[0].name){
        render_medicine_in_quadrant(0, 0, medicine_list[0].name, medicine_list[0].is_enabled, medicine_list[0].dose);
    }    
    // 区域 2: 右上角
    else if(remind_medicine == medicine_list[1].name){
        render_medicine_in_quadrant(QUADRANT_WIDTH, 0, medicine_list[1].name, medicine_list[1].is_enabled, medicine_list[1].dose);
    }

    // 区域 3: 左下角
    else if(remind_medicine == medicine_list[2].name){
        render_medicine_in_quadrant(0, QUADRANT_HEIGHT, medicine_list[2].name, medicine_list[2].is_enabled, medicine_list[2].dose);
    }

    // 区域 4: 右下角
    else if(remind_medicine == medicine_list[3].name){
        render_medicine_in_quadrant(QUADRANT_WIDTH, QUADRANT_HEIGHT, medicine_list[3].name, medicine_list[3].is_enabled, medicine_list[3].dose);
    }

    oled_refresh_gram();  // 刷新显示缓存
}

/**
 * @brief 渲染四种药物信息到屏幕的四个区域
 */
void renderAllMedicines() 
{
    oled_clear();  // 清屏

    // 1. 画出分割线来明确划分区域 (可选，但效果好)
    // 画一条横贯屏幕中间的水平线
    // oled_drawline(0, QUADRANT_HEIGHT - 1, SCREEN_WIDTH - 1, QUADRANT_HEIGHT - 1, 1);
    oled_draw_hline(0, QUADRANT_HEIGHT - 1, SCREEN_WIDTH, 1);

    // 画一条纵贯屏幕中间的垂直线
    // oled_drawline(QUADRANT_WIDTH - 1, 0, QUADRANT_WIDTH - 1, SCREEN_HEIGHT - 1,1);
    // 画一条纵贯屏幕中间的垂直线
    oled_draw_vline(QUADRANT_WIDTH - 1, 0, SCREEN_HEIGHT, 1);

    // 2. 在每个区域分别渲染药物信息
    // 区域 1: 左上角
    render_medicine_in_quadrant(0, 0, medicine_list[0].name, medicine_list[0].is_enabled, medicine_list[0].dose);
    
    // 区域 2: 右上角
    render_medicine_in_quadrant(QUADRANT_WIDTH, 0, medicine_list[1].name, medicine_list[1].is_enabled, medicine_list[1].dose);

    // 区域 3: 左下角
    render_medicine_in_quadrant(0, QUADRANT_HEIGHT, medicine_list[2].name, medicine_list[2].is_enabled, medicine_list[2].dose);

    // 区域 4: 右下角
    render_medicine_in_quadrant(QUADRANT_WIDTH, QUADRANT_HEIGHT, medicine_list[3].name, medicine_list[3].is_enabled, medicine_list[3].dose);

    oled_refresh_gram();  // 刷新显示缓存
}

void renderVolumeMenu(uint8_t current_volume) {
    char volume_text[8]; // 用于存放 "100%" 这样的字符串

    oled_clear(); // 清屏

    // 1. 在屏幕顶部居中绘制标题 "Volume"
    const char* title = "Volume";
    uint8_t title_char_width = 8; // 假设16号字体，每个英文字符宽度为8像素
    uint8_t title_display_width = strlen(title) * title_char_width;
    oled_showstring((128 - title_display_width) / 2, 8, title, 16, 1);

    // 2. 绘制进度条的外框
    // 注意：oled_drawRectangle 参数是 x1, y1, x2, y2
    // x2 = BAR_X + BAR_WIDTH - 1
    // y2 = BAR_Y + BAR_HEIGHT - 1
    oled_drawRectangle(BAR_X, BAR_Y, BAR_X + BAR_WIDTH - 1, BAR_Y + BAR_HEIGHT - 1, 1); // 颜色为1 (通常表示前景，如白色)
    
    // 3. 计算并绘制进度条内部的填充部分
    if (current_volume > 0) {
        // 进度条内部左右各留2像素边距，所以可用填充宽度为 BAR_WIDTH - 4
        // 将音量值 (0-100) 映射到进度条的内部像素宽度
        uint8_t filled_pixel_width = (uint8_t)((float)current_volume * (BAR_WIDTH - 4) / 100.0f);
        
        if (filled_pixel_width > 0) {
            // 填充区域的起始坐标 (x1, y1) 在外框基础上内缩2像素
            uint8_t fill_x1 = BAR_X + 2;
            uint8_t fill_y1 = BAR_Y + 2;
            // 填充区域的结束坐标 (x2, y2)
            // x2 = fill_x1 + filled_pixel_width - 1
            // y2 = fill_y1 + (BAR_HEIGHT - 4) - 1
            oled_fill(fill_x1, fill_y1, 
                      fill_x1 + filled_pixel_width - 1, 
                      fill_y1 + (BAR_HEIGHT - 4) - 1, 1); // 颜色为0 (通常表示反色或填充色)
        }
    }

    // 4. 创建并显示进度条下方的百分比文字
    sprintf(volume_text, "%d%%", current_volume);
    uint8_t text_display_width = strlen(volume_text) * title_char_width;
    oled_showstring((128 - text_display_width) / 2, BAR_Y + BAR_HEIGHT + 4, volume_text, 16, 1);

    // 5. 添加一个选中时的高亮效果 (如果 isSelected 为真)

    oled_refresh_gram(); // 刷新屏幕显示
}

void renderBrightnessMenu(uint8_t current_volume) {
    char volume_text[8]; // 用于存放 "100%" 这样的字符串

    oled_clear(); // 清屏

    // 1. 在屏幕顶部居中绘制标题 "Volume"
    const char* title = "Brightness";
    uint8_t title_char_width = 8; // 假设16号字体，每个英文字符宽度为8像素
    uint8_t title_display_width = strlen(title) * title_char_width;
    oled_showstring((128 - title_display_width) / 2, 8, title, 16, 1);

    // 2. 绘制进度条的外框
    // 注意：oled_drawRectangle 参数是 x1, y1, x2, y2
    // x2 = BAR_X + BAR_WIDTH - 1
    // y2 = BAR_Y + BAR_HEIGHT - 1
    oled_drawRectangle(BAR_X, BAR_Y, BAR_X + BAR_WIDTH - 1, BAR_Y + BAR_HEIGHT - 1, 1); // 颜色为1 (通常表示前景，如白色)
    
    // 3. 计算并绘制进度条内部的填充部分
    if (current_volume > 0) {
        // 进度条内部左右各留2像素边距，所以可用填充宽度为 BAR_WIDTH - 4
        // 将音量值 (0-100) 映射到进度条的内部像素宽度
        uint8_t filled_pixel_width = (uint8_t)((float)current_volume * (BAR_WIDTH - 4) / 255.0f);
        
        if (filled_pixel_width > 0) {
            // 填充区域的起始坐标 (x1, y1) 在外框基础上内缩2像素
            uint8_t fill_x1 = BAR_X + 2;
            uint8_t fill_y1 = BAR_Y + 2;
            // 填充区域的结束坐标 (x2, y2)
            // x2 = fill_x1 + filled_pixel_width - 1
            // y2 = fill_y1 + (BAR_HEIGHT - 4) - 1
            oled_fill(fill_x1, fill_y1, 
                      fill_x1 + filled_pixel_width - 1, 
                      fill_y1 + (BAR_HEIGHT - 4) - 1, 1); // 颜色为0 (通常表示反色或填充色)
        }
    }

    // 4. 创建并显示进度条下方的百分比文字
    sprintf(volume_text, "%d%%", current_volume);
    uint8_t text_display_width = strlen(volume_text) * title_char_width;
    oled_showstring((128 - text_display_width) / 2, BAR_Y + BAR_HEIGHT + 4, volume_text, 16, 1);

    // 5. 添加一个选中时的高亮效果 (如果 isSelected 为真)

    oled_refresh_gram(); // 刷新屏幕显示
}