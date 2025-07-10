#include "Selector.h"
#include "bsp_oled.h"

void Selector_init(Selector* selector, Menu* menu) {
    selector->currentMenu = menu;
}

void Selector_move(Selector* selector, int direction) {
    Menu_moveSelector(selector->currentMenu, direction);
}

void Selector_render(Selector* selector) {
    Menu_render(selector->currentMenu);
}

void Selector_select(Selector* selector) {
    Item* selectedItem = Menu_getSelectedItem(selector->currentMenu);
    printf("Selected item: %s\n", selectedItem->name);
    if (selectedItem->subMenu != NULL) {
        printf("Submenu found for item: %s\n", selectedItem->name);
        mode = MODE3; // 切换到模式2
        selector->currentMenu = selectedItem->subMenu; // 进入子菜单
        printf("Entering submenu: %s\n", selectedItem->name);
        if(selectedItem->name == "时间"){
            mode =MODE4;
            return;
        }else if(selectedItem->name == "音量"){
            printf("进入音量调节模式\n");
            mode = MODE5;
        }else if(selectedItem->name == "药物"){
            printf("进入药物模式\n");
            mode = MODE7;
            return;
        }else if(selectedItem->name == "关于"){
            printf("进入关于\n");
            mode = MODE8;
        }
        Selector_render(&launcher.selector);  // 渲染菜单
    } 
    else {
        printf("No submenu for item: %s\n", selectedItem->name);
    }
}

void Selector_back(Selector* selector) {
    if(mode == MODE2){
        printf("返回主菜单\n");
        mode = MODE1; // 切换到模式1
        Launcher_render(&launcher);
        if(selector->currentMenu->parentMenu){
            selector->currentMenu = selector->currentMenu->parentMenu; // 返回父菜单
        }
        return;
    }
    if (selector->currentMenu->parentMenu != NULL) {
        printf("Back button pressed\n");
        mode = MODE1; // 切换到模式1
        selector->currentMenu = selector->currentMenu->parentMenu; // 返回父菜单
        printf("Returning to parent menu: %d\n", selector->currentMenu->itemCount);
        Launcher_render(&launcher);  // 渲染菜单
    }
    else {
        printf("Already at the top level menu\n");
    }
}