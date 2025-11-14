/*
 * FlyMotor.h
 *
 *  Created on: 2022年2月23日
 *      Author: Fily
 */

#ifndef CODE_FLYMOTOR_H_
#define CODE_FLYMOTOR_H_

typedef struct
{
    float ExpectAngle;    //期望角度
    float RealAngle;      //实际角度
    int32 SpeedOut;       //速度输出
    int32 SpeedMAX;       //速度最大值
    int32 RealSpeed;      //动量轮实际速度
    int32 AngleOutput;    //角度环输出速度
    int32 VelocityOutput;
    float ServomotorK;
    float ServomotorQ;
    int16 FMAX;
}FlyMotor_struct;

typedef struct
{
    float KP;
    float KI;
    float KD;
    float Error;
    float SumError;
    float LastError;
    float Output;
    float KdOutPut;
    float Target;
}FlyPID;

extern FlyMotor_struct FlyMotor;
extern FlyPID FlyVelocity,FlyAngle,FlyGyro;

void FlyMotor_init(void);
void FlyMotor_out(void);
void FlyexpectSpeed_out(void);
void angular_velocity_Control(void);
#endif /* CODE_FLYMOTOR_H_ */
