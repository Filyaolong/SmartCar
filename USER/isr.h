/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		isr
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC387QP
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-12-11
 ********************************************************************************************************************/


#ifndef _isr_h
#define _isr_h

#include "headfile.h"

extern uint32 ClockTIME;
extern uint8 TIMESTRAT;
extern uint32 ClockTIME1;
extern uint8 TIMESTRAT1;
extern uint32 ClockTIME2;
extern uint8 TIMESTRAT2;
uint16 passbyms(uint8 start);//定时器
uint16 passby1ms(uint8 start);//定时器
uint16 passby500ms(uint8 start2);








#endif

