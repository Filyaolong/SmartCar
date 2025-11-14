/*
 * FlyMotor.c
 *
 *  Created on: 2022年2月23日
 *      Author: Fily
 */
#include "headfile.h"
#include "FlyMotor.h"
#include "Angle.h"
#include "Control.h"
#include "Camera.h"
#include "ServoMotor.h"
#include "MyTool.h"

FlyMotor_struct FlyMotor;
FlyPID FlyVelocity,FlyAngle,FlyGyro;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      飞轮电机初始化
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void FlyMotor_init(void)
{
    gtm_pwm_init(ATOM0_CH2_P21_4, 17000, 0);//电机初始化
    gpio_init(P21_5, GPO, 0, PUSHPULL);
    gpt12_init(GPT12_T2, GPT12_T2INB_P33_7, GPT12_T2EUDB_P33_6);//编码器初始化

    FlyMotor.SpeedMAX = 6500;
    FlyMotor.SpeedOut = 0;
    FlyMotor.ExpectAngle = -3.1;//-1.87（备用）,-2.7

    FlyVelocity.KP = 8.5;  //5，6
    FlyVelocity.KI = 0.1;//0.02，0.04

    FlyAngle.KP = 15;//17.5，18
    FlyAngle.KI = 0.2;//0.1，0.2

    FlyGyro.KP = 165; //185，175
    FlyGyro.KI = 0.2;//0.1，0.1
    FlyGyro.KD = 3;//1，1


}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      飞轮速度计算
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void FlyexpectSpeed_out(void)//计算速度
{
    FlyMotor.RealSpeed = gpt12_get(GPT12_T2);
    gpt12_clear(GPT12_T2);
    FlyMotor.RealAngle = My_pitch.Angle;
    /////// 动量轮控制//////////
    angular_velocity_Control();
    if(FlyMotor.SpeedOut > FlyMotor.SpeedMAX + FlyMotor.FMAX)   FlyMotor.SpeedOut = FlyMotor.SpeedMAX + FlyMotor.FMAX;
    if(FlyMotor.SpeedOut < -FlyMotor.SpeedMAX - FlyMotor.FMAX)  FlyMotor.SpeedOut = -FlyMotor.SpeedMAX - FlyMotor.FMAX;
}

void Velocity_Control()
{
    FlyVelocity.Target = 0;  //设定期望速度
    FlyVelocity.Error =  (float)(FlyVelocity.Target - FlyMotor.RealSpeed);
    FlyVelocity.SumError += FlyVelocity.Error;

    if(FlyVelocity.SumError > +2000) FlyVelocity.SumError = +2000;//限幅
    if(FlyVelocity.SumError < -2000) FlyVelocity.SumError = -2000;//限幅

    FlyVelocity.Output = FlyVelocity.Error * FlyVelocity.KP + FlyVelocity.SumError * FlyVelocity.KI;
    FlyVelocity.Output = FlyVelocity.Output/1000;//1024线编码器
    //限制幅度
    if(FlyVelocity.Output > 5)   FlyVelocity.Output = 5;
    else if(FlyVelocity.Output < -5)  FlyVelocity.Output = -5;                                                   //获取最终数值
}

void angular_Control()
{
     Velocity_Control();
     FlyAngle.Error = (FlyMotor.ExpectAngle - FlyMotor.ServomotorK + FlyMotor.ServomotorQ) - FlyMotor.RealAngle + FlyVelocity.Output;
     FlyAngle.SumError += FlyAngle.Error;
     if(FlyAngle.SumError > +300) FlyAngle.SumError = +300;//限幅
     if(FlyAngle.SumError < -300) FlyAngle.SumError = -300;//限幅
     FlyAngle.KdOutPut = FlyAngle.Error -FlyAngle.LastError;
     FlyAngle.Output = FlyAngle.Error * FlyAngle.KP + FlyAngle.SumError * FlyAngle.KI;
     //获取最终数值
     FlyGyro.LastError = FlyGyro.Error;
}

void angular_velocity_Control()
{
     angular_Control();
     FlyGyro.Error = My_pitch.Gyro_Final - FlyAngle.Output;
     FlyGyro.SumError += FlyGyro.Error;
     if(FlyGyro.SumError > +200) FlyGyro.SumError =+ 200;//限幅
     if(FlyGyro.SumError < -200) FlyGyro.SumError =- 200;//限幅

     FlyMotor.SpeedOut = FlyGyro.KP * FlyGyro.Error + FlyGyro.KI * FlyGyro.SumError + FlyGyro.KD * (FlyGyro.Error -FlyGyro.LastError);
     //获取最终数值
     FlyGyro.LastError = FlyGyro.Error;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      电机输出
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------

void FlyMotor_out(void)
{
    if( FlyMotor.SpeedOut >= 0)
    {
        gpio_set(P21_5, 0);
        pwm_duty(ATOM0_CH2_P21_4, FlyMotor.SpeedOut);
    }

    if( FlyMotor.SpeedOut < 0 )
    {
        gpio_set(P21_5, 1);
        pwm_duty(ATOM0_CH2_P21_4, -FlyMotor.SpeedOut);
    }
}
