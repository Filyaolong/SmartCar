/*
 * Motor.h
 *
 *  Created on: 2022年2月23日
 *      Author: Fily
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

typedef struct
{
    int16 ExpectSpeed;    //期望速度
    int16 SetSpeed;
    int16 KalmanExpectSpeed;//上一次的期望速度
    int16 RealSpeed;      //实际速度
    int32 SpeedOut;       //速度输出
    int32 SpeedMAX;       //速度最大值
    float FlySpeedrepd;
    float ServoSpeedrepd;
    int16 ExternSpeed;
}Motor_struct;

typedef struct
{
    float KP;
    float KI;
    float KD;
    float Error;
    float SumError;
    float LastError;
    float PrevError;
    float Output;
    float Target;
}MotorPID;

extern Motor_struct Motor;
extern MotorPID MotorVelocity,MotorAngle;
extern uint8 OPEN;
extern uint16 Expectspeed;
void Motor_REinit(void);
void Motor_init(void);
void expectSpeed_out(void);
void Motor_out(void);
int32 Motor_PID(float P,float I,float D,int32 NowData,int32 Point);
float Motor_AngPID(float P,float I,float D,float NowData,float Point);
#endif /* CODE_MOTOR_H_ */
