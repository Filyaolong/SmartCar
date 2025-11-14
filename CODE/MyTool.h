/*
 * MyTool.h
 *
 *  Created on: 2022年3月12日
 *      Author: Fily
 */

#ifndef CODE_MYTOOL_H_
#define CODE_MYTOOL_H_
void Guidance(uint8 WhetherOpen);
uint8 my_adapt_threshold(uint8 *image,uint16 col,uint16 row);//大津法
uint8 GetOSTUThreshold(uint8 (*img)[MT9V03X_W], uint16 start_row,uint16 end_row, uint16 start_col, uint16 end_col);
uint8 OSTU_bin(uint8 width,uint8 height,uint8 *Image);
void adaptiveThreshold(uint8* img_data, uint8* output_data, int width, int height, int block, uint8 clip_value);
void Chunkthreshold(uint8 *image,uint8 *OutputImage,uint16 width,uint16 height,uint8 Chunkwidth,uint8 Chunkheight);//自适应分段大津法
int32 My_abs(int32 i);//整数绝对值
float My_floatabs(float i);//浮点数绝对值
float Getslope(uint8 x1,uint8 y1,uint8 x2,uint8 y2);//计算斜率
float Get_angle(uint8 ax,uint8 ay,uint8 bx,uint8 by,uint8 cx,uint8 cy);//三点计算角度
float process_curvity(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 x3, uint8 y3);//三点计算曲率
void ShowlineWidth(void);//显示赛道宽度
void KeepLineWidth(void);//保存赛道宽度
void LookLookLineWidth(void);//显示保存赛道宽度
void Fill_line(uint8 LeftX,uint8 LeftY,uint8 RightX,uint8 RightY);//补线
void showline(void);//显示线
void showSuddenlyChangePoint(uint8 X,uint8 Y,uint16 Color);//显示突变点
void ShowReady_and_End(void);//显示计算误差的起始行与终止行
void My_sendBufftoPC();//传输数据到PC端
void sendimg_binary_CHK(uint8* image, uint8 width, uint8 height,uint8 otu,uint8 chk);
void sendimg_binary_zoom( uint8* image, uint8 width, uint8 height, uint8 dis_width, uint8 dis_height,uint8 otu);
void UART_Send_FloatBuff(float Buff);
void getData(void);
uint8 ZebraCrossing(void);
//需要修改的地方
#define  sw_write_byte(dat)  uart_putchar(WIRELESS_UART, dat)//串口发送字节
#define  sw_write_buffer(dat,len)  seekfree_wireless_send_buff(dat,len)//串口发送数组
#define ROW MT9V03X_W//图像宽
#define COL MT9V03X_H//图像高
//串口
//void put_int(uint8 name,int dat);
//void sendimg( uint8 *image, uint8 width, uint8 height);
void sendimg_binary(uint8* image, uint8 width, uint8 height,uint8 otu);
/*void sendline_clear( uint8 color,uint16 width, uint16 height);
void sendline_xy( uint8_t *line_zx,uint8_t *line_yx, uint32_t len);
void sendline( uint8 color,uint8_t *buff, uint32_t len);
void sendline2( uint8 color,uint8_t *linex,uint8_t *liney, uint32_t len);*/
//wifi
//void sendimg_WIFI( uint8 (*src)[COL], uint16 width, uint16 height);

#endif /* CODE_MYTOOL_H_ */
