 /*
 * Camera.c
 *
 *  Created on: 2022年8月6日
 *      Author: Fily
 */
#include "headfile.h"
#include "Camera.h"
#include "MyTool.h"
#include "isr.h"
#include "Angle.h"
#include "My_Flash.h"
#include "Control.h"
#include "ServoMotor.h"
#include "My_Fun.h"
image Camera;
uint8 Yuansujisu = 0;
//-------------------------------------------------------------------------------------------------------------------
//  @brief      摄像头采集初始化
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void CameraCollectInit(void)
{
    Camera.GetPoint.Bottom = 80;
    Camera.GetPoint.Top = 5;

    Camera.CalculateLine.MiddleLine = (uint8)flash_read(SectorofMiddleLine,PageofMiddleLine,uint32);
    Camera.CalculateLine.threshold = flash_read(SectorofThreshold,PageofThreshold,uint32);
    Camera.CalculateLine.WhichOut = (uint8)flash_read(SectorofOut,PageofOut,uint32);
    Camera.CalculateLine.WhichSpeed = 2;
    Camera.CalculateLine.ElementFinish = 0;
    Camera.CalculateLine.CircleElementFinish = 0;
    Camera.CalculateLine.SancaElementFinish = 0;
    Camera.CalculateLine.PoDaoElementFinish = 0;
    Camera.CalculateLine.ZhiluShiziElementFinish = 0;
    Camera.CalculateLine.RightXieluShiziElementFinish = 0;
    Camera.CalculateLine.LeftXieluShiziElementFinish = 0;

    Camera.CalculateLine.Light = 450;

    Camera.CalculateLine.PoDaojishu = 0;
    Camera.GetPoint.Left.CircleStep = 0;
    Camera.GetPoint.Right.CircleStep = 0;

    Camera.GetPoint.Left.LastFarChangeX = 0;
    Camera.GetPoint.Left.LastFarChangeY = 0;
    Camera.GetPoint.Right.LastFarChangeX = 0;
    Camera.GetPoint.Right.LastFarChangeY = 0;

    Camera.GetPoint.Left.BigPointX = 0;
    Camera.GetPoint.Left.BigPointY = 0;

    Camera.CalculateLine.EndROW = 0;
    Camera.CalculateLine.ReadyROW = 60;
    Camera.CalculateLine.ROWspacing = 30;

    Camera.CalculateLine.ShiziStep = 0;

    Camera.GetPoint.Left.SanCaTop = 0;
    Camera.GetPoint.Left.SanCaBottom = 0;
    Camera.GetPoint.Right.SanCaTop = 0;
    Camera.GetPoint.Right.SanCaBottom = 0;

    Camera.CalculateLine.RunTimes = 1;

    Camera.GuidanceOpen = 0;

    uint8 RemeberW[60] = {0,0,0,0,0,0,0,0,0,0,
                          103,105,109,111,113,116,118,120,122,125,
                          127,129,132,134,136,138,141,143,145,147,
                          150,152,154,157,159,161,163,166,169,171,
                          173,0,0,0,0,0,0,0,0,0,
                          0,0,0,0,0,0,0,0,0,0};
    for(uint8 i=Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;i<Camera.CalculateLine.ReadyROW;i++)//提前设定赛道宽度
    {
        Camera.CalculateLine.Line[i][4] = RemeberW[i-20];
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      计算中线
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void calculatemiddleline(void)
{
    Sideline_scan(Camera.GetPoint.Top,Camera.GetPoint.Bottom,Camera.CalculateLine.MiddleLine);
    NearSuddenlyChangePoint(Camera.GetPoint.Bottom-6,Camera.GetPoint.Top+20);
    FindContinueLine();
    if(control_status == 3)
    {
        JudgmentElementType();
    }
    if(Camera.CalculateLine.CircleElementFinish)
    {
        Circle();
    }
    if(Camera.CalculateLine.SancaElementFinish)
    {
        Threepronged();
    }
    if(Camera.CalculateLine.PoDaoElementFinish)
    {
        PoDao();
    }
    if(Camera.CalculateLine.ZhiluShiziElementFinish)
    {
        ZhiluShizi();
    }
    if(Camera.CalculateLine.LeftXieluShiziElementFinish || Camera.CalculateLine.RightXieluShiziElementFinish)
    {
        XieluShizi();
    }

    if((Camera.CalculateLine.ElementType == 1 ||
       Camera.GetPoint.Left.CircleStep == 1 || Camera.GetPoint.Left.CircleStep == 2 || Camera.GetPoint.Left.CircleStep == 3 ||
       Camera.GetPoint.Right.CircleStep == 1 || Camera.GetPoint.Right.CircleStep == 2 || Camera.GetPoint.Right.CircleStep == 3 ||
       Camera.CalculateLine.RightXieluShiziElementFinish == 1 || Camera.CalculateLine.ZhiluShiziElementFinish == 1 || Camera.CalculateLine.ElementType == 7)
       && Camera.CalculateLine.image[6][94] == 0xff && Camera.CalculateLine.image[7][94] == 0xff
       && Camera.CalculateLine.image[8][94] == 0xff && Camera.CalculateLine.image[9][94] == 0xff)
    {
        Camera.CalculateLine.ZhiDao = 1;
    }

    if(Camera.CalculateLine.ElementType == 1 && Camera.CalculateLine.ZhiDao == 1
       && Camera.CalculateLine.image[7][94] == 0xff && Camera.CalculateLine.image[4][94] == 0xff
       && Camera.CalculateLine.image[5][94] == 0xff && Camera.CalculateLine.image[6][94] == 0xff
       && Camera.CalculateLine.image[7][104] == 0xff && Camera.CalculateLine.image[4][104] == 0xff
       && Camera.CalculateLine.image[5][104] == 0xff && Camera.CalculateLine.image[6][104] == 0xff
       && Camera.CalculateLine.image[7][84] == 0xff && Camera.CalculateLine.image[4][84] == 0xff
       && Camera.CalculateLine.image[5][84] == 0xff && Camera.CalculateLine.image[6][84] == 0xff
       && Camera.GetPoint.Left.ContinueLine == 1 && Camera.GetPoint.Right.ContinueLine == 1)
    {
        Camera.CalculateLine.LongZhiDao = 1;
        Camera.CalculateLine.ZhiDao = 0;
    }

    CalculateMiddleHighLine();
    CalclulateError();
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      JudgmentElementType
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void CalclulateError(void)
{

    Camera.CalculateLine.MiddleLine = (uint8)flash_read(SectorofMiddleLine,PageofMiddleLine,uint32);

    if(Camera.CalculateLine.PoDaojishu == 1)
    {
        Camera.CalculateLine.ReadyROW = 80;
        Camera.CalculateLine.ROWspacing = 10;
        Camera.CalculateLine.EndROW = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;
    }
    else if(Camera.CalculateLine.PoDaojishu == 2)
    {
        Camera.CalculateLine.ReadyROW = 60;
        Camera.CalculateLine.ROWspacing = 10;
        Camera.CalculateLine.EndROW = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;
    }
    else
    {
        Camera.CalculateLine.ReadyROW = 60;
        Camera.CalculateLine.ROWspacing = 30;
        Camera.CalculateLine.EndROW = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;
    }
    for(uint8 i=Camera.CalculateLine.ReadyROW;i>Camera.CalculateLine.EndROW;i--)//计算中线
    {
         Camera.CalculateLine.Line[i][1] = Camera.CalculateLine.Line[i][0] / 2 + Camera.CalculateLine.Line[i][2] / 2;
    }
    for(uint8 i=Camera.CalculateLine.ReadyROW;i>Camera.CalculateLine.EndROW;i--)//计算平均中线
    {
         Camera.CalculateLine.error += Camera.CalculateLine.Line[i][1] - Camera.CalculateLine.MiddleLine;
    }

    for(uint8 i=35;i>30;i--)
    {
        Camera.CalculateLine.FlyCameraerror += (Camera.CalculateLine.Line[i][0] / 2 + Camera.CalculateLine.Line[i][2] / 2) - Camera.CalculateLine.MiddleLine;
    }
    Camera.CalculateLine.FlyCameraFinalerror = Camera.CalculateLine.FlyCameraerror / 5;
    //Camera.CalculateLine.FlyCameraFinalerror = OneKalmanFilter(&camera,Camera.CalculateLine.FlyCameraFinalerror);
    Camera.CalculateLine.Finalerror = (float)(Camera.CalculateLine.error / (Camera.CalculateLine.ReadyROW - Camera.CalculateLine.EndROW));
    Camera.CalculateLine.error = 0;
    Camera.CalculateLine.FlyCameraerror = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      JudgmentElementType
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void JudgmentElementType(void)
{
    Camera.CalculateLine.ZhiDao = 0;
    Camera.CalculateLine.LongZhiDao = 0;
    /*********************保底方案****************/
    if(Camera.CalculateLine.WhichSpeed == 1)
    {
        JudgmentWidth();
        Camera.GetPoint.Left.Slope1 = Getslope(Camera.CalculateLine.Line[10][0],10,Camera.CalculateLine.Line[40][0],40);
        Camera.GetPoint.Left.Slope2 = Getslope(Camera.CalculateLine.Line[20][0],20,Camera.CalculateLine.Line[50][0],50);
        Camera.GetPoint.Left.Slope3 = Getslope(Camera.CalculateLine.Line[30][0],30,Camera.CalculateLine.Line[60][0],60);
        Camera.GetPoint.Right.Slope1 = Getslope(Camera.CalculateLine.Line[10][2],10,Camera.CalculateLine.Line[40][2],40);
        Camera.GetPoint.Right.Slope2 = Getslope(Camera.CalculateLine.Line[20][2],20,Camera.CalculateLine.Line[50][2],50);
        Camera.GetPoint.Right.Slope3 = Getslope(Camera.CalculateLine.Line[30][2],30,Camera.CalculateLine.Line[60][2],60);
        //啥也不是
        if(!Camera.CalculateLine.ElementFinish)
        {
            Camera.CalculateLine.ElementType = 0;
        }
        //直线
        if(!Camera.CalculateLine.ElementFinish && Camera.GetPoint.Left.LostNum < 5 && Camera.GetPoint.Right.LostNum < 5)
        {
            Camera.CalculateLine.ElementType = 1;
        }
        //弯道
        if((Camera.CalculateLine.Line[10][0] > Camera.CalculateLine.MiddleLine + 20) || (Camera.CalculateLine.Line[10][2] < Camera.CalculateLine.MiddleLine - 20))
        {
            Camera.CalculateLine.ElementType = 2;
        }
        //坡道
        if(YuanSuXuWei[Yuansujisu] == 8 && My_roll.Angle > 96 && My_roll.Angle < 150 && Camera.CalculateLine.PoDaoElementFinish == 0)
        {
            Camera.CalculateLine.PoDaojishu = 1;
            Camera.CalculateLine.PoDaoElementFinish = 1;
        }
        if(Camera.CalculateLine.PoDaojishu == 1)
        {
            Camera.CalculateLine.ElementType = 8;
        }
        //斑马线
        if(YuanSuXuWei[Yuansujisu] == 3 && ZebraCrossing())
        {
            Camera.CalculateLine.ElementType = 3;
            TIMESTRAT1 = 0;
        }
        //左圆环
        if(YuanSuXuWei[Yuansujisu] == 4 && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.ElementType != 6 && Camera.CalculateLine.ElementType != 7 && Camera.CalculateLine.ElementType != 8 && !Camera.CalculateLine.ElementFinish && !Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.NearChangeX && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.image[40][94] == 0xFF && Camera.CalculateLine.image[45][94] == 0xFF)
        {
            if((Camera.GetPoint.Right.LostNum < 20
               && Camera.GetPoint.Right.Slope1 > 0 && Camera.GetPoint.Right.Slope2 > 0 && Camera.GetPoint.Right.Slope3 > 0
               && Camera.GetPoint.Right.Slope1 != 0 && Camera.GetPoint.Right.Slope2 != 0 && Camera.GetPoint.Right.Slope3 != 0
               && Camera.GetPoint.BecomeWidthNum >= 5 && My_abs(Camera.GetPoint.Right.Slope1 - Camera.GetPoint.Right.Slope2) < 0.3
               && My_abs(Camera.GetPoint.Right.Slope2 - Camera.GetPoint.Right.Slope3) < 0.5
               && My_abs(Camera.GetPoint.Right.Slope1 - Camera.GetPoint.Right.Slope3) < 0.5
               && (Camera.GetPoint.Right.Slope2 > 0.6 && Camera.GetPoint.Right.Slope2 < 1.3)
               && (Camera.GetPoint.Right.Slope1 > 0.6 && Camera.GetPoint.Right.Slope1 < 1.3)
               ) || (Camera.GetPoint.Right.ContinueLine == 1)//斜入
               )
            {
                Camera.CalculateLine.ElementType = 4;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.CircleElementFinish = 1;
                Camera.GetPoint.Right.ContinueLine = 0;
            }
        }
        //右圆环
        if(YuanSuXuWei[Yuansujisu] == 5 && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.ElementType != 6 && Camera.CalculateLine.ElementType != 7 && Camera.CalculateLine.ElementType != 8 && !Camera.CalculateLine.ElementFinish && !Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Right.NearChangeX && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.image[40][94] == 0xFF && Camera.CalculateLine.image[45][94] == 0xFF)
        {
            if((Camera.GetPoint.Left.LostNum < 20
                && Camera.GetPoint.Left.Slope1 < 0 && Camera.GetPoint.Left.Slope2 < 0 && Camera.GetPoint.Left.Slope3 < 0
                && Camera.GetPoint.Left.Slope1 != 0 && Camera.GetPoint.Left.Slope2 != 0 && Camera.GetPoint.Left.Slope3 != 0
                && Camera.GetPoint.BecomeWidthNum >= 5 && My_abs(Camera.GetPoint.Left.Slope1 - Camera.GetPoint.Left.Slope2) < 0.3
                && My_abs(Camera.GetPoint.Left.Slope2 - Camera.GetPoint.Left.Slope3) < 0.5
                && My_abs(Camera.GetPoint.Left.Slope1 - Camera.GetPoint.Left.Slope3) < 0.5
                && (Camera.GetPoint.Left.Slope1 < -0.6 && Camera.GetPoint.Left.Slope1 > -1.3)
                && (Camera.GetPoint.Left.Slope2 < -0.6 && Camera.GetPoint.Left.Slope2 > -1.3)
                ) || (Camera.GetPoint.Left.ContinueLine == 1)//斜入
                )
            {
                Camera.CalculateLine.ElementType = 5;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.CircleElementFinish = 1;
                Camera.GetPoint.Left.ContinueLine = 0;
            }
        }
        //三叉(斜入也能识别)
        if((YuanSuXuWei[Yuansujisu] == 6 || YuanSuXuWei[Yuansujisu] == 9)&& Camera.CalculateLine.ElementFinish == 0 && Camera.CalculateLine.SancaElementFinish == 0 && !Camera.CalculateLine.ElementFinish && (Camera.GetPoint.Left.NearChangeX || Camera.GetPoint.Right.NearChangeX))
        {
            FindTwoPoint();
            Camera.GetPoint.Left.Angle = Get_angle(Camera.CalculateLine.Line[Camera.GetPoint.Left.SanCaTop][0] , Camera.GetPoint.Left.SanCaTop , Camera.GetPoint.Left.NearChangeX , Camera.GetPoint.Left.NearChangeY , Camera.CalculateLine.Line[Camera.GetPoint.Left.SanCaBottom][0] , Camera.GetPoint.Left.SanCaBottom);
            Camera.GetPoint.Right.Angle = Get_angle(Camera.CalculateLine.Line[Camera.GetPoint.Right.SanCaTop][2] , Camera.GetPoint.Right.SanCaTop , Camera.GetPoint.Right.NearChangeX , Camera.GetPoint.Right.NearChangeY , Camera.CalculateLine.Line[Camera.GetPoint.Right.SanCaBottom][2] , Camera.GetPoint.Right.SanCaBottom);
            if((Camera.GetPoint.Left.Angle > 65 && Camera.GetPoint.Left.Angle < 105) || (Camera.GetPoint.Right.Angle > 65 && Camera.GetPoint.Right.Angle < 105))
            {
                Camera.CalculateLine.ElementType = 6;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.SancaElementFinish = 1;
            }
        }
        //正入十字
        if(YuanSuXuWei[Yuansujisu] == 7  && Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.NearChangeX && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Left.LostNum > 10 && Camera.GetPoint.Right.LostNum > 10)
        {
            if(Camera.GetPoint.Left.FarChangeY && Camera.GetPoint.Right.FarChangeY)
            {
                Camera.CalculateLine.ElementType = 7;
                Camera.CalculateLine.ZhiluShiziElementFinish = 1;
                Camera.CalculateLine.ElementFinish = 1;
            }
        }
        //只有右边拐点斜入十字
        if(YuanSuXuWei[Yuansujisu] == 7 && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.LostNum > 25)
        {
            Camera.CalculateLine.ElementType = 7;
            Camera.CalculateLine.RightXieluShiziElementFinish = 1;
            Camera.CalculateLine.ElementFinish = 1;
        }
        //只有左边拐点斜入十字
        if(YuanSuXuWei[Yuansujisu] == 7 && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Right.LostNum > 25)
        {
            Camera.CalculateLine.ElementType = 7;
            Camera.CalculateLine.LeftXieluShiziElementFinish = 1;
            Camera.CalculateLine.ElementFinish = 1;
        }
    }

    /********************加速方案******************/
    if(Camera.CalculateLine.WhichSpeed == 2)
    {
        JudgmentWidth();
        Camera.GetPoint.Left.Slope1 = Getslope(Camera.CalculateLine.Line[10][0],10,Camera.CalculateLine.Line[40][0],40);
        Camera.GetPoint.Left.Slope2 = Getslope(Camera.CalculateLine.Line[20][0],20,Camera.CalculateLine.Line[50][0],50);
        Camera.GetPoint.Left.Slope3 = Getslope(Camera.CalculateLine.Line[30][0],30,Camera.CalculateLine.Line[60][0],60);
        Camera.GetPoint.Right.Slope1 = Getslope(Camera.CalculateLine.Line[10][2],10,Camera.CalculateLine.Line[40][2],40);
        Camera.GetPoint.Right.Slope2 = Getslope(Camera.CalculateLine.Line[20][2],20,Camera.CalculateLine.Line[50][2],50);
        Camera.GetPoint.Right.Slope3 = Getslope(Camera.CalculateLine.Line[30][2],30,Camera.CalculateLine.Line[60][2],60);
        //啥也不是
        if(!Camera.CalculateLine.ElementFinish)
        {
            Camera.CalculateLine.ElementType = 0;
        }
        //直线
        if(!Camera.CalculateLine.ElementFinish && Camera.GetPoint.Left.LostNum < 5 && Camera.GetPoint.Right.LostNum < 5)
        {
            Camera.CalculateLine.ElementType = 1;
        }
        //弯道
        if((Camera.CalculateLine.Line[10][0] > Camera.CalculateLine.MiddleLine + 20) || (Camera.CalculateLine.Line[10][2] < Camera.CalculateLine.MiddleLine - 20))
        {
            Camera.CalculateLine.ElementType = 2;
        }
        //坡道
        if(YuanSuXuWei[Yuansujisu] == 8 && My_roll.Angle > 96 && My_roll.Angle < 150 && Camera.CalculateLine.PoDaoElementFinish == 0)
        {
            Camera.CalculateLine.PoDaojishu = 1;
            Camera.CalculateLine.PoDaoElementFinish = 1;
        }
        if(Camera.CalculateLine.PoDaojishu == 1)
        {
            Camera.CalculateLine.ElementType = 8;
        }
        //斑马线
        if(YuanSuXuWei[Yuansujisu] == 3 && ZebraCrossing())
        {
            Camera.CalculateLine.ElementType = 3;
            TIMESTRAT1 = 0;
        }
        //左圆环
        if(YuanSuXuWei[Yuansujisu] == 4 && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.ElementType != 6 && Camera.CalculateLine.ElementType != 7 && Camera.CalculateLine.ElementType != 8 && !Camera.CalculateLine.ElementFinish && !Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.NearChangeX && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.image[40][94] == 0xFF && Camera.CalculateLine.image[45][94] == 0xFF)
        {
            if((Camera.GetPoint.Right.LostNum < 20
               && Camera.GetPoint.Right.Slope1 > 0 && Camera.GetPoint.Right.Slope2 > 0 && Camera.GetPoint.Right.Slope3 > 0
               && Camera.GetPoint.Right.Slope1 != 0 && Camera.GetPoint.Right.Slope2 != 0 && Camera.GetPoint.Right.Slope3 != 0
               && Camera.GetPoint.BecomeWidthNum >= 5 && My_abs(Camera.GetPoint.Right.Slope1 - Camera.GetPoint.Right.Slope2) < 0.3
               && My_abs(Camera.GetPoint.Right.Slope2 - Camera.GetPoint.Right.Slope3) < 0.5
               && My_abs(Camera.GetPoint.Right.Slope1 - Camera.GetPoint.Right.Slope3) < 0.5
               && (Camera.GetPoint.Right.Slope2 > 0.6 && Camera.GetPoint.Right.Slope2 < 1.3)
               && (Camera.GetPoint.Right.Slope1 > 0.6 && Camera.GetPoint.Right.Slope1 < 1.3)
               ) || (Camera.GetPoint.Right.ContinueLine == 1)//斜入
               )
            {
                Camera.CalculateLine.ElementType = 4;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.CircleElementFinish = 1;
                Camera.GetPoint.Right.ContinueLine = 0;
            }
        }
        //右圆环
        if(YuanSuXuWei[Yuansujisu] == 5 && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.ElementType != 6 && Camera.CalculateLine.ElementType != 7 && Camera.CalculateLine.ElementType != 8 && !Camera.CalculateLine.ElementFinish && !Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Right.NearChangeX && Camera.CalculateLine.ElementType != 3 && Camera.CalculateLine.image[40][94] == 0xFF && Camera.CalculateLine.image[45][94] == 0xFF)
        {
            if((Camera.GetPoint.Left.LostNum < 20
                && Camera.GetPoint.Left.Slope1 < 0 && Camera.GetPoint.Left.Slope2 < 0 && Camera.GetPoint.Left.Slope3 < 0
                && Camera.GetPoint.Left.Slope1 != 0 && Camera.GetPoint.Left.Slope2 != 0 && Camera.GetPoint.Left.Slope3 != 0
                && Camera.GetPoint.BecomeWidthNum >= 5 && My_abs(Camera.GetPoint.Left.Slope1 - Camera.GetPoint.Left.Slope2) < 0.3
                && My_abs(Camera.GetPoint.Left.Slope2 - Camera.GetPoint.Left.Slope3) < 0.5
                && My_abs(Camera.GetPoint.Left.Slope1 - Camera.GetPoint.Left.Slope3) < 0.5
                && (Camera.GetPoint.Left.Slope1 < -0.6 && Camera.GetPoint.Left.Slope1 > -1.3)
                && (Camera.GetPoint.Left.Slope2 < -0.6 && Camera.GetPoint.Left.Slope2 > -1.3)
                ) || (Camera.GetPoint.Left.ContinueLine == 1)//斜入
                )
            {
                Camera.CalculateLine.ElementType = 5;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.CircleElementFinish = 1;
                Camera.GetPoint.Left.ContinueLine = 0;
            }
        }
        //三叉(斜入也能识别)
        if((YuanSuXuWei[Yuansujisu] == 6 || YuanSuXuWei[Yuansujisu] == 9)&& Camera.CalculateLine.ElementFinish == 0 && Camera.CalculateLine.SancaElementFinish == 0 && !Camera.CalculateLine.ElementFinish && (Camera.GetPoint.Left.NearChangeX || Camera.GetPoint.Right.NearChangeX))
        {
            FindTwoPoint();
            Camera.GetPoint.Left.Angle = Get_angle(Camera.CalculateLine.Line[Camera.GetPoint.Left.SanCaTop][0] , Camera.GetPoint.Left.SanCaTop , Camera.GetPoint.Left.NearChangeX , Camera.GetPoint.Left.NearChangeY , Camera.CalculateLine.Line[Camera.GetPoint.Left.SanCaBottom][0] , Camera.GetPoint.Left.SanCaBottom);
            Camera.GetPoint.Right.Angle = Get_angle(Camera.CalculateLine.Line[Camera.GetPoint.Right.SanCaTop][2] , Camera.GetPoint.Right.SanCaTop , Camera.GetPoint.Right.NearChangeX , Camera.GetPoint.Right.NearChangeY , Camera.CalculateLine.Line[Camera.GetPoint.Right.SanCaBottom][2] , Camera.GetPoint.Right.SanCaBottom);
            if((Camera.GetPoint.Left.Angle > 65 && Camera.GetPoint.Left.Angle < 105) || (Camera.GetPoint.Right.Angle > 65 && Camera.GetPoint.Right.Angle < 105))
            {
                Camera.CalculateLine.ElementType = 6;
                Camera.CalculateLine.ElementFinish = 1;
                Camera.CalculateLine.SancaElementFinish = 1;
            }
        }
        //正入十字
        if(YuanSuXuWei[Yuansujisu] == 7  && Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.NearChangeX && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Left.LostNum > 10 && Camera.GetPoint.Right.LostNum > 10)
        {
            if(Camera.GetPoint.Left.FarChangeY && Camera.GetPoint.Right.FarChangeY)
            {
                Camera.CalculateLine.ElementType = 7;
                Camera.CalculateLine.ZhiluShiziElementFinish = 1;
                Camera.CalculateLine.ElementFinish = 1;
            }
        }
        //只有右边拐点斜入十字
        if(YuanSuXuWei[Yuansujisu] == 7 && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Left.LostNum > 25)
        {
            Camera.CalculateLine.ElementType = 7;
            Camera.CalculateLine.RightXieluShiziElementFinish = 1;
            Camera.CalculateLine.ElementFinish = 1;
        }
        //只有左边拐点斜入十字
        if(YuanSuXuWei[Yuansujisu] == 7 && Camera.CalculateLine.ElementFinish == 0 && Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Right.LostNum > 25)
        {
            Camera.CalculateLine.ElementType = 7;
            Camera.CalculateLine.LeftXieluShiziElementFinish = 1;
            Camera.CalculateLine.ElementFinish = 1;
        }
    }

}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      CalculateCalculate
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void CalculateMiddleHighLine(void)
{
    Camera.GetPoint.Left.MiddleHighLine = 0;
    Camera.GetPoint.Right.MiddleHighLine = 0;
    for(uint8 i=Camera.GetPoint.Bottom; i>Camera.GetPoint.Top; i--)
    {
        if(Camera.CalculateLine.image[i][Camera.CalculateLine.MiddleLine-20] == 0xff && Camera.CalculateLine.image[i-1][Camera.CalculateLine.MiddleLine-20] == 0xff && Camera.CalculateLine.image[i-3][Camera.CalculateLine.MiddleLine-20] == 0x00)
        {
            Camera.GetPoint.Left.MiddleHighLine = i;
            break;
        }
        else Camera.GetPoint.Left.MiddleHighLine = 0;
    }
    for(uint8 i=Camera.GetPoint.Bottom; i>Camera.GetPoint.Top; i--)
    {
        if(Camera.CalculateLine.image[i][Camera.CalculateLine.MiddleLine+20] == 0xff && Camera.CalculateLine.image[i-1][Camera.CalculateLine.MiddleLine+20] == 0xff && Camera.CalculateLine.image[i-3][Camera.CalculateLine.MiddleLine+20] == 0x00)
        {
            Camera.GetPoint.Right.MiddleHighLine = i;
            break;
        }
        else Camera.GetPoint.Right.MiddleHighLine = 0;
    }

}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      Shizi
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void ZhiluShizi(void)
{
    FarSuddenlyChangePoint(Camera.GetPoint.Bottom-30,Camera.GetPoint.Top+5,1);
    FarSuddenlyChangePoint(Camera.GetPoint.Bottom-30,Camera.GetPoint.Top+5,0);
    if(Camera.GetPoint.Left.FarChangeY)
    {
        Fill_line(Camera.GetPoint.Left.FarChangeX,Camera.GetPoint.Left.FarChangeY,1,60);//拐点前半部分补线
        Camera.GetPoint.Left.LastFarChangeX = Camera.GetPoint.Left.FarChangeX;
        Camera.GetPoint.Left.LastFarChangeY = Camera.GetPoint.Left.FarChangeY;
    }
    else
    {
        Fill_line(Camera.GetPoint.Left.LastFarChangeX,Camera.GetPoint.Left.LastFarChangeY,1,60);//拐点前半部分补线
    }
    if(Camera.GetPoint.Right.FarChangeY)
    {
        Fill_line(187,60,Camera.GetPoint.Right.FarChangeX,Camera.GetPoint.Right.FarChangeY);//拐点前半部分补线
        Camera.GetPoint.Right.LastFarChangeX = Camera.GetPoint.Right.FarChangeX;
        Camera.GetPoint.Right.LastFarChangeY = Camera.GetPoint.Right.FarChangeY;
    }
    else
    {
        Fill_line(187,60,Camera.GetPoint.Right.LastFarChangeX,Camera.GetPoint.Right.LastFarChangeY);//拐点前半部分补线
    }

    if(Camera.GetPoint.Right.LostNum > 20 && Camera.GetPoint.Left.LostNum > 20)
    {
        Camera.CalculateLine.ShiziStep = 1;
    }
    if((Camera.GetPoint.Right.LostNum < 5 || Camera.GetPoint.Left.LostNum < 5) && Camera.CalculateLine.ShiziStep == 1)
    {
        Camera.CalculateLine.ZhiluShiziElementFinish = 0;
        Camera.CalculateLine.ElementFinish = 0;

        Camera.CalculateLine.ShiziStep = 0;


        Camera.GetPoint.Left.FarChangeX = 0;
        Camera.GetPoint.Left.FarChangeY = 0;
        Camera.GetPoint.Right.FarChangeX = 0;
        Camera.GetPoint.Right.FarChangeY = 0;
        Camera.GetPoint.Right.LastFarChangeX = 0;
        Camera.GetPoint.Right.LastFarChangeY = 0;
        Camera.GetPoint.Left.LastFarChangeX = 0;
        Camera.GetPoint.Left.LastFarChangeY = 0;

        Yuansujisu++;
    }

}

uint8 Zhilushizi(uint8 StartLine,uint8 EndLine)//前30行识别出来效果最佳
{//先判断是否丢线,丢线的上面有一个突变点即可判断十字
    ShiziFarSuddenlyChangePoint(StartLine,Camera.GetPoint.Top+3,0);
    ShiziFarSuddenlyChangePoint(StartLine,Camera.GetPoint.Top+3,1);
    uint8 SHIZI = 0;
    if(Camera.GetPoint.Left.ShiziFarChangeY && Camera.GetPoint.Right.ShiziFarChangeY)
    {
        SHIZI = 1;
    }
    if(!Camera.GetPoint.Left.ShiziFarChangeY && !Camera.GetPoint.Right.ShiziFarChangeY)
    {
        SHIZI = 0;
    }
    return SHIZI;
}

void XieluShizi(void)
{
    if(!Camera.CalculateLine.LeftXieluShiziElementFinish && Camera.CalculateLine.RightXieluShiziElementFinish)//看得到右拐点斜入
    {
        if(Camera.GetPoint.Right.NearChangeX && Camera.CalculateLine.ShiziStep == 0)
        {
            Fill_line(Camera.GetPoint.Right.NearChangeX,Camera.GetPoint.Right.NearChangeY,Camera.GetPoint.Right.NearChangeX-30,5);//拐点前半部分补线
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW; i++)
            {
                if(Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4] < 1)
                {
                    Camera.CalculateLine.Line[i][0] = 1;
                }
                else Camera.CalculateLine.Line[i][0] = Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4];
            }
        }
        if(!Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Right.LostNum > 15 && Camera.GetPoint.Left.LostNum > 15)
        {
            Camera.CalculateLine.ShiziStep = 1;
        }
        if(Camera.CalculateLine.ShiziStep == 1)
        {
            FarSuddenlyChangePoint(Camera.GetPoint.Bottom-30,Camera.GetPoint.Top+5,1);
            if(Camera.GetPoint.Right.FarChangeY && Camera.GetPoint.Right.FarChangeX)
            {
                Camera.GetPoint.Right.LastFarChangeX = Camera.GetPoint.Right.FarChangeX;
                Camera.GetPoint.Right.LastFarChangeY = Camera.GetPoint.Right.FarChangeY;
            }
        }
        if(Camera.CalculateLine.ShiziStep == 1 && Camera.GetPoint.Right.FarChangeX)
        {
            Fill_line(187,80,Camera.GetPoint.Right.FarChangeX,Camera.GetPoint.Right.FarChangeY);//补右线，下面用右平移来补左边线
        }
        if(Camera.CalculateLine.ShiziStep == 1 && !Camera.GetPoint.Right.FarChangeX && Camera.GetPoint.Right.LastFarChangeX)
        {
            Fill_line(187,80,Camera.GetPoint.Right.LastFarChangeX,Camera.GetPoint.Right.LastFarChangeY);//补右线，下面用右平移来补左边线
        }
        if(Camera.CalculateLine.ShiziStep == 1)
        {
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW ; i++)
            {
                if(Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4] < 1)
                {
                    Camera.CalculateLine.Line[i][0] = 1;
                }
                else Camera.CalculateLine.Line[i][0] = Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4];
            }
        }
        if(Camera.GetPoint.Right.LostNum < 15 && Camera.CalculateLine.ShiziStep == 1)
        {//十字结束
            Camera.CalculateLine.RightXieluShiziElementFinish = 0;
            Camera.CalculateLine.ShiziStep = 0;
            Camera.CalculateLine.ElementFinish = 0;
            Camera.GetPoint.Right.LastFarChangeX = 0;
            Camera.GetPoint.Right.LastFarChangeY = 0;
            Camera.GetPoint.Left.LastFarChangeX = 0;
            Camera.GetPoint.Left.LastFarChangeY = 0;
            Yuansujisu++;
        }
    }

    if(Camera.CalculateLine.LeftXieluShiziElementFinish && !Camera.CalculateLine.RightXieluShiziElementFinish)//看得到左拐点斜入
    {
        if(Camera.GetPoint.Left.NearChangeX && Camera.CalculateLine.ShiziStep == 0)
        {
            Fill_line(Camera.GetPoint.Left.NearChangeX+30,5,Camera.GetPoint.Left.NearChangeX,Camera.GetPoint.Left.NearChangeY);//拐点前半部分补线
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW; i++)
            {
                if(Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4] > 187)
                {
                    Camera.CalculateLine.Line[i][2] = 187;
                }
                else Camera.CalculateLine.Line[i][2] = Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4];
            }
        }
        if(!Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Right.LostNum > 15 && Camera.GetPoint.Left.LostNum > 15)
        {
            Camera.CalculateLine.ShiziStep = 1;
        }
        if(Camera.CalculateLine.ShiziStep == 1)
        {
            FarSuddenlyChangePoint(Camera.GetPoint.Bottom-30,Camera.GetPoint.Top+5,0);
            if(Camera.GetPoint.Left.FarChangeY && Camera.GetPoint.Left.FarChangeX)
            {
                Camera.GetPoint.Left.LastFarChangeX = Camera.GetPoint.Left.FarChangeX;
                Camera.GetPoint.Left.LastFarChangeY = Camera.GetPoint.Left.FarChangeY;
            }
        }
        if(Camera.CalculateLine.ShiziStep == 1 && Camera.GetPoint.Left.FarChangeX)
        {
            Fill_line(Camera.GetPoint.Left.FarChangeX,Camera.GetPoint.Left.FarChangeY,1,80);//补右线，下面用右平移来补左边线
        }
        if(Camera.CalculateLine.ShiziStep == 1 && !Camera.GetPoint.Left.FarChangeX && Camera.GetPoint.Left.LastFarChangeX)
        {
            Fill_line(Camera.GetPoint.Left.LastFarChangeX,Camera.GetPoint.Left.LastFarChangeY,1,80);//补右线，下面用右平移来补左边线
        }
        if(Camera.CalculateLine.ShiziStep == 1)
        {
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW ; i++)
            {
                if(Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4] > 187)
                {
                    Camera.CalculateLine.Line[i][2] = 187;
                }
                else Camera.CalculateLine.Line[i][2] = Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4];
            }
        }
        if(Camera.GetPoint.Right.LostNum < 15 && Camera.CalculateLine.ShiziStep == 1)
        {//十字结束
            Camera.CalculateLine.LeftXieluShiziElementFinish = 0;
            Camera.CalculateLine.ShiziStep = 0;
            Camera.CalculateLine.ElementFinish = 0;
            Camera.GetPoint.Right.LastFarChangeX = 0;
            Camera.GetPoint.Right.LastFarChangeY = 0;
            Camera.GetPoint.Left.LastFarChangeX = 0;
            Camera.GetPoint.Left.LastFarChangeY = 0;
            Yuansujisu++;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      PoDao
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void PoDao(void)
{
    if(Camera.CalculateLine.PoDaojishu == 1 && My_roll.Angle > 30 && My_roll.Angle < 78 && Camera.CalculateLine.PoDaoElementFinish == 1)
    {
        Camera.CalculateLine.PoDaojishu = 2;
        TIMESTRAT1 = 1;
    }
    if(Camera.CalculateLine.PoDaojishu == 2 && ClockTIME1 > 700)///////////////////////////
    {
        Camera.CalculateLine.PoDaoElementFinish = 0;
        Camera.CalculateLine.PoDaojishu = 0;
        Yuansujisu++;
        TIMESTRAT1 = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      FindTwoPoint
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void FindTwoPoint(void)
{
    for(uint8 i = Camera.GetPoint.Left.NearChangeY;i>5;i--)
    {
        if(Camera.CalculateLine.Line[i][0]<10)
        {
            Camera.GetPoint.Left.SanCaTop = i;//行
            break;
        }
        if(i == 6)
        {
            Camera.GetPoint.Left.SanCaTop = 6;
            break;
        }
    }

    for(uint8 i = Camera.GetPoint.Left.NearChangeY;i<80;i++)
    {
        if(Camera.CalculateLine.Line[i][0]<10)
        {
            Camera.GetPoint.Left.SanCaBottom = i;//行
            break;
        }
        if(i == 79)
        {
            Camera.GetPoint.Left.SanCaBottom = 79;
            break;
        }
    }

    for(uint8 i = Camera.GetPoint.Right.NearChangeY;i>5;i--)
    {
        if(Camera.CalculateLine.Line[i][2]>178)
        {
            Camera.GetPoint.Right.SanCaTop = i;//行
            break;
        }
        if(i == 6)
        {
            Camera.GetPoint.Right.SanCaTop = 6;
            break;
        }
    }

    for(uint8 i = Camera.GetPoint.Right.NearChangeY;i<80;i++)
    {
        if(Camera.CalculateLine.Line[i][2]>178)
        {
            Camera.GetPoint.Right.SanCaBottom = i;//行
            break;
        }
        if(i == 79)
        {
            Camera.GetPoint.Right.SanCaBottom = 79;
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Three-pronged
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void Threepronged(void)//入三叉
{
    if(Camera.CalculateLine.SancaElementFinish == 1 && Camera.CalculateLine.ElementType == 6 && Camera.CalculateLine.ElementFinish == 1 && Camera.CalculateLine.SanCaStep == 0 )
    {
       Camera.CalculateLine.SanCaStep = 1;
    }

    if(Camera.GetPoint.Right.LostNum > 15 && Camera.GetPoint.Left.LostNum > 15 && Camera.CalculateLine.SanCaStep == 1)
    {
       Camera.CalculateLine.SanCaStep = 2;
    }

    if(Camera.CalculateLine.SanCaStep == 2)
    {
        if(YuanSuXuWei[Yuansujisu] == 6)
        {
            Fill_line(60,80,60,20);//拐点后半部分补线
            Fill_line(1,20,1,80);//左边线拉直
        }
        else if(YuanSuXuWei[Yuansujisu] == 9)
        {
            Fill_line(128,20,128,80);//拐点后半部分补线
            Fill_line(187,80,187,20);//右线拉直
        }
     }
     if(YuanSuXuWei[Yuansujisu] == 6 && Camera.GetPoint.Right.LostNum < 10  && Camera.CalculateLine.SanCaStep == 2)
     {
        Camera.CalculateLine.SanCaStep = 0;
        Camera.CalculateLine.ElementFinish = 0;
        Camera.CalculateLine.SancaElementFinish = 0;
        Yuansujisu++;
     }
     if(YuanSuXuWei[Yuansujisu] == 9 && Camera.GetPoint.Left.LostNum < 10 && Camera.CalculateLine.SanCaStep == 2)
     {
        Camera.CalculateLine.SanCaStep = 0;
        Camera.CalculateLine.ElementFinish = 0;
        Camera.CalculateLine.SancaElementFinish = 0;
        Yuansujisu++;
     }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      FindContinueLine
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void FindContinueLine(void)
{
    Camera.GetPoint.Left.ContinueLine = 0;
    Camera.GetPoint.Right.ContinueLine = 0;
    uint8 FindStartLine = 70;
    uint8 FindEndLine = 8;
    uint8 LeftStartFlag = 0;
    uint8 RightStartFlag = 0;
    uint8 LeftTimes = 0;
    uint8 RightTimes = 0;

    for(uint8 i=FindStartLine; i>FindEndLine; i--)
    {
        if(Camera.CalculateLine.Line[i][0] > 3)
        {
            LeftStartFlag = i;
            break;
        }
    }
    for(uint8 i=FindStartLine; i>FindEndLine; i--)
    {
        if(Camera.CalculateLine.Line[i][2] < 185)
        {
            RightStartFlag = i;
            break;
        }
    }
    for(uint8 i=LeftStartFlag; i>FindEndLine; i--)
    {
        if((Camera.CalculateLine.Line[i-1][0] - Camera.CalculateLine.Line[i][0] > 4)
           || (Camera.CalculateLine.Line[i][0] - Camera.CalculateLine.Line[i+1][0] < 0
          ))
        {
            LeftTimes++;
        }
    }
    for(uint8 i=RightStartFlag; i>FindEndLine; i--)
    {
        if((Camera.CalculateLine.Line[i-1][2] - Camera.CalculateLine.Line[i][2] < -4)
           || (Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i+1][2] > 0
          ))
        {
            RightTimes++;
        }
    }
    if(LeftTimes < 2)
    {
        Camera.GetPoint.Left.ContinueLine = 1;
    }
    if(RightTimes < 2)
    {
        Camera.GetPoint.Right.ContinueLine = 1;
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      Circle
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void Circle(void)
{
    Camera.GetPoint.Right.SlopeQ = process_curvity(Camera.CalculateLine.Line[55][2],55,Camera.CalculateLine.Line[60][2],60,Camera.CalculateLine.Line[65][2],65);
    Camera.GetPoint.Left.SlopeQ = process_curvity(Camera.CalculateLine.Line[55][0],55,Camera.CalculateLine.Line[60][0],60,Camera.CalculateLine.Line[65][0],65);

    if(YuanSuXuWei[Yuansujisu] == 4 && Camera.CalculateLine.ElementFinish && Camera.GetPoint.Left.CircleStep == 0)//左圆环
    {
        Camera.GetPoint.Left.CircleStep = 1;
    }
    //第一步确定左圆环后补线走直道
    if(Camera.GetPoint.Left.CircleStep == 1 || Camera.GetPoint.Left.CircleStep == 2 || Camera.GetPoint.Left.CircleStep == 3)
    {
        if(Camera.GetPoint.Left.CircleStep == 1 && Camera.GetPoint.Left.LostNum > 23  && !Camera.GetPoint.Left.NearChangeX && Camera.GetPoint.Left.CircleStep != 2 && Camera.CalculateLine.Line[68][0] < 2 && Camera.CalculateLine.Line[69][0] < 2 && Camera.CalculateLine.Line[70][0] < 2)
        {
            Camera.GetPoint.Left.CircleStep = 2;
        }
        if(Camera.GetPoint.Left.CircleStep == 2 && Camera.GetPoint.Left.LostNum < 20)
        {
            Camera.GetPoint.Left.CircleStep = 3;
        }
        if(Camera.GetPoint.Left.CircleStep == 3 && Camera.GetPoint.Left.LostNum > 15)
        {
            FarSuddenlyChangePoint(Camera.GetPoint.Bottom-20,Camera.GetPoint.Top+5,0);
            Camera.GuidanceOpen = 1;
        }
        //找到远处拐点准备入环
        if(Camera.GetPoint.Left.CircleStep == 3 && Camera.GetPoint.Left.FarChangeY && Camera.GetPoint.Left.LostNum > 25)
        {
            Camera.GetPoint.Left.CircleStep = 4;
        }

        if(Camera.GetPoint.Left.CircleStep != 4)
        {
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW ; i++)
            {
                if(Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4] < 1)
                {
                    Camera.CalculateLine.Line[i][0] = 1;
                }
                else Camera.CalculateLine.Line[i][0] = Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4];
            }
        }
    }
    if(Camera.GetPoint.Left.CircleStep == 4)
    {
        TIMESTRAT = 1;
        FarSuddenlyChangePoint(Camera.GetPoint.Bottom-10,Camera.GetPoint.Top+5,0);
        //第二步找到左上角拐点开始补线
        if(Camera.GetPoint.Left.FarChangeY)
        {
            Fill_line(187,80,Camera.GetPoint.Left.FarChangeX,Camera.GetPoint.Left.FarChangeY);//拐点前半部分补线
            Fill_line(Camera.GetPoint.Left.FarChangeX,Camera.GetPoint.Left.FarChangeY-1,0,20);//拐点后半部分补线
            Fill_line(1,20,1,80);//左边线拉直
            Camera.GetPoint.Left.LastFarChangeX = Camera.GetPoint.Left.FarChangeX;
            Camera.GetPoint.Left.LastFarChangeY = Camera.GetPoint.Left.FarChangeY;
        }
        else
        {
            Fill_line(187,80,Camera.GetPoint.Left.LastFarChangeX,Camera.GetPoint.Left.LastFarChangeY);//拐点前半部分补线
            Fill_line(Camera.GetPoint.Left.LastFarChangeX,Camera.GetPoint.Left.LastFarChangeY-1,0,20);//拐点后半部分补线
            Fill_line(1,20,1,80);//左边线拉直
        }

        if((Camera.GetPoint.Right.LostNum < 5 && !Camera.GetPoint.Left.FarChangeY)||(ClockTIME > 1500 &&Camera.GetPoint.Right.LostNum < 5))//进入一半了，再接再厉
        {
                Camera.GetPoint.Left.CircleStep = 6;
                TIMESTRAT = 0;
                //TIMESTRAT1 = 1;
        }
    }

    if(Camera.GetPoint.Left.CircleStep == 5)
    {
        if(ClockTIME1 > 100)
        {
            Camera.GetPoint.Left.CircleStep = 6;
            TIMESTRAT1 = 0;
        }
    }
    //第三步圆环内部正常循迹
    if(Camera.GetPoint.Left.CircleStep == 6 && Camera.GetPoint.Right.LostNum > 5 && Camera.GetPoint.Right.NearChangeX)
    {
        Camera.GetPoint.Left.CircleStep = 7;
    }
    //第四步准备出环开始补线
    if(Camera.GetPoint.Left.CircleStep == 7)
    {
        TIMESTRAT1 = 1;
        FindContinueLine();
        if(Camera.GetPoint.Right.NearChangeX)
        {
            Fill_line(Camera.GetPoint.Right.NearChangeX,Camera.GetPoint.Right.NearChangeY,94,20);//拐点后半部分补线
           // Fill_line(0,40,0,80);//左边线拉直
            Camera.GetPoint.Right.LastNearChangeX = Camera.GetPoint.Right.NearChangeX;
            Camera.GetPoint.Right.LastNearChangeY = Camera.GetPoint.Right.NearChangeY;
        }
        else
        {
            Fill_line(Camera.GetPoint.Right.LastNearChangeX,Camera.GetPoint.Right.LastNearChangeY,94,20);//拐点后半部分补线
            //Fill_line(0,40,0,80);//左边线拉直
        }
        FarSuddenlyChangePoint(Camera.GetPoint.Bottom-20,Camera.GetPoint.Top+15,0);
        Camera.GetPoint.Right.Slope1 = Getslope(Camera.CalculateLine.Line[30][2],30,Camera.CalculateLine.Line[40][2],40);
        Camera.GetPoint.Right.Slope2 = Getslope(Camera.CalculateLine.Line[35][2],35,Camera.CalculateLine.Line[45][2],45);
        Camera.GetPoint.Right.Slope3 = Getslope(Camera.CalculateLine.Line[40][2],40,Camera.CalculateLine.Line[50][2],50);
        if((My_yaw.gyro_sum < -33000) || (ClockTIME1 > 500 && Camera.GetPoint.Right.LostNum < 5) || (ClockTIME1 > 500 && Camera.GetPoint.Right.LostNum < 8 && Camera.GetPoint.Left.FarChangeX && Camera.CalculateLine.image[30][104] == 0xff  && Camera.CalculateLine.image[25][104] == 0xff  && Camera.CalculateLine.image[27][104] == 0xff && Camera.GetPoint.Right.Slope1 > 0 && Camera.GetPoint.Right.Slope2 > 0 && Camera.GetPoint.Right.Slope3 > 0 &&  Camera.GetPoint.Right.Slope1 != 0 && Camera.GetPoint.Right.Slope2 != 0 && Camera.GetPoint.Right.Slope3 != 0))

        {
            Camera.GetPoint.Left.CircleStep = 8;
            TIMESTRAT1 = 0;
            Camera.GuidanceOpen = 0;
        }
    }
    //出环小补一个
    if(Camera.GetPoint.Left.CircleStep == 8 || Camera.GetPoint.Left.CircleStep == 9)
    {
        for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW; i++)
        {
            if(Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][4] < 1)
            {
                Camera.CalculateLine.Line[i][0] = 1;
            }
            else Camera.CalculateLine.Line[i][0] = Camera.CalculateLine.Line[i][2] - (uint8)Camera.CalculateLine.Line[i][4];
        }
    }

    if(Camera.GetPoint.Left.CircleStep == 8  && Camera.GetPoint.Left.LostNum > 20)
    {
        Camera.GetPoint.Left.CircleStep = 9;
    }

    if(Camera.GetPoint.Left.CircleStep == 9 && Camera.GetPoint.Left.LostNum < 20)
    {
        TIMESTRAT = 1;
        Camera.GetPoint.Left.CircleStep = 10;
    }

    if(Camera.GetPoint.Left.CircleStep == 10 && ClockTIME > 200)
    {
        Camera.CalculateLine.ElementFinish = 0;
        Camera.GetPoint.Left.CircleStep = 0;
        Camera.CalculateLine.CircleElementFinish = 0;
        TIMESTRAT = 0;
        Yuansujisu++;
    }

//-------------------------------------------------------------------------------------------------------------------//

    if(YuanSuXuWei[Yuansujisu] == 5 && Camera.CalculateLine.ElementFinish && Camera.GetPoint.Right.CircleStep == 0)//右圆环
    {
        Camera.GetPoint.Right.CircleStep = 1;
    }

    if(Camera.GetPoint.Right.CircleStep == 1 || Camera.GetPoint.Right.CircleStep == 2 || Camera.GetPoint.Right.CircleStep == 3)
    {
        if(Camera.GetPoint.Right.CircleStep == 1 && Camera.GetPoint.Right.LostNum > 23 && !Camera.GetPoint.Right.NearChangeX && Camera.GetPoint.Right.CircleStep != 2 && Camera.CalculateLine.Line[68][2] > 186 && Camera.CalculateLine.Line[69][2] > 186 && Camera.CalculateLine.Line[70][2] > 186)
        {
            Camera.GetPoint.Right.CircleStep = 2;
        }
        if(Camera.GetPoint.Right.CircleStep == 2 && Camera.GetPoint.Right.LostNum < 20)
        {
            Camera.GetPoint.Right.CircleStep = 3;
        }
        if(Camera.GetPoint.Right.CircleStep == 3 && Camera.GetPoint.Right.LostNum > 15)
        {
            FarSuddenlyChangePoint(Camera.GetPoint.Bottom-20,Camera.GetPoint.Top+5,1);
            Camera.GuidanceOpen = 1;
        }

        if(Camera.GetPoint.Right.CircleStep == 3 && Camera.GetPoint.Right.FarChangeY && Camera.GetPoint.Right.LostNum > 25)
        {
            Camera.GetPoint.Right.CircleStep = 4;
        }

        if(Camera.GetPoint.Right.CircleStep != 4)
        {
            for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW ; i++)
            {
                if(Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4] > 187)
                {
                    Camera.CalculateLine.Line[i][2] = 187;
                }
                else Camera.CalculateLine.Line[i][2] = Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4];
            }
        }
    }

    if(Camera.GetPoint.Right.CircleStep == 4)
    {
        TIMESTRAT = 1;
        FarSuddenlyChangePoint(Camera.GetPoint.Bottom-10,Camera.GetPoint.Top+5,1);
        if(Camera.GetPoint.Right.FarChangeY)
        {
            Fill_line(Camera.GetPoint.Right.FarChangeX,Camera.GetPoint.Right.FarChangeY,1,80);//拐点前半部分补线
            Fill_line(187,20,Camera.GetPoint.Right.FarChangeX,Camera.GetPoint.Right.FarChangeY-1);//拐点后半部分补线
            Fill_line(187,80,187,20);//右线拉直
            Camera.GetPoint.Right.LastFarChangeX = Camera.GetPoint.Right.FarChangeX;
            Camera.GetPoint.Right.LastFarChangeY = Camera.GetPoint.Right.FarChangeY;
        }
        else
        {
            Fill_line(Camera.GetPoint.Right.LastFarChangeX,Camera.GetPoint.Right.LastFarChangeY,1,80);//拐点前半部分补线
            Fill_line(187,20,Camera.GetPoint.Right.LastFarChangeX,Camera.GetPoint.Right.LastFarChangeY-1);//拐点后半部分补线
            Fill_line(187,80,187,20);//右线拉直
        }

        if((Camera.GetPoint.Left.LostNum < 5 && !Camera.GetPoint.Right.FarChangeX)||(ClockTIME>1500 && Camera.GetPoint.Left.LostNum < 5))
        {
                Camera.GetPoint.Right.CircleStep = 6;
                TIMESTRAT = 0;
                //TIMESTRAT1 = 1;
        }
    }

    if(Camera.GetPoint.Right.CircleStep == 5)
    {
        if(ClockTIME1 > 100)
        {
            Camera.GetPoint.Right.CircleStep = 6;
            TIMESTRAT1 = 0;
        }
    }

    //第三步圆环内部正常循迹
    if(Camera.GetPoint.Right.CircleStep == 6 && Camera.GetPoint.Left.LostNum > 5 && Camera.GetPoint.Left.NearChangeX)
    {
        Camera.GetPoint.Right.CircleStep = 7;
    }
    //第四步准备出环开始补线
    if(Camera.GetPoint.Right.CircleStep == 7)
    {
        FindContinueLine();
        TIMESTRAT1 = 1;
        if(Camera.GetPoint.Left.NearChangeX)
        {
            Fill_line(94,20,Camera.GetPoint.Left.NearChangeX,Camera.GetPoint.Left.NearChangeY);//拐点后半部分补线
            //Fill_line(187,80,187,40);//右线拉直
            Camera.GetPoint.Left.LastNearChangeX = Camera.GetPoint.Left.NearChangeX;
            Camera.GetPoint.Left.LastNearChangeY = Camera.GetPoint.Left.NearChangeY;
        }
        else
        {
            Fill_line(94,20,Camera.GetPoint.Left.LastNearChangeX,Camera.GetPoint.Left.LastNearChangeY);//拐点后半部分补线
            //Fill_line(187,80,187,40);//右线拉直
        }
        FarSuddenlyChangePoint(Camera.GetPoint.Bottom-20,Camera.GetPoint.Top+15,1);
        Camera.GetPoint.Left.Slope1 = Getslope(Camera.CalculateLine.Line[30][0],30,Camera.CalculateLine.Line[40][0],40);
        Camera.GetPoint.Left.Slope2 = Getslope(Camera.CalculateLine.Line[35][0],35,Camera.CalculateLine.Line[45][0],45);
        Camera.GetPoint.Left.Slope3 = Getslope(Camera.CalculateLine.Line[40][0],40,Camera.CalculateLine.Line[50][0],50);

        if((My_yaw.gyro_sum > 33000) || (ClockTIME1 > 500 && Camera.GetPoint.Left.LostNum < 5)||(ClockTIME1 > 500 &&Camera.GetPoint.Left.LostNum < 8 && Camera.GetPoint.Right.FarChangeX && Camera.CalculateLine.image[30][84] == 0xff  && Camera.CalculateLine.image[25][84] == 0xff  && Camera.CalculateLine.image[27][84] == 0xff && Camera.GetPoint.Left.Slope1 < 0 && Camera.GetPoint.Left.Slope2 < 0 && Camera.GetPoint.Left.Slope3 < 0 && Camera.GetPoint.Left.Slope1 != 0 && Camera.GetPoint.Left.Slope2 != 0 && Camera.GetPoint.Left.Slope3 != 0))
        {
            Camera.GetPoint.Right.CircleStep = 8;
            TIMESTRAT1 = 0;
            Camera.GuidanceOpen = 0;
        }
    }

    if(Camera.GetPoint.Right.CircleStep == 8 || Camera.GetPoint.Right.CircleStep == 9)
    {
        for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing ; i < Camera.CalculateLine.ReadyROW ; i++)
        {
            if(Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][4] > 187)
            {
                Camera.CalculateLine.Line[i][2] = 187;
            }
            else Camera.CalculateLine.Line[i][2] = Camera.CalculateLine.Line[i][0] + (uint8)Camera.CalculateLine.Line[i][4];
        }
    }

    if(Camera.GetPoint.Right.CircleStep == 8 && Camera.GetPoint.Right.LostNum > 20)
    {
        Camera.GetPoint.Right.CircleStep = 9;
    }

    if(Camera.GetPoint.Right.CircleStep == 9 && Camera.GetPoint.Right.LostNum < 20)
    {
        Camera.GetPoint.Right.CircleStep = 10;
        TIMESTRAT = 1;
    }

    if(Camera.GetPoint.Right.CircleStep == 10 && ClockTIME > 200)
    {
        Camera.CalculateLine.ElementFinish = 0;
        Camera.GetPoint.Right.CircleStep = 0;
        Camera.CalculateLine.CircleElementFinish = 0;
        TIMESTRAT = 0;
        Yuansujisu++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      JudgmentWidth
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void JudgmentWidth(void)
{
    Camera.GetPoint.BecomeWidthNum = 0;
    for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing; i < Camera.CalculateLine.ReadyROW;i++)
    {
        Camera.CalculateLine.Line[i][5] = My_abs((int32)(Camera.CalculateLine.Line[i][3] - Camera.CalculateLine.Line[i][4]));
        if(Camera.CalculateLine.Line[i][5] > 10)
        {
            Camera.GetPoint.BecomeWidthNum++;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Near Suddenly change point
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void NearSuddenlyChangePoint(uint8 bottom,uint8 top)
{
    for(uint8 i=bottom;i>top;i--)//近处突变点
    {
      if( (Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i+2][0]>=0) && ( (Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i-5][0])>=8 || (Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i-3][0])>=5 || (Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i-2][0])>=3) )
      {
          Camera.GetPoint.Left.NearChangeX = Camera.CalculateLine.Line[i][0];
          Camera.GetPoint.Left.NearChangeY = i;
          break;
      }
      else
      {
          Camera.GetPoint.Left.NearChangeX = 0;
          Camera.GetPoint.Left.NearChangeY = 0;
      }
     }



    for(uint8 i=bottom;i>top;i--)//近处突变点
    {
        if( (Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i+2][2]<=0) && ( (Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i-5][2])<=-8 || (Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i-3][2])<=-5 || (Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i-2][2])<=-3) )
        {
            Camera.GetPoint.Right.NearChangeX = Camera.CalculateLine.Line[i][2];
            Camera.GetPoint.Right.NearChangeY = i;
            break;
        }
        else
        {
            Camera.GetPoint.Right.NearChangeX = 0;
            Camera.GetPoint.Right.NearChangeY = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Far Suddenly change point
//  @param      左边远处突变点 Location == 0 右边远处突变点Location == 1
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void FarSuddenlyChangePoint(uint8 bottom,uint8 top,uint8 Location)
{
    if(Location == 0)//左
    {
        Sideline_Leftscan(top,bottom,94);
        for(uint8 i=top;i<bottom;i++)//远处突变点
        {
              if((Camera.CalculateLine.Line[i][8]-Camera.CalculateLine.Line[i+5][8]>15)&&(Camera.CalculateLine.Line[i][8]-Camera.CalculateLine.Line[i-1][8])<=0)
              {
                  Camera.GetPoint.Left.FarChangeX = Camera.CalculateLine.Line[i][8];
                  Camera.GetPoint.Left.FarChangeY = i;
                  break;
              }
              else
              {
                  Camera.GetPoint.Left.FarChangeX = 0;
                  Camera.GetPoint.Left.FarChangeY = 0;
              }
              if(Camera.GetPoint.Left.FarChangeX >= 94)
              {
                  Camera.GetPoint.Left.FarChangeX = 0;
                  Camera.GetPoint.Left.FarChangeY = 0;
              }
         }
    }
    else if(Location == 1)//右
    {
        Sideline_Rightscan(top,bottom,94);
        for(uint8 i=top ;i<bottom;i++)//远处突变点
        {
            if(( Camera.CalculateLine.Line[i][9]-Camera.CalculateLine.Line[i+5][9]<-15)&&(Camera.CalculateLine.Line[i][9]-Camera.CalculateLine.Line[i-1][9])>=0)
            {
                Camera.GetPoint.Right.FarChangeX = Camera.CalculateLine.Line[i][9];
                Camera.GetPoint.Right.FarChangeY = i;
                break;
            }
            else
            {
                Camera.GetPoint.Right.FarChangeX = 0;
                Camera.GetPoint.Right.FarChangeY = 0;
            }
        }
        if(Camera.GetPoint.Right.FarChangeX <= 94)
        {
            Camera.GetPoint.Right.FarChangeX = 0;
            Camera.GetPoint.Right.FarChangeY = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      Shizi Far Suddenly change point
//  @param      左边远处突变点 Location == 0 右边远处突变点Location == 1
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void ShiziFarSuddenlyChangePoint(uint8 bottom,uint8 top,uint8 Location)
{
    if(Location == 0)//左
    {
        for(uint8 i=top;i<bottom;i++)//远处突变点
        {
              if((Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i+5][0]>20)&&(Camera.CalculateLine.Line[i][0]-Camera.CalculateLine.Line[i-3][0])<=0)
              {
                  Camera.GetPoint.Left.ShiziFarChangeX = Camera.CalculateLine.Line[i][0];
                  Camera.GetPoint.Left.ShiziFarChangeY = i;
                  break;
              }
              else
              {
                  Camera.GetPoint.Left.ShiziFarChangeX = 0;
                  Camera.GetPoint.Left.ShiziFarChangeY = 0;
              }
              if(Camera.GetPoint.Left.ShiziFarChangeX >= 94)
              {
                  Camera.GetPoint.Left.ShiziFarChangeX = 0;
                  Camera.GetPoint.Left.ShiziFarChangeY = 0;
              }
         }
    }
    else if(Location == 1)//右
    {
        for(uint8 i=top ;i<bottom;i++)//远处突变点
        {
            if(( Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i+5][2]<-20)&&(Camera.CalculateLine.Line[i][2]-Camera.CalculateLine.Line[i-3][2])>=0)
            {
                Camera.GetPoint.Right.ShiziFarChangeX = Camera.CalculateLine.Line[i][2];
                Camera.GetPoint.Right.ShiziFarChangeY = i;
                break;
            }
            else
            {
                Camera.GetPoint.Right.ShiziFarChangeX = 0;
                Camera.GetPoint.Right.ShiziFarChangeY = 0;
            }
        }
        if(Camera.GetPoint.Right.ShiziFarChangeX <= 94)
        {
            Camera.GetPoint.Right.ShiziFarChangeX = 0;
            Camera.GetPoint.Right.ShiziFarChangeY = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      FindLine
//  @param      MinddleLine[i][0]         左边线
//  @param      MinddleLine[i][2]         右边线
//  @return     void
//  Sample usage:               Sideline_scan(top,bottom);//y代表最下面行，x代表最上面行
//-------------------------------------------------------------------------------------------------------------------
void Sideline_scan(uint8 x,uint8 y,uint8 middleline)
{
  uint16 tiaobian = 2;
  uint8 MIDDLE = middleline;

  for(uint8 i=y;i>=x;i--)
  {
      if(middleline >= 186)
      {
          middleline = 186;
      }
      if(middleline <= 2)
      {
          middleline = 2;
      }
      for(uint8 j1=MIDDLE;j1>0;j1--)//左边线确定
      {
          if(Camera.CalculateLine.image[i][j1] == 0xff && tiaobian > 0)
          {
              tiaobian = 2;
              Camera.CalculateLine.Line[i][0] = 0;
          }
          else if((Camera.CalculateLine.image[i][j1] == 0x00 && tiaobian > 0 && Camera.CalculateLine.image[i][j1+1] == 0xff) || Camera.CalculateLine.image[i][j1+2] ==0xff)
          {
              tiaobian--;
              if(tiaobian == 0)
              {
                  Camera.CalculateLine.Line[i][0] = j1;
                  tiaobian = 2;
                  break;
              }
          }
      }
      for(uint8 j1=MIDDLE;j1<188;j1++)//右边线确定
      {
          if(Camera.CalculateLine.image[i][j1] == 0xff && tiaobian > 0)
          {
              tiaobian = 2;
              Camera.CalculateLine.Line[i][2] = 188;
          }
          else if((Camera.CalculateLine.image[i][j1] == 0x00 && tiaobian > 0 && Camera.CalculateLine.image[i][j1-1] == 0xff) || Camera.CalculateLine.image[i][j1-2] == 0xff)
          {
              tiaobian--;
              if(tiaobian == 0)
              {
                 Camera.CalculateLine.Line[i][2] = j1;
                 tiaobian = 2;
                 break;
              }
          }
      }
      MIDDLE = (Camera.CalculateLine.Line[i][0] + Camera.CalculateLine.Line[i][2]) / 2;//计算下次中线
  }

  Camera.GetPoint.Left.LostNum = 0;
  Camera.GetPoint.Right.LostNum = 0;
  Camera.GetPoint.Left.AllLostNum = 0;
  Camera.GetPoint.Right.AllLostNum = 0;
  Linesmooth();//越界？别寻了。
  for(uint8 i=Camera.CalculateLine.ReadyROW;i>=Camera.CalculateLine.ReadyROW-Camera.CalculateLine.ROWspacing;i--)
  {
      if(Camera.CalculateLine.Line[i][0]<2)
      {
          Camera.GetPoint.Left.LostNum += 1;
      }
      if(Camera.CalculateLine.Line[i][2]>186)
      {
          Camera.GetPoint.Right.LostNum += 1;
      }
      Camera.CalculateLine.Line[i][3] = Camera.CalculateLine.Line[i][2] - Camera.CalculateLine.Line[i][0];//计算这一行的赛道宽度
   }

  for(uint8 i=y-20;i>=x;i--)
  {
      if(Camera.CalculateLine.Line[i][0]<2)
      {
          Camera.GetPoint.Left.AllLostNum += 1;
      }
      if(Camera.CalculateLine.Line[i][2]>186)
      {
          Camera.GetPoint.Right.AllLostNum += 1;
      }
   }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      FindLeftLine
//  @param      Camera.CalculateLine.Line[i][8]
//  @return     void
//  Sample usage:               Sideline_scan(top,bottom);//y代表最下面行，x代表最上面行
//-------------------------------------------------------------------------------------------------------------------
void Sideline_Leftscan(uint8 x,uint8 y,uint8 middleline)
{
  uint16 tiaobian = 2;
  uint8 MIDDLE = middleline;
  for(uint8 i=y;i>=x;i--)
  {
      for(uint8 j1=MIDDLE;j1>0;j1--)//左边线确定
      {
          if(Camera.CalculateLine.image[i][j1] == 0xff && tiaobian > 0)
          {
              tiaobian = 2;
              Camera.CalculateLine.Line[i][8] = 0;
          }
          else if((Camera.CalculateLine.image[i][j1] == 0x00 && tiaobian > 0 && Camera.CalculateLine.image[i][j1+1] == 0xff) || Camera.CalculateLine.image[i][j1+2] ==0xff)
          {
              tiaobian--;
              if(tiaobian == 0)
              {
                  Camera.CalculateLine.Line[i][8] = j1;
                  tiaobian = 2;
                  break;
              }
          }
      }
  }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      FindRightLine
//  @param      Camera.CalculateLine.Line[i][9]
//  @param
//  @return     void
//  Sample usage:               Sideline_scan(top,bottom);//y代表最下面行，x代表最上面行
//-------------------------------------------------------------------------------------------------------------------
void Sideline_Rightscan(uint8 x,uint8 y,uint8 middleline)
{
  uint16 tiaobian = 2;
  uint8 MIDDLE = middleline;

  for(uint8 i=y;i>=x;i--)
  {
      for(uint8 j1=MIDDLE;j1<188;j1++)//右边线确定
      {
          if(Camera.CalculateLine.image[i][j1] == 0xff && tiaobian > 0)
          {
              tiaobian = 2;
              Camera.CalculateLine.Line[i][9] = 188;
          }
          else if((Camera.CalculateLine.image[i][j1] == 0x00 && tiaobian > 0 && Camera.CalculateLine.image[i][j1-1] == 0xff) || Camera.CalculateLine.image[i][j1-2] == 0xff)
          {
              tiaobian--;
              if(tiaobian == 0)
              {
                 Camera.CalculateLine.Line[i][9] = j1;
                 tiaobian = 2;
                 break;
              }
          }
      }
  }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      左右线段丢线处理
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void Linesmooth(void)
{
    uint8 LeftLost_Y = 0;
    uint8 RightLost_Y = 0;
    for(uint8 i=Camera.GetPoint.Bottom-5;i>Camera.GetPoint.Top;i--)
    {
        if(Camera.CalculateLine.Line[i][0] > 140)
        {
            LeftLost_Y = i;
            break;
        }
    }
    for(uint8 i=LeftLost_Y;i>Camera.GetPoint.Top;i--)
    {
        Camera.CalculateLine.Line[i][0] = 187;
        Camera.CalculateLine.Line[i][2] = 187;
    }

    for(uint8 i=Camera.GetPoint.Bottom-5;i>Camera.GetPoint.Top;i--)
    {
        if(Camera.CalculateLine.Line[i][2] < 48)
        {
            RightLost_Y = i;
            break;
        }
    }
    for(uint8 i=RightLost_Y;i>Camera.GetPoint.Top;i--)
    {
        Camera.CalculateLine.Line[i][0] = 1;
        Camera.CalculateLine.Line[i][2] = 1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      向上寻上边线
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void lineTop_scan(uint8 x1, uint8 x2)
{
    for(uint8 p=x1;p<=x2;p++)
    {
      for(uint8 p1=Camera.GetPoint.Bottom-20;p1>Camera.GetPoint.Top;p1--)
      {
         if(Camera.CalculateLine.image[p1+1][p] == 0xff && Camera.CalculateLine.image[p1][p] == 0xff && Camera.CalculateLine.image[p1-1][p] == 0x00 && Camera.CalculateLine.image[p1-2][p] == 0x00)
          {
             Camera.CalculateLine.SidelineTop[p] = p1;
             break;
          }
         else
         {
             Camera.CalculateLine.SidelineTop[p] = 0;
         }
      }
    }
    RightFindTopBigOne(x1,x2);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      寻找中间段的最大点
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void RightFindTopBigOne(uint8 Topx1,uint8 Topx2)
{
    Camera.GetPoint.MiddleChangeX = 0;
    Camera.GetPoint.MiddleChangeY = 0;

     for(uint8 k=Topx1;k<Topx2;k++)
    {
        if(Camera.CalculateLine.SidelineTop[k]>Camera.CalculateLine.SidelineTop[k-1])
        {
            Camera.GetPoint.MiddleChangeX = k;
            Camera.GetPoint.MiddleChangeY = Camera.CalculateLine.SidelineTop[k];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      二值化
//  @return     void
//-------------------------------------------------------------------------------------------------------------------

void binaryzation(void)
{
    //Camera.CalculateLine.image_threshold = my_adapt_threshold(mt9v03x_image[0],MT9V03X_W,MT9V03X_H) + Camera.CalculateLine.threshold - 30;
   // Camera.CalculateLine.image_threshold = OSTU_bin(MT9V03X_W,MT9V03X_H,mt9v03x_image[0]);
    Camera.CalculateLine.image_threshold = GetOSTUThreshold(mt9v03x_image[0],5,80,1,187) + Camera.CalculateLine.threshold - 30;
    for(uint8 h=0;h<MT9V03X_H;h++)
    {
      for(uint8 w=0;w<MT9V03X_W;w++)
      {
         if(mt9v03x_image[h][w] < Camera.CalculateLine.image_threshold)   Camera.CalculateLine.image[h][w]=0x00;
         else                                                             Camera.CalculateLine.image[h][w]=0xff;
      }
    }
}
