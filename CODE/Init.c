/*
 * Init.c
 *
 *  Created on: 2022年3月12日
 *      Author: Fily
 */
#include "headfile.h"
#include "Init.h"
#include "My_Fun.h"
#include "Motor.h"
#include "FlyMotor.h"
#include "Angle.h"
#include "ServoMotor.h"
#include "Camera.h"
//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Initcar()
{
    //初始化屏幕
    ips114_init();

    //初始化ADC采集
    adc_init(ADC_1, ADC1_CH0_A8);

    //初始化陀螺仪
    icm20602_init_spi();

    Init_KalmanValue(&My_pitch);
    //Init_KalmanValue(&My_yaw);
    Init_KalmanValue(&My_roll);
    Kalman_Init(&speed);
    Kalman_Init(&dir);
    Kalman_Init(&camera);
    //初始化菜单
    MenuInit();

    //初始化无线
    //uart_init(UART_2,460800,UART0_TX_P14_0,UART0_RX_P14_1);
    seekfree_wireless_init();

    //初始化摄像头
    mt9v03x_init();

    CameraCollectInit();

    //初始化舵机
    ServoMotor_Incrementinit();
    MotorModelinginit();

    //初始化电机
    Motor_init();

    //初始化飞轮电机
    FlyMotor_init();

    gtm_pwm_init(ATOM1_CH1_P33_9, 50, IncrementPID.Middle);

    pit_interrupt_ms(CCU6_0, PIT_CH0, 5);
    pit_interrupt_ms(CCU6_1, PIT_CH0, 1);

}
