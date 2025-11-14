/*
 * Control.c
 *
 *  Created on: 2022年8月5日
 *      Author: Fily
 */
#include "headfile.h"
#include "Motor.h"
#include "Control.h"
#include "Angle.h"
#include "FlyMotor.h"
#include "MyTool.h"
#include "Camera.h"
#include "ServoMotor.h"
#include "isr.h"

uint8 core1 = 0;
uint8 core2 = 0;
uint16 core3 = 0;
uint8 core4 = 0;
uint8 core5 = 0;
uint8 core6 = 0;

uint8 control_status = 0;//0保护状态;1摩托车准备运行状态;2.摩托车运行状态
uint8 CarStable = 1;//0调试状态;1跑路状态
uint8 Protect = 0;

void Control(void)
{
  core1++;
  core2++;
if(CarStable == 1)
{
  core3++;
/***************************跑车状态***********************************/
  if(core2 == 2)
  {
    GetAngle();
    Guidance(Camera.GuidanceOpen);
    //My_sendBufftoPC();
    if(My_pitch.Angle > FlyMotor.ExpectAngle + 15 || My_pitch.Angle < FlyMotor.ExpectAngle - 15)//角度大小判断是否需要触发保护
    {
        control_status = 0;
    }

    if(control_status == 0)
    {
        gpio_set(P21_5, 0);
        pwm_duty(ATOM0_CH2_P21_4, 0);
        gpio_set(P21_3, 0);
        pwm_duty(ATOM0_CH0_P21_2, 0);
        pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle);
        core3 = 0;
    }
    /*******************稳态**********************/
    else if(control_status == 1)
    {
        FlyexpectSpeed_out();//串级PID
        FlyMotor_out();//飞轮控制
        pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle);
        gpio_set(P21_3, 0);
        pwm_duty(ATOM0_CH0_P21_2, 0);

        core3++;
        if(core3 > 8*100)//开启计时，五秒以后发车
        {
            if(Camera.CalculateLine.WhichOut == 0)//调试
            {
                control_status = 3;
            }
            if(Camera.CalculateLine.WhichOut == 1 || Camera.CalculateLine.WhichOut == 2)//斑马线发车
            {
                control_status = 2;
            }
            Motor_init();
            ServoMotor_Incrementinit();
        }
    }
    /*******************出库**********************/
    else if(control_status == 2)
    {
        FlyexpectSpeed_out();//串级PID
        FlyMotor_out();//飞轮控制
        expectSpeed_out();//PID速度环
        Motor_out();//电机输出
        core5++;
        core3 = 0;

        if(Camera.CalculateLine.WhichOut == 2)//向右出库
        {
            if(core5 > 1*30)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 60);
            }
            if(core5 > 1*45)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 80);
            }
            if(core5 > 1*60)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 100);
            }
            if(core5 > 1*75)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 120);
            }
            if(core5 > 1*85)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 100);
            }
            if(core5 > 1*100)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 80);
            }
            if(core5 > 1*110)
             {
                 pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 60);
             }
            if(core5 > 1*120)
            {
                TIMESTRAT2 = 1;
                control_status = 3;
            }
        }

        else if(Camera.CalculateLine.WhichOut == 1)//向左出库
        {
            if(core5 > 1*30)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 60);
            }
            if(core5 > 1*45)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 80);
            }
            if(core5 > 1*60)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 100);
            }
            if(core5 > 1*75)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 120);
            }
            if(core5 > 1*85)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 100);
            }
            if(core5 > 1*100)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 80);
            }
            if(core5 > 1*110)
             {
                 pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 60);
             }
            if(core5 > 1*120)
            {
                TIMESTRAT2 = 1;
                control_status = 3;
            }
        }
    }
    /*******************循迹**********************/
    else if(control_status == 3)
    {
        ServoMotor_IncrementOut();
        FlyexpectSpeed_out();//串级PID
        FlyMotor_out();//飞轮控制
        expectSpeed_out();//PID速度环
        Motor_out();//电机输出
        core3 = 0;
        if(Camera.CalculateLine.ElementType == 3)//看到斑马线入库
        {
            core5 = 0;
            control_status = 4;
        }
    }
    /*******************入库**********************/
    else if(control_status == 4)
    {
        Camera.CalculateLine.ElementType = 3;
        core6++;

        if(Camera.CalculateLine.WhichOut == 1)
        {
            FlyMotor.ServomotorQ = -1.2;
        }
        if(Camera.CalculateLine.WhichOut == 2)
        {
            FlyMotor.ServomotorQ = 1.2;
        }

        FlyexpectSpeed_out();//串级PID
        FlyMotor_out();//飞轮控制
        expectSpeed_out();//PID速度环
        Motor_out();//电机输出

        if(Camera.CalculateLine.WhichOut == 2)//向右入库
        {
            if(core6 > 1*0)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 40);
            }
            if(core6 > 1*5)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 50);
            }
            if(core6 > 1*10)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 60);
            }
            if(core6 > 1*15)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 70);
            }
            if(core6 > 1*20)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 80);
            }
            if(core6 > 1*30)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 90);
            }
            if(core6 > 1*40)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 110);
            }
            if(core6 > 1*50)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 125);
            }
            if(core6 > 1*70)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 135);
            }
            if(core6 > 1*80)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 140);
            }
            if(core6 > 1*90)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 140);
            }
            if(core6 > 1*100)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 150);
            }
            if(core6 > 1*120)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle - 150);
            }
            if(core6 > 1*130)
            {
                control_status = 1;
                core3 = 0;
            }
        }
        if(Camera.CalculateLine.WhichOut == 1)//向左入库
        {
            if(core6 > 1*0)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 40);
            }
            if(core6 > 1*5)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 50);
            }
            if(core6 > 1*10)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 60);
            }
            if(core6 > 1*15)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 70);
            }
            if(core6 > 1*20)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 80);
            }
            if(core6 > 1*30)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 90);
            }
            if(core6 > 1*40)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 110);
            }
            if(core6 > 1*50)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 125);
            }
            if(core6 > 1*70)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 135);
            }
            if(core6 > 1*80)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 140);
            }
            if(core6 > 1*90)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 140);
            }
            if(core6 > 1*100)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 150);
            }
            if(core6 > 1*120)
            {
                pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Middle + 150);
            }
            if(core6 > 1*130)
            {
                control_status = 1;
                core3 = 0;
            }
        }
    }
    core2 = 0;
  }
}
else
{
/***************************调试状态**************************************/
  if(core2 == 2)
  {
    GetAngle();
    FlyexpectSpeed_out();//串级PID
    FlyMotor_out();//飞轮控制
    core2 = 0;
  }
}
}

