/**
 ****************************************************************************************************
 * @file        bsp_oled.c
 * @author      普中科技
 * @version     V1.0
 * @date        2024-06-05
 * @brief       OLED液晶显示实验
 * @license     Copyright (c) 2024-2034, 深圳市普中科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:普中-Hi3861
 * 在线视频:https://space.bilibili.com/2146492485
 * 公司网址:www.prechin.cn
 * 购买地址:
 *
 */

#include "bsp_oled.h"
#include <unistd.h>
#include "hi_time.h"
#include "hi_i2c.h"
#include "oledfont.h" 


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
static uint8_t OLED_GRAM[128][8];

//IIC发送数据
uint32_t iic_senddata(uint8_t *data, size_t size)
{
    hi_i2c_data i2cData = {0};
    i2cData.send_buf = data;
    i2cData.send_len = size;

    return hi_i2c_write(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, &i2cData);
}

//IIC写命令
uint32_t write_iic_cmd(uint8_t cmd)
{
	uint8_t buffer[] = {0x00, cmd};
    return iic_senddata(buffer, sizeof(buffer));
}

//IIC写数据
uint32_t write_iic_data(uint8_t dat)
{
	uint8_t buffer[] = {0x40, dat};
    return iic_senddata(buffer, sizeof(buffer));
}

//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void oled_wr_byte(uint8_t dat,uint8_t cmd)
{
	if(cmd)
		write_iic_data(dat);
	else
		write_iic_cmd(dat);
}

//更新显存到LCD		 
void oled_refresh_gram(void)
{
    uint8_t buffer[1025]; // 1控制字节 + 1024数据字节
    hi_i2c_data i2cData;

    // 设置水平寻址模式和地址范围
    uint8_t cmd[] = {
        0x00,       // 控制字节：命令
        0x21, 0x00, 0x7F, // 列地址范围：0~127
        0x22, 0x00, 0x07  // 页地址范围：0~7
    };
    i2cData.send_buf = cmd;
    i2cData.send_len = 7;
    hi_i2c_write(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, &i2cData);

    // 打包整屏数据
    buffer[0] = 0x40; // 控制字节：数据
    uint16_t idx = 1;
    for (uint8_t page = 0; page < 8; page++) {
        for (uint8_t col = 0; col < 128; col++) {
            buffer[idx++] = OLED_GRAM[col][page];
        }
    }

    // 一次性写入 1024 字节
    i2cData.send_buf = buffer;
    i2cData.send_len = 1025;
    hi_i2c_write(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, &i2cData);
}


void oled_debug_image(void)
{
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 128; x++) {
            int page = y / 8;
            int bit = y % 8;
            if (OLED_GRAM[x][page] & (1 << bit)) {
                printf("█");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}


void oled_set_pos(unsigned char x, unsigned char y) 
{ 
	oled_wr_byte(0xb0+y,OLED_CMD);
	oled_wr_byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	oled_wr_byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void oled_display_on(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_wr_byte(0X14,OLED_CMD);  //DCDC ON
	oled_wr_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void oled_display_off(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC命令
	oled_wr_byte(0X10,OLED_CMD);  //DCDC OFF
	oled_wr_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
		
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void oled_clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{    
		for(n=0;n<128;n++)
		{
			OLED_GRAM[n][i]=0;
		}
	}
	// oled_refresh_gram();//更新显示 
}

//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void oled_drawpoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;		    
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
		{ 	
			oled_drawpoint(x,y,dot);	
		}
	}													    
}

//填充矩形
//x0,y0:矩形的左上角坐标
//width,height:矩形的尺寸
//color:颜色
void oled_fill_rectangle(uint8_t x0,uint8_t y0,uint8_t width,uint8_t height,uint8_t color)
{	  							   
	if(width==0||height==0)return;//非法.	 
	oled_fill(x0,y0,x0+width-1,y0+height-1,color);	   	   
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void oled_drawline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
{
	uint8_t t; 
	char xerr=0,yerr=0,delta_x,delta_y,distance; 
	char incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		oled_drawpoint(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}

//画大点函数
//x0,y0:坐标
//color:颜色
//以(x0,y0)为中心,画一个9个点的大点
void oled_draw_bigpoint(uint8_t x0,uint8_t y0,uint8_t color)
{
	uint8_t i,j;
	uint8_t x,y;				    
	if(x0>=1)x=x0-1;
	else x=x0;
	if(y0>=1)y=y0-1;
	else y=y0;
	for(i=y;i<y0+2;i++)
	{
		for(j=x;j<x0+2;j++)oled_drawpoint(j,i,color);
	}  						   
}

//画垂直线
//x0,y0:坐标
//len:线长度
//color:颜色
void oled_draw_vline(uint8_t x0,uint8_t y0,uint8_t len,uint8_t color)
{
	if((len==0)||(x0>Max_Column-1)||(y0>Max_Row-1))return;
	oled_fill(x0,y0,x0,y0+len-1,color);	
}

//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void oled_draw_hline(uint8_t x0,uint8_t y0,uint8_t len,uint8_t color)
{
	if((len==0)||(x0>Max_Column-1)||(y0>Max_Row-1))return;
	oled_fill(x0,y0,x0+len-1,y0,color);	
}

//画实心圆
//x0,y0:坐标
//r半径
//color:颜色
void oled_fill_circle(uint8_t x0,uint8_t y0,uint8_t r,uint8_t color)
{											  
	uint16_t i;
	uint16_t imax = ((uint32_t)r*707)/1000+1;
	uint16_t sqmax = (uint32_t)r*(uint32_t)r+(uint32_t)r/2;
	uint16_t x=r;
	oled_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax) 
		{
			// draw lines from outside  
			if (x>imax) 
			{
				oled_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				oled_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		oled_draw_hline(x0-x,y0+i,2*x,color);
		oled_draw_hline(x0-x,y0-i,2*x,color);
	}
}

//画椭圆
//x0,y0:坐标
//rx:x方向半径
//ry:y方向半径
void oled_draw_ellipse(uint8_t x0, uint8_t y0, uint8_t rx, uint8_t ry,uint8_t color) 
{
	uint16_t OutConst, Sum, SumY;
	uint8_t x,y;
	uint8_t xOld;
	uint8_t _rx = rx;
	uint8_t _ry = ry;
	if(rx>x0||ry>y0)return;//非法.
	OutConst = _rx*_rx*_ry*_ry+(_rx*_rx*_ry>>1);  // Constant as explaint above 
	// To compensate for rounding  
	xOld = x = rx;
	for (y=0; y<=ry; y++) 
	{
		if (y==ry)x=0; 
		else 
		{
			SumY =((uint16_t)(rx*rx))*((uint32_t)(y*y)); // Does not change in loop  
			while(Sum = SumY + ((uint16_t)(ry*ry))*((uint32_t)(x*x)),(x>0) && (Sum>OutConst)) x--;
		}
		// Since we draw lines, we can not draw on the first iteration		    
		if (y) 
		{
			oled_drawline(x0-xOld,y0-y+1,x0-x,y0-y,color);
			oled_drawline(x0-xOld,y0+y-1,x0-x,y0+y,color);
			oled_drawline(x0+xOld,y0-y+1,x0+x,y0-y,color);
			oled_drawline(x0+xOld,y0+y-1,x0+x,y0+y,color);
		}
		xOld = x;
	}
}

//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void oled_drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
{
	oled_drawline(x1,y1,x2,y1,color);
	oled_drawline(x1,y1,x1,y2,color);
	oled_drawline(x1,y2,x2,y2,color);
	oled_drawline(x2,y1,x2,y2,color);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void oled_draw_circle(uint8_t x0,uint8_t y0,uint8_t r,uint8_t color)
{
	char a,b;
	char di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		oled_drawpoint(x0+a,y0-b,color);             //5
 		oled_drawpoint(x0+b,y0-a,color);             //0           
		oled_drawpoint(x0+b,y0+a,color);             //4               
		oled_drawpoint(x0+a,y0+b,color);             //6 
		oled_drawpoint(x0-a,y0+b,color);             //1       
 		oled_drawpoint(x0-b,y0+a,color);             
		oled_drawpoint(x0-a,y0-b,color);             //2             
  		oled_drawpoint(x0-b,y0-a,color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}  

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void oled_showchar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=ascii_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=ascii_1608[chr][t];	//调用1608字体
		else if(size==24)temp=ascii_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_drawpoint(x,y,mode);
			else oled_drawpoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//num:数值(0~4294967295);	 		  
void oled_shownum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_showchar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	oled_showchar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
// void oled_showstring(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode) {
//     if (size != 16) return; // 仅支持16点阵字体
// 	printf("oled_showstring: x=%d, y=%d, size=%d, mode=%d\n", x, y, size, mode);
//     while (*p >= 0x20 && *p <= 0xF8) { // 遍历字符串
//         if (*p & 0x80) { // 中文字符（GB2312）

//             oled_showfontHZ(x, y, (*p - 0xA0) * 94 + (*(p + 1) - 0xA0), size, mode); // 显示中文字符
//             x += size; // 调整x坐标
//         	p += 3; // 跳过双字节
//         } else if (*p >= ' ' && *p <= '~') { // ASCII字符
//             if(x>(X_WIDTH-(size/2))){x=0;y+=size;}
//         	if(y>(Y_WIDTH-size)){y=x=0;oled_clear();}
//         	oled_showchar(x,y,*p,size,mode);	 
//         	x+=size/2;
//         	p++;
// 		}
// 	}
// }
uint16_t unicode_to_gb2312(uint16_t unicode) {
    for (uint16_t i = 0; i < map_size; i++) {
        if (utf8_to_gb2312_map[i].unicode == unicode) {
            return utf8_to_gb2312_map[i].gb2312;
        }
    }
    return 0; // 未找到
}

void oled_showstring(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode) {
    if (size != 16) return; // 仅支持16点阵字体
	//printf("oled_showstring: x=%d, y=%d, size=%d, mode=%d\n", x, y, size, mode);
    while (*p !='\0') { // 遍历字符串
        if (*p & 0x80) { // 中文字符（UTF-8）
			uint16_t unicode = ((*p & 0x0F) << 12) | ((*(p + 1) & 0x3F) << 6) | (*(p + 2) & 0x3F);
			//printf("unicode: %x\n", unicode);
			uint16_t index = unicode_to_gb2312(unicode);
			//printf("index: %x\n", index);
            oled_showfontHZ(x, y, index, size, mode); // 显示中文字符
            x += size; // 调整x坐标
        	p += 3; // 跳过三字节
        } else if (*p >= ' ' && *p <= '~') { // ASCII字符
            if(x>(X_WIDTH-(size/2))){x=0;y+=size;}
        	if(y>(Y_WIDTH-size)){y=x=0;oled_clear();}
        	oled_showchar(x,y,*p,size,mode);	 
        	x+=size/2;
        	p++;
		}
	}
}

#include <ctype.h>
void oled_showstring_12(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode) {
    // 强制要求：只处理12号字体
    if (size != 12) {
        return;
    }
    uint8_t char_width = size / 2; // 根据 oled_showchar 的逻辑，12号字体宽度为6
    while (*p != '\0') {
        // 使用标准库函数 isalnum() 检查当前字符是否为字母或数字
        // if (isalnum(*p)) {
            // 检查是否需要换行
        if (x > (X_WIDTH - char_width)) {
            x = 0;         // X坐标回到行首
            y += size;     // Y坐标换到下一行
        }
        // 调用现有的函数来显示单个字符
        oled_showchar(x, y, *p, size, mode);
        
        // 更新X坐标，准备显示下一个字符
        x += char_width;
        

        // 移动到字符串中的下一个字符，无论它是否被显示
        p++;
    }
}

//显示汉字
//x,y:起点坐标  
//pos:数组位置汉字显示
//size:字体大小 
//mode:0,反白显示;1,正常显示
void oled_showfontHZ(uint8_t x,uint8_t y,long long pos,uint8_t size,uint8_t mode)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;  
	uint8_t csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	 
	for(t=0;t<csize;t++)
	{   												   
		if(size==12)temp=FontHzk[pos][t]; 	 	//调用1206字体
		else if(size==16)temp=FontHzk[pos][t];	//调用1608字体
		else if(size==24)temp=FontHzk[pos][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_drawpoint(x,y,mode);
			else oled_drawpoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  	
} 

//显示BMP图片128×64
//起始点坐标(x,y),x的范围0～127，y为页的范围0～7
void oled_drawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[])
{ 	
 	uint16_t j=0;
 	uint8_t x,y;

  	if(y1%8==0)y=y1/8;      
  	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		oled_set_pos(x0,y);
    	for(x=x0;x<x1;x++)
	    {      
	    	oled_wr_byte(BMP[j++],OLED_DATA);	    	
	    }
	}
}



// void oled_draw_32_BMP(uint8_t x0, uint8_t y0, uint8_t BMP[]) 
// {
//     uint16_t j = 0;
//     uint8_t x, y;
//     uint8_t byte_data;

//     // 先将图像数据写入缓冲区
//     for (y = y0; y < y0 + 32; y++)  // 遍历32行
//     {       
//         oled_set_pos(x0, y);  // 设置每一行的起始位置
        
//         for (x = x0; x < x0 + 32; x++)  // 遍历32列
//         {       
//             // 计算图片数据中的位置
//             byte_data = BMP[j];  // 获取当前字节数据（每个字节对应8个像素）
            
//             // 获取当前像素的值（0 或 1），并通过位运算检查每一位
//             if (byte_data & (0x80 >> (x - x0))) {  
//                 oled_drawpoint(x, y, 1);  // 绘制白色像素
//             } else {
//                 oled_drawpoint(x, y, 0);  // 绘制黑色像素
//             }

//             // 每次绘制 8 个像素，跳到下一个字节
//             if ((x - x0 + 1) % 8 == 0) {
//                 j++;  // 每 8 个像素后跳到下一个字节
//             }
//         }
//     }
// }

// 显示BMP图片32x32
// 左下角坐标(x0,y0),x0的范围-32～127，y0的范围-32～63
// y0这里的坐标是从下往上数

void oled_draw_32_BMP(int8_t x0, int8_t y0, const uint8_t *image_data)
{
	oled_fill(x0, y0, x0 + 32, y0 - 32, 0);

	int8_t img_width = 32;  // 图像宽度
	int8_t img_height = 32; // 图像高度

	if (x0 >= 128 || y0 >= 64 || x0 + img_width <= 0 || y0 + img_height <= 0) return;

    // 绘制图像（翻转坐标以适配当前 OLED 设置）
    for (int8_t y = 0; y < img_height; y++) {
        int8_t page = (y0 + y) / 8;
        int8_t local_bit = (y0 + y) % 8;

        for (int8_t x = 0; x < img_width; x++) {
            // 镜像图像坐标（适配 OLED 设置 A1+C0）
            int8_t flipped_x = img_width - 1 - x;
            int8_t flipped_y = img_height - 1 - y;

            int8_t pixel = image_data[flipped_y * img_width + flipped_x];

            if (pixel) {
				int16_t screen_x = x0 + x;
				if (screen_x >= 0 && screen_x < 128 && page >= 0 && page < 8) {
    				OLED_GRAM[screen_x][page] |= (1 << local_bit);
				}
                // if ((x0 + x < 128) && (page < 8)) {
                //     OLED_GRAM[x0 + x][page] |= (1 << local_bit);
                // }
            }
        }
    }
}


//oled初始化
void oled_init(void)
{
	uint32_t result;

	hi_gpio_init();
    // gpio_9 复用为 I2C_SCL
    hi_io_set_pull(HI_IO_NAME_GPIO_9, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_I2C0_SCL);
    // gpio_10 复用为 I2C_SDA
    hi_io_set_pull(HI_IO_NAME_GPIO_10, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_I2C0_SDA);

    result = hi_i2c_init(SSD1306_I2C_IDX, 100000);
	//hi_i2c_set_baudrate(SSD1306_I2C_IDX, 400000); 
	hi_i2c_set_baudrate(SSD1306_I2C_IDX, 1000000); // 设置I2C时钟频率为100KHz
    if (result != HI_ERR_SUCCESS) 
	{
        printf("I2C SSD1306 Init status is 0x%x!!!\r\n", result);
        return result;
    }
	usleep(100*1000);//100ms
	
	write_iic_cmd(0x20); // 设置地址模式
	write_iic_cmd(0x00); // 00 = 横向寻址

	oled_wr_byte(0xAE,OLED_CMD); //关闭显示
	oled_wr_byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	oled_wr_byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	oled_wr_byte(0xA8,OLED_CMD); //设置驱动路数
	oled_wr_byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //设置显示偏移
	oled_wr_byte(0X00,OLED_CMD); //默认为0

	oled_wr_byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	oled_wr_byte(0x8D,OLED_CMD); //电荷泵设置
	oled_wr_byte(0x14,OLED_CMD); //bit2，开启/关闭
	oled_wr_byte(0x20,OLED_CMD); //设置内存地址模式
	//oled_wr_byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_wr_byte(0x00,OLED_CMD); //水平寻址
	oled_wr_byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_wr_byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	oled_wr_byte(0x12,OLED_CMD); //[5:4]配置
		 
	oled_wr_byte(0x81,OLED_CMD); //对比度设置
	oled_wr_byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	oled_wr_byte(0xD9,OLED_CMD); //设置预充电周期
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_wr_byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	oled_wr_byte(0xAF,OLED_CMD); //开启显示	

	oled_clear();

}

void oled_draw_digit(uint8_t x0, uint8_t y0, uint8_t v, uint8_t size) 
{
	if(size == 32)
	{	
		switch (v)
		{
		case 1:
			oled_draw_hline(x0+2, y0+2, 12, 1);
			oled_draw_hline(x0+3, y0+3, 10, 1);
			break;
		case 2:
			oled_draw_vline(x0+1, y0+3, 12, 1);
			oled_draw_vline(x0+2, y0+4, 10, 1);
			break;
		case 3:
			oled_draw_vline(x0+14, y0+3, 12, 1);
			oled_draw_vline(x0+13, y0+4, 10, 1);
			break;
		case 4:
			oled_draw_hline(x0+2, y0+15, 12, 1);
			oled_draw_hline(x0+3, y0+14, 10, 1);
			oled_draw_hline(x0+3, y0+16, 10, 1);
			break;
		case 5:
			oled_draw_vline(x0+1, y0+16, 12, 1);
			oled_draw_vline(x0+2, y0+17, 10, 1);
			break;
		case 6:
			oled_draw_vline(x0+14, y0+16, 12, 1);
			oled_draw_vline(x0+13, y0+17, 10, 1);
			break;
		case 7:
			oled_draw_hline(x0+2, y0+28, 12, 1);
			oled_draw_hline(x0+3, y0+27, 10, 1);
			break;
		default:
			break;
		}
	}else if(size == 24)
	{	
		switch (v)
		{
		case 1:
			oled_draw_hline(x0+2, y0+2, 8, 1);
			oled_draw_hline(x0+3, y0+3, 6, 1);
			break;
		case 2:
			oled_draw_vline(x0+1, y0+3, 8, 1);
			oled_draw_vline(x0+2, y0+4, 6, 1);
			break;
		case 3:
			oled_draw_vline(x0+10, y0+3, 8, 1);
			oled_draw_vline(x0+9, y0+4, 6, 1);
			break;
		case 4:
			oled_draw_hline(x0+2, y0+11, 8, 1);
			oled_draw_hline(x0+3, y0+10, 6, 1);
			oled_draw_hline(x0+3, y0+12, 6, 1);
			break;
		case 5:
			oled_draw_vline(x0+1, y0+12, 8, 1);
			oled_draw_vline(x0+2, y0+13, 6, 1);
			break;
		case 6:
			oled_draw_vline(x0+10, y0+12, 8, 1);
			oled_draw_vline(x0+9, y0+13,  6, 1);
			break;
		case 7:
			oled_draw_hline(x0+2, y0+20, 8, 1);
			oled_draw_hline(x0+3, y0+19, 6, 1);
			break;
		default:
			break;
		}
	}
	
}

void oled_draw_digit_num(uint8_t x0, uint8_t y0, uint8_t num, uint8_t size) 
{	
	switch (num)
	{
	case 0:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 5, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);
		break;
	case 1:
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 6, size);
		break;
	case 2:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 5, size);
		oled_draw_digit(x0, y0, 7, size);
		break;
	case 3:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);
		break;
	case 4:
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 6, size);
		break;
	case 5:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);
		break;
	case 6:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 5, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);
		break;
	case 7:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 6, size);
		break;
	case 8:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 5, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);
		break;	
	case 9:
		oled_draw_digit(x0, y0, 1, size);
		oled_draw_digit(x0, y0, 2, size);
		oled_draw_digit(x0, y0, 3, size);
		oled_draw_digit(x0, y0, 4, size);
		oled_draw_digit(x0, y0, 6, size);
		oled_draw_digit(x0, y0, 7, size);	
		break;
	default:
		break;
	}
}

void oled_show_digit_string(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size) {
    // 根据您的 oled_draw_digit 函数，每个数字的宽度约为16像素
    const uint8_t digit_width = size / 2;
    // 设置数字之间的间距，可以根据喜好调整
    const uint8_t spacing = size / 8;

    // 遍历整个字符串，直到遇到结束符'\0'
    while (*p != '\0') {
        // 判断当前字符是否是 '0' 到 '9' 之间的数字
        if (*p >= '0' && *p <= '9') {
            // 将字符 '0'-'9' 转换为对应的整数 0-9
            uint8_t num = *p - '0';
            
            // 调用您的函数来绘制这个数字
            oled_draw_digit_num(x, y, num, size);
            
            // 更新x坐标，为下一个数字准备好位置
            x += digit_width + spacing;
        }
        
        // 移动指针，处理字符串的下一个字符
        p++; 
    }
}

