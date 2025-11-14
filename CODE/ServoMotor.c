/*
 * ServoMotor.c
 *
 *  Created on: 2022年8月5日
 *      Author: Fily
 */
#include "headfile.h"
#include "ServoMotor.h"
#include "Camera.h"
#include "Angle.h"
#include "FlyMotor.h"
#include "Motor.h"
#include "MyTool.h"
ServoMotorPID IncrementPID;
Model Modeling;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      位置式打角计算并输出
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void ServoMotor_IncrementOut(void)
{
/*********************************计算压弯角度***********************************/
    if(Camera.CalculateLine.PoDaojishu == 0)
    {
        if(Camera.CalculateLine.WhichSpeed == 1)//方案一
        {
            FlyMotor.ServomotorK = (IncrementPID.Output - IncrementPID.Middle) * IncrementPID.CoefficientK;
            FlyMotor.ServomotorQ = Camera.CalculateLine.FlyCameraFinalerror * IncrementPID.CoefficientQ;
            if(FlyMotor.ServomotorK >= 0.5)
            {
                FlyMotor.ServomotorK = 0.5;
            }
            if(FlyMotor.ServomotorK <= -0.5)
            {
                FlyMotor.ServomotorK = -0.5;
            }
            if(FlyMotor.ServomotorQ >= 0.8)
            {
                FlyMotor.ServomotorQ = 0.8;
            }
            if(FlyMotor.ServomotorQ <= -0.8)
            {
                FlyMotor.ServomotorQ = -0.8;
            }
        }
        else if(Camera.CalculateLine.WhichSpeed == 2)//方案二
        {
            FlyMotor.ServomotorK = (IncrementPID.Output - IncrementPID.Middle) * IncrementPID.CoefficientK;
            FlyMotor.ServomotorQ = Camera.CalculateLine.FlyCameraFinalerror * IncrementPID.CoefficientQ;

            if(FlyMotor.ServomotorK >= 0.7)
            {
                FlyMotor.ServomotorK = 0.7;
            }
            if(FlyMotor.ServomotorK <= -0.7)
            {
                FlyMotor.ServomotorK = -0.7;
            }
            if(FlyMotor.ServomotorQ >= 0.7)
            {
                FlyMotor.ServomotorQ = 0.7;
            }
            if(FlyMotor.ServomotorQ <= -0.7)
            {
                FlyMotor.ServomotorQ = -0.7;
            }
            if(Camera.GetPoint.Right.CircleStep == 5 || Camera.GetPoint.Left.CircleStep == 5
               || Camera.GetPoint.Right.CircleStep == 6 || Camera.GetPoint.Left.CircleStep == 6)
            {
                FlyMotor.ServomotorQ = Camera.CalculateLine.FlyCameraFinalerror * (IncrementPID.CoefficientQ + 0.05);
                if(FlyMotor.ServomotorQ >= 1.2)
                {
                    FlyMotor.ServomotorQ = 1.2;
                }
                if(FlyMotor.ServomotorQ <= -1.2)
                {
                    FlyMotor.ServomotorQ = -1.2;
                }
            }
        }
    }
/*********************************计算舵机打角***********************************/
    IncrementPID.Target = Camera.CalculateLine.Finalerror;
    if(Camera.CalculateLine.WhichSpeed == 1)//方案一
    {
        if(Camera.GetPoint.Left.CircleStep == 4 || Camera.GetPoint.Left.CircleStep == 5)
        {
            IncrementPID.IMAX = 1;
            IncrementPID.PMAX = 40;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.13;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 4 || Camera.GetPoint.Right.CircleStep == 5)
        {
            IncrementPID.IMAX = 1.3;
            IncrementPID.PMAX = 40;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.15;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Left.CircleStep == 6 || Camera.GetPoint.Right.CircleStep == 6)
        {
            IncrementPID.PMAX = 40;
            IncrementPID.IMAX = 0.01;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.01;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 7 || Camera.GetPoint.Left.CircleStep == 7)
        {
            IncrementPID.IMAX = 0.5;
            IncrementPID.PMAX = 30;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.2;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 8 || Camera.GetPoint.Left.CircleStep == 8)
        {
            IncrementPID.IMAX = 0.7;
            IncrementPID.PMAX = 50;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.22;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.CalculateLine.SanCaStep == 2)
        {
            IncrementPID.PMAX = 40;
            IncrementPID.IMAX = 1.7;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.3;
            IncrementPID.KD = 0.1;
        }
        else
        {
            IncrementPID.PMAX = 40;
            IncrementPID.IMAX = 0.1;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.13;
            IncrementPID.KD = 0.1;
        }
        IncrementPID.DirctionOutput = IncrementPID.DirctionOutput + ServoMotor_IncrementPID(IncrementPID.KP,IncrementPID.KI,IncrementPID.KD,IncrementPID.Target,0);
        if(IncrementPID.DirctionOutput > IncrementPID.Middle + 150)
        {
            IncrementPID.DirctionOutput = IncrementPID.Middle + 150;
        }
        if(IncrementPID.DirctionOutput < IncrementPID.Middle - 150)
        {
            IncrementPID.DirctionOutput = IncrementPID.Middle - 150;
        }//限幅
        //方向环限幅
        IncrementPID.Output = IncrementPID.DirctionOutput;
    }

    else if(Camera.CalculateLine.WhichSpeed == 2)//方案一
    {
        if(Camera.GetPoint.Left.CircleStep == 4 || Camera.GetPoint.Left.CircleStep == 5)
        {
            IncrementPID.IMAX = 1;
            IncrementPID.PMAX = 40;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.23;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 4 || Camera.GetPoint.Right.CircleStep == 5)
        {
            IncrementPID.IMAX = 1;
            IncrementPID.PMAX = 40;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.23;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 7 || Camera.GetPoint.Left.CircleStep == 7)
        {
            IncrementPID.IMAX = 0.5;
            IncrementPID.PMAX = 30;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.2;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.GetPoint.Right.CircleStep == 8 || Camera.GetPoint.Left.CircleStep == 8)
        {
            IncrementPID.IMAX = 0.7;
            IncrementPID.PMAX = 50;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.22;
            IncrementPID.KD = 0.1;
        }
        else if(Camera.CalculateLine.SanCaStep == 2)
        {
            IncrementPID.PMAX = 40;
            IncrementPID.IMAX = 1.7;
            IncrementPID.KP = 4;
            IncrementPID.KI = 0.3;
            IncrementPID.KD = 0.1;
        }
        else
        {
            IncrementPID.PMAX = 40;
            IncrementPID.IMAX = 0.1;
            IncrementPID.KP = 3.5;
            IncrementPID.KI = 0.13;
            IncrementPID.KD = 0.1;
        }
        IncrementPID.DirctionOutput = IncrementPID.DirctionOutput + ServoMotor_IncrementPID(IncrementPID.KP,IncrementPID.KI,IncrementPID.KD,IncrementPID.Target,0);
        if(IncrementPID.DirctionOutput > IncrementPID.Middle + 150)
        {
            IncrementPID.DirctionOutput = IncrementPID.Middle + 150;
        }
        if(IncrementPID.DirctionOutput < IncrementPID.Middle - 150)
        {
            IncrementPID.DirctionOutput = IncrementPID.Middle - 150;
        }//限幅
        //方向环限幅
        IncrementPID.Output = IncrementPID.DirctionOutput;
    }

    if(IncrementPID.Output > IncrementPID.Middle + 150)
    {
        IncrementPID.Output = IncrementPID.Middle + 150;
    }
    if(IncrementPID.Output < IncrementPID.Middle - 150)
    {
        IncrementPID.Output = IncrementPID.Middle - 150;
    }//限幅
    pwm_duty(ATOM1_CH1_P33_9,IncrementPID.Output);//输出
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      舵机初始化
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void ServoMotor_Incrementinit(void)
{
    IncrementPID.KP = 3;
    IncrementPID.KI = 0.1;
    IncrementPID.KD = 0;

    IncrementPID.ErectKP = 6;
    IncrementPID.ErectKI = 0;
    IncrementPID.ErectKD = 0;

    IncrementPID.CoefficientK =  0.03;//舵机压弯
    IncrementPID.CoefficientQ =  0.09;//摄像头压弯

    IncrementPID.Middle = 620;
    IncrementPID.DirctionOutput = IncrementPID.Middle;

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      方向环
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
float ServoMotor_IncrementPID(float P,float I,float D,float NowData,float Point)
{
    float Realize;                   // 最后得出的实际增量
    float RealizeA,RealizeB,RealizeC;

    IncrementPID.previousError = IncrementPID.LastError;
    IncrementPID.LastError = IncrementPID.Error;             // 更新上次误差
    IncrementPID.Error = Point - NowData;                    // 计算当前误差

    RealizeA = P * (IncrementPID.Error - IncrementPID.LastError);
    RealizeB = I * IncrementPID.Error;
    RealizeC = D * (IncrementPID.Error - 2 * IncrementPID.LastError + IncrementPID.previousError);


    if(RealizeA > IncrementPID.PMAX)  RealizeA = IncrementPID.PMAX;
    if(RealizeA < -IncrementPID.PMAX) RealizeA = -IncrementPID.PMAX;

    if(RealizeB > IncrementPID.IMAX)  RealizeB = IncrementPID.IMAX;
    if(RealizeB < -IncrementPID.IMAX) RealizeB = -IncrementPID.IMAX;

    if(RealizeC > IncrementPID.DMAX)  RealizeC = IncrementPID.DMAX;
    if(RealizeC < -IncrementPID.DMAX) RealizeC = -IncrementPID.DMAX;


    Realize = RealizeA + RealizeB + RealizeC;

    return Realize;                                // 返回实际值
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      舵机初始化
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void MotorModelinginit(void)
{
    Modeling.kp = 0.1;
    Modeling.kd = 0.01;
    Modeling.L=0.2;
    Modeling.Lm=0.064;
    Modeling.V=0.4;
    Modeling.d_angle = 0;
    Modeling.ax = 0;
    Modeling.angle = 0;
    Modeling.limit_angle = 30;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      MotorModeling
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
void MotorModeling(void)
{
    float temp;
    Modeling.d_angle = FlyMotor.ExpectAngle - FlyMotor.RealAngle;

    Modeling.ax = Modeling.kp*Modeling.d_angle - Modeling.kd*My_pitch.Gyro_Final;
    temp = -Modeling.ax/Modeling.V/Modeling.V*Modeling.Lm;
    if(temp < -1) temp=-1;
    else if(temp > 1) temp=1;
    Modeling.angle = atan((Modeling.L/Modeling.Lm)*tan(asin(temp)))*180/PI;
    if(Modeling.angle > Modeling.limit_angle) Modeling.angle = Modeling.limit_angle;
    else if(Modeling.angle < -Modeling.limit_angle) Modeling.angle = -Modeling.limit_angle;
    Modeling.Output = Modeling.angle;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      直立环
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
float ServoMotor_ErectPID(float P,float I,float D,float Angle,float ZeroAngle,float Gyro)
{
    float Realize;                   // 最后得出的实际增量
    IncrementPID.ErectError = Angle - ZeroAngle;
    IncrementPID.ErectSumerror += IncrementPID.ErectError;

    if(IncrementPID.ErectSumerror > 640)  IncrementPID.ErectSumerror = 640;
    if(IncrementPID.ErectSumerror < -640) IncrementPID.ErectSumerror = -640;

    Realize = P * IncrementPID.ErectError + I * IncrementPID.ErectSumerror + D * Gyro;
    return Realize;
}

