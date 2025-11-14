/*
 * Camera.h
 *
 *  Created on: 2022年3月12日
 *      Author: Fily
 */

#ifndef CODE_CAMERA_H_
#define CODE_CAMERA_H_

#define ThresholdChunkwidth 1
#define ThresholdChunkheight 1

typedef struct image
{
    int Threshold[ThresholdChunkwidth][ThresholdChunkheight];
    uint8 GuidanceOpen;
    struct CalculateLine
    {
        uint8 ElementType;           //啥也不是0 直道1 弯道2 斑马线3 左圆环4 右圆环5 三叉左6 十字7 坡道8 三叉右9
        uint8 MiddleLine;            //赛道中点
        uint8 Line[120][10];//左边线0 中线1 右边线2 赛道宽3 提前测量赛道宽4 赛道宽度差值5 左边赛道宽6 右边赛道宽7 重新扫得左线8 重新扫得右线9
        int16 error;
        int16 FlyCameraerror;
        float FlyCameraFinalerror;
        float Finalerror;
        uint8 image[120][188];          //二值化后图像
        uint8 image_threshold;
        uint8 SidelineTop[188];
        uint8 ElementFinish;
        uint8 CircleElementFinish;
        uint8 SancaElementFinish;
        uint8 PoDaoElementFinish;
        uint8 ZhiluShiziElementFinish;
        uint8 LeftXieluShiziElementFinish;
        uint8 RightXieluShiziElementFinish;
        uint8 PoDaojishu;
        uint8 ReadyROW;
        uint8 EndROW;
        uint8 ROWspacing;
        uint8 threshold;
        uint8 SanCaStep;
        uint8 ShiziStep;
        uint8 WhichOut;   //1左 2右
        uint8 TOPline;
        uint8 WhichSpeed; //1保底 2冲刺
        uint8 ZhiDao;
        uint8 LongZhiDao;
        uint8 RunTimes;
        uint16 Light;
        uint16 ReturnLight;
    }CalculateLine;

    struct GetPoint
    {
        uint8 Bottom;
        uint8 Top;
        uint8 BecomeWidthNum;
        uint8 MiddleChangeX;
        uint8 MiddleChangeY;
        struct Left
        {
            uint8 LostNum;
            uint8 ShiziLineLost;
            uint8 AllLostNum;
            uint8 NearChangeX;
            uint8 FarChangeX;
            uint8 LastNearChangeX;
            uint8 LastFarChangeX;
            uint8 NearChangeY;
            uint8 FarChangeY;
            uint8 LastNearChangeY;
            uint8 LastFarChangeY;
            uint8 ShiziFarChangeY;
            uint8 ShiziFarChangeX;
            uint8 BigPointX;
            uint8 BigPointY;
            float Slope1;
            float Slope2;
            float Slope3;
            float SlopeQ;
            uint8 CircleStep;
            float Angle;
            uint8 SanCaBottom;
            uint8 SanCaTop;
            uint8 ContinueLine;
            uint8 MiddleHighLine;
        }Left;

        struct Right
        {
            uint8 LostNum;
            uint8 ShiziLineLost;
            uint8 AllLostNum;
            uint8 NearChangeX;
            uint8 FarChangeX;
            uint8 LastNearChangeX;
            uint8 LastFarChangeX;
            uint8 NearChangeY;
            uint8 FarChangeY;
            uint8 LastNearChangeY;
            uint8 LastFarChangeY;
            uint8 ShiziFarChangeY;
            uint8 ShiziFarChangeX;
            uint8 BigPointX;
            uint8 BigPointY;
            float Slope1;
            float Slope2;
            float Slope3;
            float SlopeQ;
            uint8 CircleStep;
            float Angle;
            uint8 SanCaBottom;
            uint8 SanCaTop;
            uint8 ContinueLine;
            uint8 MiddleHighLine;
        }Right;

    }GetPoint;

}image;

extern image Camera;
extern uint8 Yuansujisu;
void CalculateMiddleHighLine(void);
void FindContinueLine(void);
void ZhiluShizi(void);
void XieluShizi(void);
uint8 Zhilushizi(uint8 StartLine,uint8 EndLine);
void PoDao(void);
void Threepronged(void);
void FindTwoPoint(void);
void NearSuddenlyChangePoint(uint8 bottom,uint8 top);
void FarSuddenlyChangePoint(uint8 bottom,uint8 top,uint8 Location);
void ShiziFarSuddenlyChangePoint(uint8 bottom,uint8 top,uint8 Location);
void CameraCollectInit(void);
void binaryzation(void);
void Sideline_scan(uint8 x,uint8 y,uint8 middleline);
void Sideline_Leftscan(uint8 x,uint8 y,uint8 middleline);
void Sideline_Rightscan(uint8 x,uint8 y,uint8 middleline);
void CalclulateError(void);
void calculatemiddleline(void);
void JudgmentWidth(void); //判断赛道宽度
void JudgmentElementType(void);//判断赛道类型
void Linesmooth(void);//弯道滤波处理
void Circle(void);
void lineTop_scan(uint8 x1, uint8 x2);//找中间边线
void RightFindTopBigOne(uint8 x1,uint8 x2);//找中间突变点最大值
#endif /* CODE_CAMERA_H_ */
