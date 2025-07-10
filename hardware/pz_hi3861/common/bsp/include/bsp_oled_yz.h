/**
 ****************************************************************************************************
 * @file        bsp_oled.h
 * @author      普中科技
 * @version     V1.3
 * @date        2025-05-18
 * @brief       OLED显示库头文件，定义宏、类型和函数原型（适配Hi3861，支持高级绘图和中英文显示）
 * @license     Copyright (c) 2024-2034, 深圳市普中科技有限公司
 * @note        更新说明：
 *              - V1.3: 合并OLED.c功能，新增高级绘图（线、圆、矩形等）和区域显示功能
 *              - 支持中英文混合显示，兼容bsp_oled.c原有函数
 *              - 统一函数命名和注释格式
 *              - 原功能基于b站UP主江协科技的OLED.c，感谢其贡献
 ****************************************************************************************************
 */

#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#include <stdint.h>
#include <stdarg.h>

/**
 * @defgroup OLED_Constants OLED常量定义
 * @{
 */

/** OLED屏幕宽度（像素） */
#define OLED_WIDTH 128

/** OLED屏幕高度（像素） */
#define OLED_HEIGHT 64

/** 最大字符串长度 */
#define MAX_STRING_LENGTH 100

/** 汉字字符宽度（字节数，GB2312编码） */
#define OLED_CHN_CHAR_WIDTH 2

/** 字体大小定义（ASCII字符） */
#define OLED_FONT_12 12  // 6x12像素
#define OLED_FONT_16 16  // 8x16像素
#define OLED_FONT_24 24  // 12x24像素

/** 字体大小定义（汉字，兼容OLED.c） */
#define OLED_8X8_FULL 8    // 8x8像素汉字
#define OLED_12X12_FULL 12 // 12x12像素汉字
#define OLED_16X16_FULL 16 // 16x16像素汉字
#define OLED_20X20_FULL 20 // 20x20像素汉字

/** 填充模式 */
#define OLED_FILLED 1   // 填充
#define OLED_UNFILLED 0 // 不填充

/** @} */

/**
 * @defgroup OLED_Functions OLED功能函数
 * @{
 */

/**
 * @brief 初始化OLED显示屏
 * @note 配置I2C/SPI接口，初始化SSD1306控制器，清空显存
 */
void oled_init(void);

/**
 * @brief 清空显存
 * @note 调用后需调用 oled_refresh_gram() 更新显示
 */
void oled_clear(void);

/**
 * @brief 清空指定区域
 * @param x 区域左上角X坐标
 * @param y 区域左上角Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void oled_clear_area(int16_t x, int16_t y, int16_t width, int16_t height);

/**
 * @brief 反转整个显存（黑白反显）
 */
void oled_reverse(void);

/**
 * @brief 反转指定区域
 * @param x 区域左上角X坐标
 * @param y 区域左上角Y坐标
 * @param width 区域宽度
 * @param height 区域高度
 */
void oled_reverse_area(int16_t x, int16_t y, int16_t width, int16_t height);

/**
 * @brief 显示图片
 * @param x 图片左上角X坐标
 * @param y 图片左上角Y坐标
 * @param width 图片宽度
 * @param height 图片高度
 * @param image 图片数据（8位垂直排列）
 */
void oled_show_image(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t *image);

/**
 * @brief 显示ASCII字符
 * @param x 字符左上角X坐标
 * @param y 字符左上角Y坐标
 * @param chr 字符
 * @param size 字体大小（OLED_FONT_12/16/24）
 */
void oled_show_char(int16_t x, int16_t y, char chr, uint8_t size);

/**
 * @brief 显示十进制正整数
 * @param x 数字左上角X坐标
 * @param y 数字左上角Y坐标
 * @param num 数字
 * @param len 数字长度
 * @param size 字体大小
 */
void oled_show_num(int16_t x, int16_t y, uint32_t num, uint8_t len, uint8_t size);

/**
 * @brief 显示有符号数字
 * @param x 数字左上角X坐标
 * @param y 数字左上角Y坐标
 * @param num 数字
 * @param len 数字长度
 * @param size 字体大小
 */
void oled_show_signed_num(int16_t x, int16_t y, int32_t num, uint8_t len, uint8_t size);

/**
 * @brief 显示十六进制数字
 * @param x 数字左上角X坐标
 * @param y 数字左上角Y坐标
 * @param num 数字
 * @param len 数字长度
 * @param size 字体大小
 */
void oled_show_hex_num(int16_t x, int16_t y, uint32_t num, uint8_t len, uint8_t size);

/**
 * @brief 显示二进制数字
 * @param x 数字左上角X坐标
 * @param y 数字左上角Y坐标
 * @param num 数字
 * @param len 数字长度
 * @param size 字体大小
 */
void oled_show_bin_num(int16_t x, int16_t y, uint32_t num, uint8_t len, uint8_t size);

/**
 * @brief 显示浮点数
 * @param x 数字左上角X坐标
 * @param y 数字左上角Y坐标
 * @param num 数字
 * @param int_len 整数部分长度
 * @param fra_len 小数部分长度
 * @param size 字体大小
 */
void oled_show_float_num(int16_t x, int16_t y, double num, uint8_t int_len, uint8_t fra_len, uint8_t size);

/**
 * @brief 显示字符串
 * @param x 字符串左上角X坐标
 * @param y 字符串左上角Y坐标
 * @param str 字符串
 * @param size 字体大小
 */
void oled_show_string(int16_t x, int16_t y, const char *str, uint8_t size);

/**
 * @brief 显示汉字
 * @param x 汉字左上角X坐标
 * @param y 汉字左上角Y坐标
 * @param chinese 汉字字符串（GB2312编码）
 * @param size 字体大小（OLED_8X8_FULL/12X12_FULL/16X16_FULL/20X20_FULL）
 */
void oled_show_chinese(int16_t x, int16_t y, const char *chinese, uint8_t size);

/**
 * @brief 显示中英文混合字符串
 * @param x 字符串左上角X坐标
 * @param y 字符串左上角Y坐标
 * @param str 字符串
 * @param chinese_size 汉字字体大小
 * @param ascii_size ASCII字体大小
 */
void oled_show_mix_string(int16_t x, int16_t y, const char *str, uint8_t chinese_size, uint8_t ascii_size);

/**
 * @brief 格式化显示中英文混合字符串
 * @param x 字符串左上角X坐标
 * @param y 字符串左上角Y坐标
 * @param chinese_size 汉字字体大小
 * @param ascii_size ASCII字体大小
 * @param format 格式化字符串
 * @param ... 参数列表
 */
void oled_printf_mix(int16_t x, int16_t y, uint8_t chinese_size, uint8_t ascii_size, const char *format, ...);

/**
 * @brief 在指定区域显示图片
 * @param x_pic 图片X坐标
 * @param y_pic 图片Y坐标
 * @param pic_width 图片宽度
 * @param pic_height 图片高度
 * @param x_area 区域X坐标
 * @param y_area 区域Y坐标
 * @param area_width 区域宽度
 * @param area_height 区域高度
 * @param image 图片数据
 */
void oled_show_image_area(int16_t x_pic, int16_t y_pic, int16_t pic_width, int16_t pic_height,
                          int16_t x_area, int16_t y_area, int16_t area_width, int16_t area_height, const uint8_t *image);

/**
 * @brief 在指定区域显示字符
 * @param range_x 区域X坐标
 * @param range_y 区域Y坐标
 * @param range_width 区域宽度
 * @param range_height 区域高度
 * @param x 字符X坐标
 * @param y 字符Y坐标
 * @param chr 字符
 * @param size 字体大小
 */
void oled_show_char_area(int16_t range_x, int16_t range_y, int16_t range_width, int16_t range_height,
                         int16_t x, int16_t y, char chr, uint8_t size);

/**
 * @brief 在指定区域显示字符串
 * @param range_x 区域X坐标
 * @param range_y 区域Y坐标
 * @param range_width 区域宽度
 * @param range_height 区域高度
 * @param x 字符串X坐标
 * @param y 字符串Y坐标
 * @param str 字符串
 * @param size 字体大小
 */
void oled_show_string_area(int16_t range_x, int16_t range_y, int16_t range_width, int16_t range_height,
                           int16_t x, int16_t y, const char *str, uint8_t size);

/**
 * @brief 在指定区域显示汉字
 * @param range_x 区域X坐标
 * @param range_y 区域Y坐标
 * @param range_width 区域宽度
 * @param range_height 区域高度
 * @param x 汉字X坐标
 * @param y 汉字Y坐标
 * @param chinese 汉字字符串
 * @param size 字体大小
 */
void oled_show_chinese_area(int16_t range_x, int16_t range_y, int16_t range_width, int16_t range_height,
                            int16_t x, int16_t y, const char *chinese, uint8_t size);

/**
 * @brief 在指定区域显示中英文混合字符串
 * @param range_x 区域X坐标
 * @param range_y 区域Y坐标
 * @param range_width 区域宽度
 * @param range_height 区域高度
 * @param x 字符串X坐标
 * @param y 字符串Y坐标
 * @param str 字符串
 * @param chinese_size 汉字字体大小
 * @param ascii_size ASCII字体大小
 */
void oled_show_mix_string_area(int16_t range_x, int16_t range_y, int16_t range_width, int16_t range_height,
                               int16_t x, int16_t y, const char *str, uint8_t chinese_size, uint8_t ascii_size);

/**
 * @brief 在指定区域格式化显示中英文混合字符串
 * @param range_x 区域X坐标
 * @param range_y 区域Y坐标
 * @param range_width 区域宽度
 * @param range_height 区域高度
 * @param x 字符串X坐标
 * @param y 字符串Y坐标
 * @param chinese_size 汉字字体大小
 * @param ascii_size ASCII字体大小
 * @param format 格式化字符串
 * @param ... 参数列表
 */
void oled_printf_mix_area(int16_t range_x, int16_t range_y, int16_t range_width, int16_t range_height,
                         int16_t x, int16_t y, uint8_t chinese_size, uint8_t ascii_size, const char *format, ...);

/**
 * @brief 在指定位置画一个点
 * @param x 点X坐标
 * @param y 点Y坐标
 */
void oled_draw_point(int16_t x, int16_t y);

/**
 * @brief 获取指定位置点的值
 * @param x 点X坐标
 * @param y 点Y坐标
 * @return 1: 点亮, 0: 熄灭
 */
uint8_t oled_get_point(uint8_t x, uint8_t y);

/**
 * @brief 画线
 * @param x0 起点X坐标
 * @param y0 起点Y坐标
 * @param x1 终点X坐标
 * @param y1 终点Y坐标
 */
void oled_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

/**
 * @brief 画矩形
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param width 宽度
 * @param height 高度
 * @param is_filled 是否填充（OLED_FILLED/OLED_UNFILLED）
 */
void oled_draw_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t is_filled);

/**
 * @brief 画三角形
 * @param x0 顶点1 X坐标
 * @param y0 顶点1 Y坐标
 * @param x1 顶点2 X坐标
 * @param y1 顶点2 Y坐标
 * @param x2 顶点3 X坐标
 * @param y2 顶点3 Y坐标
 * @param is_filled 是否填充
 */
void oled_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t is_filled);

/**
 * @brief 画圆
 * @param x 圆心X坐标
 * @param y 圆心Y坐标
 * @param radius 半径
 * @param is_filled 是否填充
 */
void oled_draw_circle(int16_t x, int16_t y, int16_t radius, uint8_t is_filled);

/**
 * @brief 画椭圆
 * @param x 中心X坐标
 * @param y 中心Y坐标
 * @param a 横向半轴长度
 * @param b 纵向半轴长度
 * @param is_filled 是否填充
 */
void oled_draw_ellipse(int16_t x, int16_t y, int16_t a, int16_t b, uint8_t is_filled);

/**
 * @brief 画圆弧
 * @param x 圆心X坐标
 * @param y 圆心Y坐标
 * @param radius 半径
 * @param start_angle 起始角度（-180~180）
 * @param end_angle 终止角度（-180~180）
 * @param is_filled 是否填充
 */
void oled_draw_arc(int16_t x, int16_t y, int16_t radius, int16_t start_angle, int16_t end_angle, uint8_t is_filled);

/**
 * @brief 画圆角矩形
 * @param x 左上角X坐标
 * @param y 左上角Y坐标
 * @param width 宽度
 * @param height 高度
 * @param radius 圆角半径
 * @param is_filled 是否填充
 */
void oled_draw_rounded_rectangle(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius, uint8_t is_filled);

/**
 * @brief 刷新显存到OLED硬件
 * @note 所有显示函数操作显存后需调用此函数更新显示
 */
void oled_refresh_gram(void);

/**
 * @brief 向OLED发送命令
 * @param cmd 命令字节
 * @note 底层函数，由oled_init()等调用
 */
void oled_write_cmd(uint8_t cmd);

/** @} */

#endif /* __BSP_OLED_H */