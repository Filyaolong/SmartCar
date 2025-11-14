#include "headfile.h"
#include"angle.h"

Kalman My_pitch,My_yaw,My_roll;
OneKalman speed,dir,camera;

void GetAngle(void)
{
    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();

    My_pitch.acc = (float)(atan2((icm_acc_x),(icm_acc_z))*57.297);
    My_pitch.gyro = ((icm_gyro_y)/16.4)*-1;

    My_roll.acc = (float)(atan2((icm_acc_z),(icm_acc_y))*57.297);
    My_roll.gyro = ((icm_gyro_x)/16.4)*-1;

    My_yaw.gyro = 0.45 + ((icm_gyro_z)/16.4)*-1;

    Kalman_Filter(&My_pitch,My_pitch.acc,My_pitch.gyro);
    Kalman_Filter(&My_roll,My_roll.acc,My_roll.gyro);
}


void Init_KalmanValue(Kalman *sptr)
{
    sptr->Q_angle = 0.001;
    sptr->Q_gyro = 0.003;
    sptr->R_angle = 1.2;
    sptr->dt = 0.01;
    sptr->C_0 = 1;
    sptr->gyro_sum = 0;
    sptr->Pdot[0] = 0;
    sptr->Pdot[1] = 0;
    sptr->Pdot[2] = 0;
    sptr->Pdot[3] = 0;
    sptr->PP[0] = 1;
    sptr->PP[1] = 0;
    sptr->PP[2] = 0;
    sptr->PP[3] = 1;
}

void Kalman_Init(OneKalman *sptr)
{
    sptr->Last_P = 1;
    sptr->Now_P = 0;
    sptr->out = 0;
    sptr->Kg = 0;
    sptr->Q = 0.001;
    sptr->R = 1.2;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      一阶Kalman
//  @return     void
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
float OneKalmanFilter(OneKalman *sptr,float input)
{
   //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
    sptr->Now_P = sptr->Last_P + sptr->Q;
   //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
    sptr->Kg = sptr->Now_P / (sptr->Now_P + sptr->R);
   //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
    sptr->out = sptr->out + sptr->Kg * (input -sptr->out);//因为这一次的预测值就是上一次的输出值
   //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
    sptr->Last_P = (1-sptr->Kg) * sptr->Now_P;
   return sptr->out;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Kalman
//  @return     void
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void Kalman_Filter(Kalman *sptr,float Accel,float Gyro) //卡尔曼函数
{
    sptr->Angle += (Gyro - sptr->Q_bias) * sptr->dt; //先验估计

    sptr->Pdot[0] = sptr->Q_angle - sptr->PP[1] - sptr->PP[2]; // Pk-先验估计误差协方差的微分
    sptr->Pdot[1] =-sptr->PP[3];
    sptr->Pdot[2] =-sptr->PP[3];
    sptr->Pdot[3] = sptr->Q_gyro;

    sptr->PP[0] += sptr->Pdot[0] * sptr->dt;   // Pk-先验估计误差协方差微分的积分
    sptr->PP[1] += sptr->Pdot[1] * sptr->dt;   // =先验估计误差协方差
    sptr->PP[2] += sptr->Pdot[2] * sptr->dt;
    sptr->PP[3] += sptr->Pdot[3] * sptr->dt;

    sptr->Angle_err = Accel - sptr->Angle;  //zk-先验估计

    sptr->PCt_0 = sptr->C_0 * sptr->PP[0];
    sptr->PCt_1 = sptr->C_0 * sptr->PP[2];

    sptr->E = sptr->R_angle + sptr->C_0 * sptr->PCt_0;

    sptr->K_0 = sptr->PCt_0 / sptr->E;
    sptr->K_1 = sptr->PCt_1 / sptr->E;

    sptr->t_0 = sptr->PCt_0;
    sptr->t_1 = sptr->C_0 * sptr->PP[1];

    sptr->PP[0] -= sptr->K_0 * sptr->t_0;       //后验估计误差协方差
    sptr->PP[1] -= sptr->K_0 * sptr->t_1;
    sptr->PP[2] -= sptr->K_1 * sptr->t_0;
    sptr->PP[3] -= sptr->K_1 * sptr->t_1;

    sptr->Angle     += sptr->K_0 * sptr->Angle_err;    //后验估计
    sptr->Q_bias    += sptr->K_1 * sptr->Angle_err;    //后验估计
    sptr->Gyro_Final = Gyro - sptr->Q_bias;  //输出值（后验估计）的微分 = 角速度
}

