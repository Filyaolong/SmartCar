/*
 * My_Fun.c
 *
 *  Created on: 2022年8月5日
 *      Author: Fily
 */
#include "headfile.h"
#include "My_Fun.h"
#include "isr.h"
#include "Angle.h"
#include "Motor.h"
#include "FlyMotor.h"
#include "Camera.h"
#include "MyTool.h"
#include "Control.h"
#include "ServoMotor.h"
#include "My_Flash.h"

//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;
uint8 key5_status = 1;

//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;
uint8 key5_last_status;

//开关标志位
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;
uint8 key5_flag;

/***************************《菜单变量定义》***********************************/

uint8 key=0,ii=90;                                                 //按键标志，逻辑位置

/***************************《菜单使用》***********************************/
void fun_use()
{
        getData();
        if(wireless_rx_buffer)
        {
            key = wireless_rx_buffer;
            wireless_rx_buffer = 0;
        }

        key_use();
        if(key1_flag)
        {
            key1_flag = 0;
            key = 1;//确定
        }

        if(key2_flag)
        {
            key2_flag = 0;
            key = 2;//下翻
        }

        if(key3_flag)
        {
            key3_flag = 0;
            key = 3;//加大
        }

        if(key4_flag)
        {
            key4_flag = 0;
            key = 4;//上翻
        }

        if(key5_flag)
        {
            key5_flag = 0;
            key = 5;//减小
        }

        if(key)
        {
          switch(key)
          {
          case 1:ii=table[ii].enter;break;
          case 2:ii=table[ii].down;break;
          case 3:ii=table[ii].left;break;
          case 4:ii=table[ii].up;break;
          case 5:ii=table[ii].right;break;
          };
          ips114_clear(WHITE);
          key=0;

        }

        (*table[ii].f)();
}

/***************************《发送数据》***********************************/
void SendMESSGE(void)
{
    uint8 a[3]={4,5,6};
    if(Camera.GetPoint.Left.CircleStep != 0)
    {
        uart_putbuff(WIRELESS_UART,&a[0],sizeof(a[0]));
    }

    if(Camera.GetPoint.Right.CircleStep != 0)
    {
        uart_putbuff(WIRELESS_UART,&a[1],sizeof(a[1]));
    }

    if(Camera.CalculateLine.SanCaStep != 0)
    {
        uart_putbuff(WIRELESS_UART,&a[2],sizeof(a[2]));
    }

    if(Camera.GetPoint.Right.CircleStep == 0 && Camera.GetPoint.Left.CircleStep == 0 && Camera.CalculateLine.SanCaStep == 0)
    {
        uart_putbuff(WIRELESS_UART,&Camera.CalculateLine.ElementType,sizeof(Camera.CalculateLine.ElementType));
    }
}

/***************************《菜单初始化》***********************************/
void MenuInit(void)
{
    gpio_init(KEY1,GPI,0,PULLUP);
    gpio_init(KEY2,GPI,0,PULLUP);
    gpio_init(KEY3,GPI,0,PULLUP);
    gpio_init(KEY4,GPI,0,PULLUP);
    gpio_init(KEY5 ,GPI,0,PULLUP);

    gpio_init(P33_10,GPO,0,PUSHPULL);
}

/***************************《搭建各逻辑位置》***********************************/
struct Fun const table[105]=                               //搭建各逻辑位置
{
//（目录数组,上翻,下翻,减小,加大,确定,功能函数指针）
/************决策选择************/
   {0,3,1,4,5,12,fun0},//第一层  决策选择
   {1,0,2,6,7,1,fun1},
   {2,1,3,8,9,2,fun2},
   {3,2,0,10,11,3,fun3},
   {4,4,4,4,4,4,fun4},
   {5,5,5,5,5,5,fun5},
   {6,6,6,6,6,6,fun6},
   {7,7,7,7,7,7,fun7},
   {8,8,8,8,8,8,fun8},
   {9,9,9,9,9,9,fun9},
   {10,10,10,10,10,10,fun10},
   {11,11,11,11,11,11,fun11},
   /************元素选择************/  //12 1  13 2  14 3  15 4  16 5  17 6  18 7  19 8  20 9
   {12,18,15,14,30,21,fun12},//第二层  元素选择
   {13,19,16,12,14,22,fun13},
   {14,20,17,13,12,23,fun14},
   {15,12,18,17,16,24,fun15},
   {16,13,19,15,17,25,fun16},
   {17,14,20,16,15,26,fun17},
   {18,15,12,20,19,27,fun18},
   {19,16,13,18,20,28,fun19},
   {20,17,14,19,18,29,fun20},
   /************元素跳转************/
   {21,21,21,21,21,21,fun21},
   {22,22,22,22,22,22,fun22},
   {23,23,23,23,23,23,fun23},
   {24,24,24,24,24,24,fun24},
   {25,25,25,25,25,25,fun25},
   {26,26,26,26,26,26,fun26},
   {27,27,27,27,27,27,fun27},
   {28,28,28,28,28,28,fun28},
   {29,29,29,29,29,29,fun29},
   /************送入Flash************/
   {30,30,30,30,30,31,fun30},
   {31,31,31,31,31,32,fun31},
   {32,32,32,32,32,33,fun32},
   {33,33,33,33,33,34,fun33},
   /************调参************/
   {34,38,35,34,34,95,fun34},
   {35,34,36,35,35,39,fun35},
   {36,35,37,36,36,61,fun36},
   {37,36,38,37,37,83,fun37},
   {38,37,34,38,38,38,fun38},
   /************调参Motor************/
   {39,46,40,47,48,39,fun39},
   {40,39,41,49,50,40,fun40},
   {41,40,42,51,52,41,fun41},
   {42,41,43,53,54,42,fun42},
   {43,42,44,55,56,43,fun43},
   {44,43,45,57,58,44,fun44},
   {45,44,46,59,60,45,fun45},
   {46,45,39,46,46,35,fun46},

   {47,47,47,47,47,47,fun47},
   {48,48,48,48,48,48,fun48},
   {49,49,49,49,49,49,fun49},
   {50,50,50,50,50,50,fun50},
   {51,51,51,51,51,51,fun51},
   {52,52,52,52,52,52,fun52},
   {53,53,53,53,53,53,fun53},
   {54,54,54,54,54,54,fun54},
   {55,55,55,55,55,55,fun55},
   {56,56,56,56,56,56,fun56},
   {57,57,57,57,57,57,fun57},
   {58,58,58,58,58,58,fun58},
   {59,59,59,59,59,59,fun59},
   {60,60,60,60,60,60,fun60},
   /************调参ServoMotor************/
   {61,68,62,69,70,61,fun61},
   {62,61,63,71,72,62,fun62},
   {63,62,64,73,74,63,fun63},
   {64,63,65,75,76,70,fun64},
   {65,64,66,77,78,72,fun65},
   {66,65,67,79,80,75,fun66},
   {67,66,68,81,82,71,fun67},
   {68,67,61,76,76,36,fun68},

   {69,69,69,69,69,69,fun69},
   {70,70,70,70,70,70,fun70},
   {71,71,71,71,71,71,fun71},
   {72,72,72,72,72,73,fun72},
   {73,73,73,73,73,73,fun73},
   {74,74,74,74,74,74,fun74},
   {75,75,75,75,75,75,fun75},
   {76,76,76,76,76,76,fun76},
   {77,77,77,77,77,77,fun77},
   {78,78,78,78,78,78,fun78},
   {79,79,79,79,79,79,fun79},
   {80,80,80,80,80,80,fun80},
   {81,81,81,81,81,81,fun81},
   {82,82,82,82,82,82,fun82},
   /************调参FlyMotor************/
   {83,95,84,86,87,83,fun83},
   {84,83,85,88,89,84,fun84},
   {85,84,83,85,85,37,fun85},

   {86,86,86,86,86,86,fun86},
   {87,87,87,87,87,87,fun87},
   {88,88,88,88,88,88,fun88},
   {89,89,89,89,89,89,fun89},
   /************调参Camera************/
   {90,99,98,91,92,0,fun90},

   {91,91,91,91,91,91,fun91},
   {92,92,92,92,92,92,fun92},
   {93,93,93,93,93,93,fun93},
   {94,94,94,94,94,94,fun94},
   /************跑车&&查看值************/
   {95,95,95,95,95,95,fun95},
   {96,97,97,97,97,95,fun96},
   {97,96,96,96,96,96,fun97},
   /************调参Camera(附加)************/
   {98,90,99,93,94,98,fun98},
   {99,98,90,100,101,99,fun99},

   {100,100,100,100,100,100,fun100},
   {101,101,101,101,101,101,fun101},
   /*{102,102,102,102,102,102,fun102},
   {103,103,103,103,103,103,fun103},
   {104,104,104,104,104,104,fun104},*/

};

/***************************《按键标志位》***********************************/
void key_use()
{
      //保存按键状态
      key1_last_status = key1_status;
      key2_last_status = key2_status;
      key3_last_status = key3_status;
      key4_last_status = key4_status;
      key5_last_status = key5_status;
      //读取当前按键状态
      key1_status = gpio_get(KEY1);
      key2_status = gpio_get(KEY2);
      key3_status = gpio_get(KEY3);
      key4_status = gpio_get(KEY4);
      key5_status = gpio_get(KEY5);
      //检测到按键按下之后  并放开置位标志位
      if(key1_status && !key1_last_status)    key1_flag = 1;
      if(key2_status && !key2_last_status)    key2_flag = 1;
      if(key3_status && !key3_last_status)    key3_flag = 1;
      if(key4_status && !key4_last_status)    key4_flag = 1;
      if(key5_status && !key5_last_status)    key5_flag = 1;
}

/****************************第一层状态机*******************************/
void fun0()
{
    uint16 ADCBATVCC = 0;
    uint8 BATVCC = 0;
    ADCBATVCC = adc_mean_filter(ADC_1, ADC1_CH0_A8, ADC_12BIT, 5);
    if(ADCBATVCC-1304>0)
    {
        BATVCC = 100 * (ADCBATVCC-1304) / (1404-1304);
    }
    else BATVCC = 0;
    //10K/(100K+10K)=0.091  最高电压ADC=0.091*12.6=1.1466，最低电压ADC=0.091*11.7=1.0647
    //参照电压3.344V 12位ADC 4096 电池最高ADC=(1.1466/3.344)*4096=1404 电池最低ADC=(1.0647/3.344)*4096=1304
    //百分比差1404-1304=100 100除以100,取整数为1,如果1404为电量100% ,1403为电路99%,1304为电路1%
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"WhichSpeed");   ips114_showuint8(150,1,Camera.CalculateLine.WhichSpeed);
    ips114_showstr(10,2,(int8 *)"WhichOut");     ips114_showuint8(150,2,Camera.CalculateLine.WhichOut);
    ips114_showstr(10,3,(int8 *)"BATVCC");       ips114_showuint16(150,3,BATVCC);
    ips114_showstr(10,4,(int8 *)"FMAX");         ips114_showuint16(150,4,FlyMotor.FMAX);
    ips114_showstr(0,0,(int8 *)">");
}

void fun1()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"WhichSpeed");   ips114_showuint8(150,1,Camera.CalculateLine.WhichSpeed);
    ips114_showstr(10,2,(int8 *)"WhichOut");     ips114_showuint8(150,2,Camera.CalculateLine.WhichOut);
    ips114_showstr(10,3,(int8 *)"FMAX");         ips114_showuint16(150,3,FlyMotor.FMAX);
    ips114_showstr(0,1,(int8 *)">");
}

void fun2()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"WhichSpeed");   ips114_showuint8(150,1,Camera.CalculateLine.WhichSpeed);
    ips114_showstr(10,2,(int8 *)"WhichOut");     ips114_showuint8(150,2,Camera.CalculateLine.WhichOut);
    ips114_showstr(10,3,(int8 *)"FMAX");         ips114_showuint16(150,3,FlyMotor.FMAX);
    ips114_showstr(0,2,(int8 *)">");
}

void fun3()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"WhichSpeed");   ips114_showuint8(150,1,Camera.CalculateLine.WhichSpeed);
    ips114_showstr(10,2,(int8 *)"WhichOut");     ips114_showuint8(150,2,Camera.CalculateLine.WhichOut);
    ips114_showstr(10,3,(int8 *)"FMAX");         ips114_showuint16(150,3,FlyMotor.FMAX);
    ips114_showstr(0,3,(int8 *)">");
}

void fun4()
{
    Motor.ExternSpeed -= 3;
    ii = 0;
}

void fun5()
{
    Motor.ExternSpeed += 3;
    ii = 0;
}

void fun6()
{
    Camera.CalculateLine.WhichSpeed = 1;
    ii = 1;
}

void fun7()
{
    Camera.CalculateLine.WhichSpeed = 2;
    ii = 1;
}

void fun8()
{
    Camera.CalculateLine.WhichOut = (uint32)SavedValueMini(SectorofOut,PageofOut);
    ii = 2;
}

void fun9()
{
    Camera.CalculateLine.WhichOut = (uint32)SavedValueAdd(SectorofOut,PageofOut);
    ii = 2;
}

void fun10()
{
    FlyMotor.FMAX -= 200;
    ii = 3;
}

void fun11()
{
    FlyMotor.FMAX += 200;
    ii = 3;
}
/****************************元素*******************************/
//无法判断0 直道1 弯道2 斑马线3 左圆环4 右圆环5 三叉左6 十字7 坡道8 三叉右9
uint32 YuanSuXuWei[50];
uint8 YuanSuJishu = 0;
void fun12()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"*1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun13()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"*2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun14()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"*3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun15()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"*4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun16()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"*5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun17()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"*6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun18()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"*7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun19()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"*8"); ips114_showstr(60,7,(int8 *)"9");
}

void fun20()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }

    ips114_showstr(20,5,(int8 *)"1"); ips114_showstr(40,5,(int8 *)"2"); ips114_showstr(60,5,(int8 *)"3");
    ips114_showstr(20,6,(int8 *)"4"); ips114_showstr(40,6,(int8 *)"5"); ips114_showstr(60,6,(int8 *)"6");
    ips114_showstr(20,7,(int8 *)"7"); ips114_showstr(40,7,(int8 *)"8"); ips114_showstr(60,7,(int8 *)"*9");
}

void fun21()
{
    YuanSuXuWei[YuanSuJishu] = 1;
    YuanSuJishu++;
    ii = 12;
}

void fun22()
{
    YuanSuXuWei[YuanSuJishu] = 2;
    YuanSuJishu++;
    ii = 13;
}

void fun23()
{
    YuanSuXuWei[YuanSuJishu] = 3;
    YuanSuJishu++;
    ii = 14;
}

void fun24()
{
    YuanSuXuWei[YuanSuJishu] = 4;
    YuanSuJishu++;
    ii = 15;
}

void fun25()
{
    YuanSuXuWei[YuanSuJishu] = 5;
    YuanSuJishu++;
    ii = 16;
}

void fun26()
{
    YuanSuXuWei[YuanSuJishu] = 6;
    YuanSuJishu++;
    ii = 17;
}

void fun27()
{
    YuanSuXuWei[YuanSuJishu] = 7;
    YuanSuJishu++;
    ii = 18;
}

void fun28()
{
    YuanSuXuWei[YuanSuJishu] = 8;
    YuanSuJishu++;
    ii = 19;
}

void fun29()
{
    YuanSuXuWei[YuanSuJishu] = 9;
    YuanSuJishu++;
    ii = 20;
}

void fun30()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }
    ips114_showstr(0,5,(int8 *)"WhetherSure...");
}

void fun31()
{
    for(uint8 i=0;i<YuanSuJishu;i++)
    {
        if(flash_check(SectorofOutYuansu, i))
        {
            eeprom_erase_sector(SectorofOutYuansu);
        }
    }

    for(uint8 i=0;i<YuanSuJishu;i++)
    {
        eeprom_page_program(SectorofOutYuansu,i, &YuanSuXuWei[i]);
    }
    ips114_showstr(0,0,(int8 *)"FlashOK...");
}

void fun32()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            ips114_showuint8(i,j,flash_read(SectorofOutYuansu,k,uint8));
            k++;
        }
    }
    ips114_showstr(0,5,(int8 *)"SeeFlash...");
}

void fun33()
{
    uint8 k = 0;
    for(uint8 j=0;j<5;j=j+1)
    {
        for(uint8 i=0;i<240;i=i+30)
        {
            YuanSuXuWei[k] = flash_read(SectorofOutYuansu,k,uint8);
            ips114_showuint8(i,j,YuanSuXuWei[k]);
            k++;
        }
    }
    ips114_showstr(0,5,(int8 *)"Finish!!!");
}
/****************************调参*******************************/
void fun34()
{
    ips114_showstr(10,0,(int8 *)"LetGO!");
    ips114_showstr(10,1,(int8 *)"Motor");
    ips114_showstr(10,2,(int8 *)"ServoMotor");
    ips114_showstr(10,3,(int8 *)"FlyMotor");
    ips114_showstr(10,4,(int8 *)"Camera");
    ips114_showstr(0,0,(int8 *)">");
}

void fun35()
{
    ips114_showstr(10,0,(int8 *)"LetGO!");
    ips114_showstr(10,1,(int8 *)"Motor");
    ips114_showstr(10,2,(int8 *)"ServoMotor");
    ips114_showstr(10,3,(int8 *)"FlyMotor");
    ips114_showstr(10,4,(int8 *)"Camera");
    ips114_showstr(0,1,(int8 *)">");
}

void fun36()
{
    ips114_showstr(10,0,(int8 *)"LetGO!");
    ips114_showstr(10,1,(int8 *)"Motor");
    ips114_showstr(10,2,(int8 *)"ServoMotor");
    ips114_showstr(10,3,(int8 *)"FlyMotor");
    ips114_showstr(10,4,(int8 *)"Camera");
    ips114_showstr(0,2,(int8 *)">");
}

void fun37()
{
    ips114_showstr(10,0,(int8 *)"LetGO!");
    ips114_showstr(10,1,(int8 *)"Motor");
    ips114_showstr(10,2,(int8 *)"ServoMotor");
    ips114_showstr(10,3,(int8 *)"FlyMotor");
    ips114_showstr(10,4,(int8 *)"Camera");
    ips114_showstr(0,3,(int8 *)">");
}

void fun38()
{
    ips114_showstr(10,0,(int8 *)"LetGO!");
    ips114_showstr(10,1,(int8 *)"Motor");
    ips114_showstr(10,2,(int8 *)"ServoMotor");
    ips114_showstr(10,3,(int8 *)"FlyMotor");
    ips114_showstr(10,4,(int8 *)"Camera");
    ips114_showstr(0,4,(int8 *)">");
}

/****************************调参Motor*******************************/
void fun39()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,0,(int8 *)">");
}

void fun40()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,1,(int8 *)">");
}

void fun41()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,2,(int8 *)">");
}

void fun42()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,3,(int8 *)">");
}

void fun43()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,4,(int8 *)">");
}

void fun44()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,5,(int8 *)">");
}

void fun45()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,6,(int8 *)">");
}

void fun46()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,1,(int8 *)"Velocity.KP");  ips114_showfloat(150,1,MotorVelocity.KP,2,3);
    ips114_showstr(10,2,(int8 *)"Velocity.KI");  ips114_showfloat(150,2,MotorVelocity.KI,2,3);
    ips114_showstr(10,3,(int8 *)"Velocity.KD");  ips114_showfloat(150,3,MotorVelocity.KD,2,3);
    ips114_showstr(10,4,(int8 *)"Angle.KP");     ips114_showfloat(150,4,MotorAngle.KP,2,3);
    ips114_showstr(10,5,(int8 *)"Angle.KI");     ips114_showfloat(150,5,MotorAngle.KI,2,3);
    ips114_showstr(10,6,(int8 *)"Angle.KD");     ips114_showfloat(150,6,MotorAngle.KD,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,7,(int8 *)">");
}

void fun47()
{
    Motor.ExternSpeed -= 3;
    ii=39;
}

void fun48()
{
    Motor.ExternSpeed += 3;
    ii=39;
}

void fun49()
{
    MotorVelocity.KP -= 1;
    ii=40;
}

void fun50()
{
    MotorVelocity.KP += 1;
    ii=40;
}

void fun51()
{
    MotorVelocity.KI -= 1;
    ii=41;
}

void fun52()
{
    MotorVelocity.KI += 1;
    ii=41;
}

void fun53()
{
    MotorVelocity.KD -= 1;
    ii=42;
}

void fun54()
{
    MotorVelocity.KD += 1;
    ii=42;
}

void fun55()
{
    MotorAngle.KP -= 1;
    ii=43;
}

void fun56()
{
    MotorAngle.KP += 1;
    ii=43;
}

void fun57()
{
    MotorAngle.KI -= 1;
    ii=44;
}

void fun58()
{
    MotorAngle.KI += 1;
    ii=44;
}

void fun59()
{
    MotorAngle.KD -= 1;
    ii=45;
}

void fun60()
{
    MotorAngle.KD += 1;
    ii=45;
}

/****************************调参ServoMotor*******************************/
void fun61()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,0,(int8 *)">");
}


void fun62()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,1,(int8 *)">");
}

void fun63()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,2,(int8 *)">");
}

void fun64()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,3,(int8 *)">");
}

void fun65()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,4,(int8 *)">");
}

void fun66()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,5,(int8 *)">");
}

void fun67()
{
    ips114_showstr(10,0,(int8 *)"ExternSpeed");  ips114_showint16(150,0,Motor.ExternSpeed);
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,6,(int8 *)">");
}

void fun68()
{
    ips114_showstr(10,0,(int8 *)"KP");       ips114_showfloat(150,0,IncrementPID.KP,2,3);
    ips114_showstr(10,1,(int8 *)"KI");       ips114_showfloat(150,1,IncrementPID.KI,2,3);
    ips114_showstr(10,2,(int8 *)"ErectKP");  ips114_showfloat(150,2,IncrementPID.ErectKP,2,3);
    ips114_showstr(10,3,(int8 *)"ErectKI");  ips114_showfloat(150,3,IncrementPID.ErectKI,2,3);
    ips114_showstr(10,4,(int8 *)"ErectKD");  ips114_showfloat(150,4,IncrementPID.ErectKD,2,3);
    ips114_showstr(10,5,(int8 *)"K");        ips114_showfloat(150,5,IncrementPID.CoefficientK,2,3);
    ips114_showstr(10,6,(int8 *)"Q");        ips114_showfloat(150,6,IncrementPID.CoefficientQ,2,3);
    ips114_showstr(10,7,(int8 *)"Down");
    ips114_showstr(0,7,(int8 *)">");
}

void fun69()
{
    IncrementPID.KP -= 0.3;
    ii=61;
}

void fun70()
{
    IncrementPID.KP += 0.3;
    ii=61;
}

void fun71()
{
    IncrementPID.KI -= 0.1;
    ii=62;
}

void fun72()
{
    IncrementPID.KI += 0.1;
    ii=62;
}

void fun73()
{
    IncrementPID.ErectKP -= 1;
    ii=63;
}

void fun74()
{
    IncrementPID.ErectKP += 1;
    ii=63;
}

void fun75()
{
    IncrementPID.ErectKI -= 0.1;
    ii=64;
}

void fun76()
{
    IncrementPID.ErectKI += 0.1;
    ii=64;
}

void fun77()
{
    IncrementPID.ErectKD -= 0.1;
    ii=65;
}

void fun78()
{
    IncrementPID.ErectKD += 0.1;
    ii=65;
}

void fun79()
{
    IncrementPID.CoefficientK -= 0.01;
    ii=66;
}

void fun80()
{
    IncrementPID.CoefficientK += 0.01;
    ii=66;
}

void fun81()
{
    IncrementPID.CoefficientQ -= 0.01;
    ii=67;
}

void fun82()
{
    IncrementPID.CoefficientQ += 0.01;
    ii=67;
}

/****************************调参FlyMotor*******************************/
void fun83()
{
    ips114_showstr(10,0,(int8 *)"ExpectAngle");  ips114_showfloat(150,0,FlyMotor.ExpectAngle,3,3);
    ips114_showstr(10,1,(int8 *)"FMAX");         ips114_showint16(150,1,FlyMotor.FMAX);
    ips114_showstr(10,2,(int8 *)"Down");
    ips114_showstr(0,0,(int8 *)">");
}


void fun84()
{
    ips114_showstr(10,0,(int8 *)"ExpectAngle");  ips114_showfloat(150,0,FlyMotor.ExpectAngle,3,3);
    ips114_showstr(10,1,(int8 *)"FMAX");         ips114_showint16(150,1,FlyMotor.FMAX);
    ips114_showstr(10,2,(int8 *)"Down");
    ips114_showstr(0,1,(int8 *)">");
}

void fun85()
{
    ips114_showstr(10,0,(int8 *)"ExpectAngle");  ips114_showfloat(150,0,FlyMotor.ExpectAngle,3,3);
    ips114_showstr(10,1,(int8 *)"FMAX");         ips114_showint16(150,1,FlyMotor.FMAX);
    ips114_showstr(10,2,(int8 *)"Down");
    ips114_showstr(0,2,(int8 *)">");
}

void fun86()
{
    FlyMotor.ExpectAngle -= 0.01;
    ii=83;
}

void fun87()
{
    FlyMotor.ExpectAngle += 0.01;
    ii=83;
}

void fun88()
{
    FlyMotor.FMAX -= 300;
    ii=84;
}

void fun89()
{
    FlyMotor.FMAX += 300;
    ii=84;
}

/****************************调参Camera*******************************/
void fun90()//二值化后的图像 用于查看阈值
{
     ips114_displayimage032_zoom(Camera.CalculateLine.image[0], MT9V03X_W, MT9V03X_H,188,120);
     showline();
     ShowReady_and_End();

     ips114_showuint16(200,0,Camera.CalculateLine.threshold);
     ips114_showuint8(200,1,Camera.CalculateLine.MiddleLine);
     ips114_showuint16(200,2,Camera.CalculateLine.Light);

     ips114_showuint16(200,6,Camera.CalculateLine.image_threshold);
     ips114_showuint8(200,7,Camera.CalculateLine.Finalerror);
     ips114_showstr(180,0,(int8 *)">");

}

void fun98()
{
    ips114_displayimage032_zoom(mt9v03x_image[0], MT9V03X_W, MT9V03X_H,188,120);
    showline();
    ShowReady_and_End();

    ips114_showuint16(200,0,Camera.CalculateLine.threshold);
    ips114_showuint8(200,1,Camera.CalculateLine.MiddleLine);
    ips114_showuint16(200,2,Camera.CalculateLine.Light);

    ips114_showuint16(200,6,Camera.CalculateLine.image_threshold);
    ips114_showuint8(200,7,Camera.CalculateLine.Finalerror);
    ips114_showstr(180,1,(int8 *)">");
}

void fun99()
{
    ips114_displayimage032_zoom(mt9v03x_image[0], MT9V03X_W, MT9V03X_H,188,120);
    showline();
    ShowReady_and_End();

    ips114_showuint16(200,0,Camera.CalculateLine.threshold);
    ips114_showuint8(200,1,Camera.CalculateLine.MiddleLine);
    ips114_showuint16(200,2,Camera.CalculateLine.Light);
    ips114_showuint16(200,3,Camera.CalculateLine.ReturnLight);

    ips114_showuint16(200,6,Camera.CalculateLine.image_threshold);
    ips114_showuint8(200,7,Camera.CalculateLine.Finalerror);
    ips114_showstr(180,2,(int8 *)">");
}

void fun100()
{
    Camera.CalculateLine.Light -= 10;
    Camera.CalculateLine.ReturnLight = set_exposure_time(MT9V03X_COF_UART,Camera.CalculateLine.Light);
    ii = 99;
}

void fun101()
{
    Camera.CalculateLine.Light += 10;
    Camera.CalculateLine.ReturnLight = set_exposure_time(MT9V03X_COF_UART,Camera.CalculateLine.Light);
    ii = 99;
}

void fun91()
{
    Camera.CalculateLine.threshold = SavedValueMini(SectorofThreshold,PageofThreshold);
    ii = 90;
}

void fun92()
{
    Camera.CalculateLine.threshold = SavedValueAdd(SectorofThreshold,PageofThreshold);
    ii = 90;
}

void fun93()
{
    Camera.CalculateLine.MiddleLine = SavedValueMini(SectorofMiddleLine,PageofMiddleLine);
    ii = 98;
}

void fun94()
{
    Camera.CalculateLine.MiddleLine = SavedValueAdd(SectorofMiddleLine,PageofMiddleLine);
    ii = 98;
}



/****************************跑车&&查看值*******************************/
void fun95()
{
    control_status = 1;
    ii = 96;
}

void fun96()
{
    ips114_showstr(0,0,(int8 *)"LetGO!");

    ips114_showstr(0,1,(int8 *)"ZhiDao");         ips114_showuint8(150,1,Camera.CalculateLine.ZhiDao);
    ips114_showstr(0,2,(int8 *)"ElementType");    ips114_showuint8(150,2,Camera.CalculateLine.ElementType);
    ips114_showstr(0,3,(int8 *)"LongZhiDao");     ips114_showuint8(150,3,Camera.CalculateLine.LongZhiDao);
    ips114_showstr(0,4,(int8 *)"Right.Slope1");   ips114_showfloat(150,4,Camera.GetPoint.Right.Slope1,3,3);
    ips114_showstr(0,5,(int8 *)"gyro");           ips114_showfloat(150,5,My_yaw.gyro,5,3);
    ips114_showstr(0,6,(int8 *)"gyro_sum");       ips114_showfloat(150,6,My_yaw.gyro_sum,5,3);
    ips114_showstr(0,7,(int8 *)"ClockTIME2");     ips114_showfloat(150,7,ClockTIME2/2,4,3);

}

void fun97()
{
    ips114_displayimage032_zoom(Camera.CalculateLine.image[0], MT9V03X_W, MT9V03X_H,188,120);
    showline();
    ShowReady_and_End();
    if(Camera.GetPoint.Left.NearChangeX)
    {
        showSuddenlyChangePoint(Camera.GetPoint.Left.NearChangeX,Camera.GetPoint.Left.NearChangeY,GREEN);
    }
    if(Camera.GetPoint.Right.NearChangeX)
    {
        showSuddenlyChangePoint(Camera.GetPoint.Right.NearChangeX,Camera.GetPoint.Right.NearChangeY,GREEN);
    }
    if(Camera.GetPoint.Left.FarChangeX)
    {
        showSuddenlyChangePoint(Camera.GetPoint.Left.FarChangeX,Camera.GetPoint.Left.FarChangeY,GREEN);
    }
    if(Camera.GetPoint.Right.FarChangeX)
    {
        showSuddenlyChangePoint(Camera.GetPoint.Right.FarChangeX,Camera.GetPoint.Right.FarChangeY,GREEN);
    }

    ips114_showstr(200,0,(int8 *)"Step");            ips114_showuint8(200,1,Camera.GetPoint.Right.CircleStep);
    ips114_showstr(200,2,(int8 *)"gyro");            ips114_showuint8(200,3,My_yaw.gyro_sum);
    ips114_showstr(200,4,(int8 *)"Right");           ips114_showuint8(200,5,Camera.GetPoint.Right.CircleStep);
    ips114_showstr(200,6,(int8 *)"Left");            ips114_showuint8(200,7,Camera.GetPoint.Left.CircleStep);

}
