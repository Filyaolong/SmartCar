/*
 * Motor.c
 *
 *  Created on: 2022年2月23日
 *      Author: Fily
 */
#include "headfile.h"
#include "Motor.h"
#include "ServoMotor.h"
#include "Camera.h"
#include "Angle.h"
#include "Control.h"
#include "MyTool.h"
#include "FlyMotor.h"
Motor_struct Motor;
MotorPID MotorVelocity,MotorAngle;

uint8 OPEN = 0;
uint16 Expectspeed = 0;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机初始化
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Motor_init(void)
{
    gtm_pwm_init(ATOM0_CH0_P21_2, 17000, 0);//电机初始化
    gpio_init(P21_3, GPO, 0, PUSHPULL);
    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);//编码器初始化
    Motor.RealSpeed = 0;
    Motor.SpeedMAX = 7000;
    Motor.SpeedOut = 0;

    Motor.SetSpeed = 120;

    MotorVelocity.KP = 20;
    MotorVelocity.KI = 0.7;
    MotorVelocity.KD = 4;

    MotorAngle.KP = 7;
    MotorAngle.KI = 0;
    MotorAngle.KD = 0;

    MotorVelocity.PrevError = 0;
    MotorVelocity.LastError = 0;
    MotorVelocity.Error = 0;

    MotorAngle.PrevError = 0;
    MotorAngle.LastError = 0;
    MotorAngle.Error = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      速度计算
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void expectSpeed_out(void)//计算速度
{
    Motor.RealSpeed = gpt12_get(GPT12_T4);
    gpt12_clear(GPT12_T4);

    if(Camera.CalculateLine.WhichSpeed == 1)//方案一
    {
        Expectspeed = Motor.SetSpeed + Motor.ExternSpeed;
        if(Camera.CalculateLine.PoDaoElementFinish == 1)
        {
            Expectspeed = 120;
        }
        if(Camera.CalculateLine.SancaElementFinish == 1)
        {
            Expectspeed = 105;
        }
        if(Camera.CalculateLine.ElementType == 3)
        {
            Expectspeed = 100;
        }
        Motor.SpeedOut += Motor_PID(MotorVelocity.KP,MotorVelocity.KI,MotorVelocity.KD,Motor.RealSpeed,Expectspeed);
    }
    else if(Camera.CalculateLine.WhichSpeed == 2)//方案二
    {
        Expectspeed = Motor.SetSpeed + Motor.ExternSpeed;

        if(Camera.CalculateLine.ZhiDao)
        {
            Expectspeed = 200;
        }
        if(Camera.CalculateLine.LongZhiDao)
        {
            Expectspeed = 350;
        }
        if(Camera.CalculateLine.PoDaoElementFinish == 1)
        {
            Expectspeed = 160;
        }
        if(Camera.CalculateLine.SancaElementFinish == 1)
        {
            Expectspeed = 105;
        }
        if(Camera.GetPoint.Right.CircleStep == 6 || Camera.GetPoint.Left.CircleStep == 6)
        {
            Expectspeed = 175;
        }
        if(Camera.CalculateLine.ElementType == 7)
        {
            Expectspeed = 200;
        }
        if(Camera.CalculateLine.ElementType == 3)
        {
            Expectspeed = 115;
        }
        Motor.KalmanExpectSpeed = OneKalmanFilter(&speed,Expectspeed);
        Motor.SpeedOut += Motor_PID(MotorVelocity.KP,MotorVelocity.KI,MotorVelocity.KD,Motor.RealSpeed,Motor.KalmanExpectSpeed);

    }
    if(Motor.SpeedOut > Motor.SpeedMAX)   Motor.SpeedOut = Motor.SpeedMAX;
    if(Motor.SpeedOut < -Motor.SpeedMAX)  Motor.SpeedOut = -Motor.SpeedMAX;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      增量PID闭环
//  @param      int32
//  @return     int32
//  @since
//-------------------------------------------------------------------------------------------------------------------
int32 Motor_PID(float P,float I,float D,int32 NowData,int32 Point)
{
    int32 Realize;                   // 最后得出的实际增量
    MotorVelocity.PrevError = MotorVelocity.LastError;          // 更新前次误差
    MotorVelocity.LastError = MotorVelocity.Error;             // 更新上次误差
    MotorVelocity.Error = Point - NowData;        // 计算当前误差
    Realize = (int32)( P * (MotorVelocity.Error - MotorVelocity.LastError) + I * MotorVelocity.Error + D * (MotorVelocity.Error - 2 * MotorVelocity.LastError + MotorVelocity.PrevError) );
    return Realize;                                // 返回实际值
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      速度前馈PID
//  @param      int32
//  @return     int32
//  @since
//-------------------------------------------------------------------------------------------------------------------
float Motor_AngPID(float P,float I,float D,float NowData,float Point)
{
    float Realize;                   // 最后得出的实际增量
    MotorAngle.LastError = MotorAngle.Error;             // 更新上次误差
    MotorAngle.Error = Point - NowData;        // 计算当前误差
    MotorAngle.SumError += MotorAngle.Error;
    Realize = P * MotorAngle.Error + I * MotorAngle.SumError + D * MotorAngle.LastError;
    return Realize;                                // 返回实际值
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机输出
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Motor_out(void)
{
    if( Motor.SpeedOut >= 0)
    {
        gpio_set(P21_3, 1);
        pwm_duty(ATOM0_CH0_P21_2, Motor.SpeedOut);
    }

    if( Motor.SpeedOut < 0 )
    {
        gpio_set(P21_3, 0);
        pwm_duty(ATOM0_CH0_P21_2, -Motor.SpeedOut);
    }
}




