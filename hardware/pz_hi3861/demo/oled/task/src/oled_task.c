#include "hi_io.h"
#include "hi_gpio.h"
#include "bsp_key.h"
#include "bsp_oled.h"
#include "Item.h"
#include "Menu.h"
#include "Selector.h"
#include "img.h"
#include "launcher.h"
#include "task_define.h"

// 菜单项
Item timeDisplayItem;
Item brightnessItem;
Item volumeItem;
Item medicineItem;
Item infoItem;
Item brightnessItem2;
Item brightnessup;
Item brightnessdown;
Item volumeup;
Item volumedown;
Item time;

Item medicine1;
Item medicine2;
Item medicine3;
Item medicine4;

Item name;
Item version;
Item teamname;

Menu menu;
Menu subMenu1;
Menu subMenu2;
Menu subMenu3;
Menu subMenu4;
Menu subMenu5;

Selector selector;

char medicine1_str[17];
char medicine2_str[17];
char medicine3_str[17];
char medicine4_str[17];

// 拼接药物名称和状态的函数
void create_display_string(char* display_string, const char* name, uint8_t is_enabled) {
    // 使用 snprintf 将药物名称和状态拼接为一个 16 个字符的字符串
    if(is_enabled==255){
        return;
    }
    snprintf(display_string, 17, "%-12s %s", name, is_enabled==1 ? "ON" : "OFF");
}

void REFRESH_Task(void) {
    // 刷新 OLED 显示
    while (1)
    {
    switch (mode) {
        case MODE2:
            // printf("time submenu\n");
            renderRemindMessage(remind_medicine);
            usleep(100);
            break;
        case MODE3:
            renderBrightnessMenu(brightness);
            break;
        case MODE4:
            // printf("time submenu\n");
            renderTimeSubMenu();
            break;
        case MODE5:
            renderVolumeMenu(volume);
            break;
        case MODE6:
            renderSuccessMessage();
            usleep(1000000);
            osSemaphoreRelease(SEMAPHORE_OUT);
            // mode = MODE1;
            // Launcher_render(&launcher);
            break; 
        case MODE7:
            renderAllMedicines();
            break; 
        default:
            // printf("default menu\n");
            // Launcher_render(&launcher);
            break;
    }
    usleep(10000);  // 每10ms刷新一次
    }
    
    
}

void OLED_Task(void) {
    //Launcher launcher;
    Launcher_init(&launcher);

    // 初始化子菜单
    Menu_init(&subMenu1,&launcher.menu);
    Menu_init(&subMenu2,&launcher.menu);
    Menu_init(&subMenu3,&launcher.menu);
    Menu_init(&subMenu4,&launcher.menu);
    Menu_init(&subMenu5,&launcher.menu);

    Item_init(&brightnessup, "Brightness Up", 0, NULL, NULL,0,0);
    Item_init(&brightnessdown, "Brightness Down", 1, NULL, NULL,0,16);
    Item_init(&volumeup, "Volume Up", 0, NULL, NULL,0,0);
    Item_init(&volumedown, "Volume Down", 1, NULL, NULL,0,16);

    create_display_string(medicine1_str, medicine_list[0].name, medicine_list[0].is_enabled);
    create_display_string(medicine2_str, medicine_list[1].name, medicine_list[1].is_enabled);
    create_display_string(medicine3_str, medicine_list[2].name, medicine_list[2].is_enabled);
    create_display_string(medicine4_str, medicine_list[3].name, medicine_list[3].is_enabled);

    Item_init(&medicine1, medicine1_str, 0, NULL, NULL,0,0);
    Item_init(&medicine2, medicine2_str, 1, NULL, NULL,0,16);
    Item_init(&medicine3, medicine3_str, 2, NULL, NULL,0,32);
    Item_init(&medicine4, medicine4_str, 1, NULL, NULL,0,48);

    Item_init(&name, "Smart Pill Box", 0, NULL, NULL,0,0);
    Item_init(&version, "V1.2.0", 1, NULL, NULL,0,16);
    Item_init(&teamname, "team:XD", 2, NULL, NULL,0,32);

    snprintf(timeStr, sizeof(timeStr), "00:00:00");  // 初始化时间字符串
    Item_init(&time, timeStr, 0, NULL, NULL,0,32);

    Menu_addItem(&subMenu2, &brightnessup);
    Menu_addItem(&subMenu2, &brightnessdown);
    Menu_addItem(&subMenu3, &volumeup);
    Menu_addItem(&subMenu3, &volumedown);

    Menu_addItem(&subMenu4, &medicine1);
    Menu_addItem(&subMenu4, &medicine2);
    Menu_addItem(&subMenu4, &medicine3);
    Menu_addItem(&subMenu4, &medicine4);

    Menu_addItem(&subMenu5, &name);
    Menu_addItem(&subMenu5, &version);
    Menu_addItem(&subMenu5, &teamname);

    Menu_addItem(&subMenu1, &time);
    // 初始化菜单项
    Item_init(&timeDisplayItem, "时间", 0, clock_icon,&subMenu1,48 + 0 * 40,24);
    Item_init(&brightnessItem, "亮度", 1, light_icon,&subMenu2,48 + 1 * 40,24);  
    Item_init(&volumeItem, "音量", 2, speaker_icon,&subMenu3,48 + 2 * 40,24);
    Item_init(&medicineItem, "药物", 3, medicine_icon,&subMenu4,48 + 3 * 40,24);  
    Item_init(&infoItem, "关于", 4, creeper,&subMenu5,48 + 4 * 40,24);
    // Item_init(&brightnessItem2, "亮度", 5, creeper,NULL,48 + 5 * 40, 24);      
    // 添加菜单项    
    // 添加菜单项
    Menu_addItem(&launcher.menu, &timeDisplayItem);
    Menu_addItem(&launcher.menu, &brightnessItem);
    Menu_addItem(&launcher.menu, &volumeItem);
    Menu_addItem(&launcher.menu, &medicineItem);
    Menu_addItem(&launcher.menu, &infoItem);
    // Menu_addItem(&launcher.menu, &brightnessItem2);
    oled_init();  // 初始化 OLED

    Launcher_render(&launcher);  // 渲染菜单

    while (1) {
        // 渲染菜单
        // Launcher_render(&launcher);  // 渲染菜单
        // printf("key perperation\n");
        // printf("key1: %d\n", osSemaphoreGetCount(SEMAPHORE_KEY1));
        // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
        // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);

        if (osSemaphoreAcquire(SEMAPHORE_KEY1,0)==osOK) {
            // printf("step1 \n");
            uint8_t previousIndex = Menu_getSelectedItem(&launcher.menu)->id;
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step2 \n");
            Menu_moveSelector(&launcher.menu, -1); 
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step3 \n"); 
            uint8_t currentIndex = Menu_getSelectedItem(&launcher.menu)->id; 
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step4 \n");
            Launcher_moveMenu(&launcher, currentIndex, previousIndex);  
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step5 \n");
            // printf("1111");

        } else if (osSemaphoreAcquire(SEMAPHORE_KEY2,0)==osOK) {
            // printf("step1 \n");
            uint8_t previousIndex = Menu_getSelectedItem(&launcher.menu)->id;
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step2 \n");
            Menu_moveSelector(&launcher.menu, 1); 
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step3 \n"); 
            uint8_t currentIndex = Menu_getSelectedItem(&launcher.menu)->id; 
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step4 \n");
            Launcher_moveMenu(&launcher, currentIndex, previousIndex);  
            // printf("SEMAPHORE_KEY1 addr: %p\n", SEMAPHORE_KEY1);
            // printf("SEMAPHORE_KEY2 addr: %p\n", SEMAPHORE_KEY2);
            // printf("step5 \n");
        }else if(osSemaphoreAcquire(SEMAPHORE_IN,0)==osOK) {
            Selector_select(&launcher.selector);  // 选择当前菜单项
            printf("rendering current menu \n");
        }else if(osSemaphoreAcquire(SEMAPHORE_OUT,0)==osOK) {
            Selector_back(&launcher.selector);  // 选择当前菜单项
            printf("rendering current menu \n");
        }
        // else{
        //     Launcher_render(&launcher);  // 渲染菜单
        // }
        
        usleep(1000);  // 延时，避免按键抖动
        
     }
}
