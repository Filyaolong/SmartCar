#ifndef CODE_ANGLE_H_
#define CODE_ANGLE_H_

typedef struct Kalman
{
        float Q_angle;
        float Q_gyro;
        float R_angle;
        float dt;
        char  C_0;
        float Q_bias, Angle_err;
        float PCt_0, PCt_1, E;
        float K_0, K_1, t_0, t_1;
        float Pdot[4];
        float PP[4];
        float acc;
        float gyro;
        float Angle,Gyro_Final;
        float gyro_sum;
}Kalman;

typedef struct OneKalman
{
    float Last_P;//上次估算协方差 不可以为0 ! ! ! ! !
    float Now_P;//当前估算协方差
    float out;//卡尔曼滤波器输出
    float Kg;//卡尔曼增益
    float Q;//过程噪声协方差
    float R;//观测噪声协方差
}OneKalman;

void Kalman_Init(OneKalman *sptr);
float OneKalmanFilter(OneKalman *sptr,float input);

extern OneKalman speed,dir,camera;
extern Kalman My_pitch,My_yaw,My_roll;    // 当前角度

void GetAngle(void);
void Init_KalmanValue(Kalman *sptr);
void Kalman_Filter(Kalman *sptr,float Accel,float Gyro);
#endif /* CODE_ANGLE_H_ */
