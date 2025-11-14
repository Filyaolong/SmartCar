/*
 * ServoMotor.h
 *
 *  Created on: 2022年8月5日
 *      Author: Fily
 */

#ifndef CODE_SERVOMOTOR_H_
#define CODE_SERVOMOTOR_H_

typedef struct
{
    float KP;
    float KI;
    float KD;
    float PMAX;
    float IMAX;
    float DMAX;
    float Error;
    float LastError;
    float previousError;
    float Target;
    float DirctionOutput;
    float SumError;

    float ErectKP;
    float ErectKI;
    float ErectKD;
    float ErectError;
    float ErectLastError;
    float ErectpreviousError;
    float ErectSumerror;
    float ErectPrevent;
    float ErectTarget;
    float ErectOutput;

    int16 Middle;
    float CoefficientK;
    float CoefficientQ;
    float Output;
    float LastOutput;
    float KalmanOutput;
}ServoMotorPID;

typedef struct
{
    float kp, kd;
    float L, Lm, V;    //L是前后轮触底点距离，Lm是重心距离后轮的距离，V是前进速度。都是国际单位
    float d_angle;     //角度偏差，目标角度-当前角度，向右倾斜偏差为正，向左倾斜偏差为负
    float ax;          //倒立摆横向加速度，向右是正，向左是负
    float angle;       //方向盘的角度，正前方是0，向右是正，向左是负
    float limit_angle;
    float Output;
}Model;
extern Model Modeling;
extern ServoMotorPID IncrementPID;
float ServoMotor_IncrementPID(float P,float I,float D,float NowData,float Point);
float ServoMotor_ErectPID(float P,float I,float D,float Angle,float ZeroAngle,float Gyro);
void ServoMotor_IncrementOut(void);
void ServoMotor_Incrementinit(void);
void MotorModeling(void);
void MotorModelinginit(void);

#endif /* CODE_SERVOMOTOR_H_ */
