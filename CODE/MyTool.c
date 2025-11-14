/*
 * MyTool.c
 *
 *  Created on: 2022年8月6日
 *      Author: Fily
 */

#include "headfile.h"
#include "MyTool.h"
#include "Camera.h"
#include "Angle.h"
#include "My_Fun.h"
#include "ServoMotor.h"
#include "Motor.h"
#include "FlyMotor.h"

//-------------------------------------------------------------------------------------------------------------------
//  @brief      惯性导航(丐版)
//  @param      void
//  @return     void
//  @since      圆环强制出弯
//-------------------------------------------------------------------------------------------------------------------
void Guidance(uint8 WhetherOpen)//0关 1开
{
    if(!WhetherOpen)
    {
        My_yaw.gyro_sum = 0;
    }
    else if(WhetherOpen)
    {
        My_yaw.gyro_sum += My_yaw.gyro;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      检测斑马线
//  @param      uint8
//  @return     uint8
//  @since
//-------------------------------------------------------------------------------------------------------------------
uint8 ZebraCrossing(void)
{
    uint8 ZebraFlag = 0;
    uint8 start_point = 37;
    uint8 end_point = 45;
    uint8 times = 0;
    for(uint8 y = start_point; y <= end_point; y++)
    {
        uint8 cursor = 0;
        uint8 black_blocks = 0;
        for (uint8 x = 0; x <= 185; x++)
        {
            if (Camera.CalculateLine.image[y][x] == 0x00)
            {
                if (cursor >= 51)
                {
                    break;
                }
                else
                {
                    cursor++;
                }
            }
            else
            {
                if (cursor >= 5 && cursor <= 50)
                {
                    black_blocks++;
                    cursor = 0;
                }
                else
                {
                    cursor = 0;
                }
            }
        }
        if (black_blocks >= 5 && black_blocks <= 9)
        {
            times++;
        }
        if (times >= 1)
        {
            ZebraFlag = 1;
        }
        else
        {
            ZebraFlag = 0;
        }
    }
    return ZebraFlag;
}




//-------------------------------------------------------------------------------------------------------------------
//  @brief      保存赛道宽度
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void KeepLineWidth(void)
{
    for(uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing; i < Camera.CalculateLine.ReadyROW;i++)
    {
        Camera.CalculateLine.Line[i][4] = Camera.CalculateLine.Line[i][3];
    }
}

void LookLookLineWidth(void)
{
    uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;
    for(uint8 x = 0;x < 200;x = x + 30)
    {
        for(uint8 y = 0;y < 8;y = y + 1)
        {
            ips114_showuint8(x,y,Camera.CalculateLine.Line[i][4]);
            i++;
            if(i > Camera.CalculateLine.ReadyROW) break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      屏幕显示赛道宽度
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void ShowlineWidth(void)
{
    uint8 i = Camera.CalculateLine.ReadyROW - Camera.CalculateLine.ROWspacing;
    for(uint8 x = 0;x < 200;x = x + 30)
    {
        for(uint8 y = 0;y < 8;y = y + 1)
        {
            ips114_showuint8(x,y,Camera.CalculateLine.Line[i][3]);
            i++;
            if(i > Camera.CalculateLine.ReadyROW) break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      屏幕显示突变点
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void showSuddenlyChangePoint(uint8 X,uint8 Y,uint16 Color)
{
    for(uint8 i=X-2;i<X+2;i++)
    {
        ips114_drawpoint(i,Y,Color);
    }

    for(uint8 i=Y-2;i<Y+2;i++)
    {
        ips114_drawpoint(X,i,Color);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      屏幕显示线
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void showline(void)
{
  for(uint8 i=Camera.GetPoint.Bottom;i>Camera.GetPoint.Top;i--)
  {
     ips114_drawpoint(Camera.CalculateLine.Line[i][0]+5,i,RED);
     ips114_drawpoint(Camera.CalculateLine.Line[i][1],i,BLUE);
     ips114_drawpoint(Camera.CalculateLine.Line[i][2]-5,i,RED);
  }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      屏幕显示计算误差起始行与终止行
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void ShowReady_and_End(void)
{
    for(uint8 i=0;i<188;i++)
    {
        ips114_drawpoint(i,Camera.CalculateLine.ReadyROW,BROWN);
        ips114_drawpoint(i,Camera.CalculateLine.EndROW,PINK);
        ips114_drawpoint(i,Camera.CalculateLine.ReadyROW+1,BROWN);
        ips114_drawpoint(i,Camera.CalculateLine.EndROW+1,PINK);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      补线
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
void Fill_line(uint8 LeftX,uint8 LeftY,uint8 RightX,uint8 RightY)
{
  float Dx = 0;//增长数
  uint8 count = 0;
  if(LeftY>RightY)//左远补右线
  {
    Camera.CalculateLine.Line[RightY][2] = RightX;
    Dx = (RightX-LeftX)*1.0/(LeftY-RightY);
    for(uint8 i=RightY;i<=LeftY;i++)
    {
      uint8 j = (int)(Camera.CalculateLine.Line[RightY][2] - count*Dx);
      Camera.CalculateLine.Line[i][2] = j;
      count++;
    }
  }
  else if(LeftY<RightY)//右远补左线
  {
    Camera.CalculateLine.Line[LeftY][0] = LeftX;
    Dx = (RightX-LeftX)*1.0/(RightY-LeftY);
    for(uint8 i=LeftY;i<=RightY;i++)
    {
      uint8 j = (int)(Camera.CalculateLine.Line[LeftY][0] + count*Dx);
      Camera.CalculateLine.Line[i][0] = j;
      count++;
    }
  }
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      计算斜率
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
float Getslope(uint8 x1,uint8 y1,uint8 x2,uint8 y2)
{
  float BackValue;

  if(y1==y2)   return 0;
  else         BackValue  =  (1.0*y2-y1)/(float)(x2-x1);

  if(BackValue > 10) BackValue = 0;
  if(BackValue < -10) BackValue = 0;

  return BackValue;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      三点计算曲率
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
float process_curvity(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 x3, uint8 y3)
{
    float K;
    float q1,AB,BC,AC;
    float S_of_ABC;
    //面积的符号表示方向
    S_of_ABC = ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) / 2;
    q1 = (float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    AB = sqrt(q1);
    q1 = (float)((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
    BC = sqrt(q1);
    q1 = (float)((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1));
    AC = sqrt(q1);
    if (AB * BC * AC == 0)
    {
        K = 0;
    }
    else
        K = (float)4 * S_of_ABC / (AB * BC * AC);
    return K;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      三点角度
//  @param      float
//  @return     float
//  @since
//-------------------------------------------------------------------------------------------------------------------
float Get_angle(uint8 ax,uint8 ay,uint8 bx,uint8 by,uint8 cx,uint8 cy)
{
    float abx=ax-bx;
    float aby=ay-by;
    float cbx=cx-bx;
    float cby=cy-by;
    float ab_muti_cb=abx*cbx+aby*cby;
    float dist_ab=sqrt(abx*abx+aby*aby);
    float dist_cb=sqrt(cbx*cbx+cby*cby);
    float cosvalue=ab_muti_cb/(dist_ab*dist_cb);
    if(cx<5 || cx>183 || ax<5 || ax>183)
    {
        return 0;
    }
    else return (float)(acos(cosvalue)*180/3.14);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      局部阈值法
//  @param      uint8
//  @return     uint8
//  @since
//-------------------------------------------------------------------------------------------------------------------
void adaptiveThreshold(uint8* img_data, uint8* output_data, int width, int height, int block, uint8 clip_value)
{
  int half_block = block / 2; //half = 3
  for(int y=half_block; y<height-half_block; y++)
  {
    for(int x=half_block; x<width-half_block; x++)
    {
      // 计算局部阈值
      int thres = 0;
      for(int dy=-half_block; dy<=half_block; dy++)
      {
        for(int dx=-half_block; dx<=half_block; dx++)
        {
          thres += img_data[(x+dx)+(y+dy)*width];
        }
      }
      thres = thres / (block * block) - clip_value;
      // 进行二值化
      output_data[x+y*width] = img_data[x+y*width]>thres ? 255 : 0;
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      分块大津法计算阈值(有BUG)
//  @param      平方函数：pow(A,B) = A ^ B;Chunkwidth = 2*2*7*7;Chunkheight = 2*2*2*3*5;width = 188;height = 120
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void Chunkthreshold(uint8 *image,uint8 *OutputImage,uint16 width,uint16 height,uint8 Chunkwidth,uint8 Chunkheight)
{
    #define GrayScale 256
    uint8 *Imagedata = image;
    int Everywidth = width/Chunkwidth;
    int Everyheight = height/Chunkheight;
    int32 pixelSum = Everywidth * Everyheight;

    for(uint8 dy=0;dy<Chunkheight;dy++)
    {
        for(uint8 dx=0;dx<Chunkwidth;dx++)
        {
            Camera.Threshold[dy][dx] = 0;
        }
    }

    for(uint8 dy=0;dy<Chunkheight;dy++)
    {
        for(uint8 dx=0;dx<Chunkwidth;dx++)
        {

            uint32 gray_sum=0;
            int pixelCount[GrayScale];//灰度值数量统计
            float pixelPro[GrayScale];//比例值
            int dn = dy*Chunkwidth+dx;//第n个图像块
            //遍历灰度级[0,255]
            float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
            w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;

            for (int i = 0; i < GrayScale; i++)
            {
                pixelCount[i] = 0;
                pixelPro[i] = 0;
            }

            for (int ddy=0;ddy<Everyheight;ddy++)
            {
                for (int ddx=0;ddx<Everywidth;ddx++)//统计灰度级中每个像素在整幅图像中的个数
                {
                    pixelCount[(int)Imagedata[Everywidth*(dn%Chunkwidth)+ddx+(Everyheight*(dn%Chunkwidth)+ddy)*188]]++;
                    gray_sum+=(int)Imagedata[Everywidth*(dn%Chunkwidth)+ddx+(Everyheight*(dn%Chunkwidth)+ddy)*188];
                }
            }

            for (int i=0;i<GrayScale;i++)//计算每个像素值的点在整幅图像中的比例
            {
                pixelPro[i] = (float)pixelCount[i] / pixelSum;
            }

            for (int j = 0; j < GrayScale; j++)
            {
                w0 += pixelPro[j];         //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

                w1=1 - w0;
                u1tmp=gray_sum / pixelSum - u0tmp;

                u0 = u0tmp / w0;              //背景平均灰度
                u1 = u1tmp / w1;              //前景平均灰度
                u = u0tmp + u1tmp;            //全局平均灰度
                deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
                if (deltaTmp > deltaMax)
                {
                    deltaMax = deltaTmp;
                    Camera.Threshold[dy][dx] = j;
                }
            }

            for (int ddy=0;ddy<Everyheight;ddy++)
            {
                for (int ddx=0;ddx<Everywidth;ddx++)//统计灰度级中每个像素在整幅图像中的个数
                {
                    OutputImage[Everywidth*(dn%Chunkwidth)+ddx+(Everyheight*(dn%Chunkwidth)+ddy)*188] = Imagedata[Everywidth*(dn%Chunkwidth)+ddx+(Everyheight*(dn%Chunkwidth)+ddy)*188]>Camera.Threshold[dy][dx]?255:0;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      大津法计算阈值(法1)
//  @param      uint8
//  @return     uint8
//  @since
//-------------------------------------------------------------------------------------------------------------------

uint8 my_adapt_threshold(uint8 *image, uint16 col, uint16 row)   //注意计算阈值的一定要是原图像
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }
    //计算每个像素值的点在整幅图像中的比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    return threshold;
}

/*--------------------------------------------------------------------------
* 【函数功能】：大津法计算阈值(法2)
* 【参    数】：无
* 【返 回 值】：无
*--------------------------------------------------------------------------*/
uint8 OSTU_bin(uint8 width,uint8 height,uint8 *Image)
{
        #define GrayScale 256   //frame
        typedef unsigned char uchar;
        int pixel[256]={0};
        int threshold1=0;
        int32 sum_gray=0;
        int32 sum_pix_num=0;
        int32 pl_pix_num=0;
        int32 p2_pix_mum=0;
        int32 p1_sum_gray=0;
        float m1=0;
        float m2=0;
        float V=0;
        float variance=0;
        int i,j,k=0;

        for(i = 0;i<256;i++)
            pixel[i] = 0;


        for(i = 0; i < height; i++)
        {
            for(j = 0;j < width;j++)
            {
                pixel[(int)Image[i * width + j]]++;
            }
        }

            for(k=0;k<GrayScale;k++)
            {
                sum_gray+=k*pixel[k];
                sum_pix_num+=pixel[k];
            }

            for(k=0;k<GrayScale-1;k++)
            {
                pl_pix_num+=pixel[k];
                p2_pix_mum=sum_pix_num-pl_pix_num;
                p1_sum_gray+=k*pixel[k];
                m1=(float)p1_sum_gray/pl_pix_num;
                m2=(float)(sum_gray-p1_sum_gray)/p2_pix_mum;

                V=pl_pix_num*p2_pix_mum*(m1-m2)*(m1-m2);

                if(V>variance)
                {
                    variance=V;
                    threshold1=k;
                }
            }
            return threshold1;
}

/*--------------------------------------------------------------------------
* 【函数功能】：大津法计算阈值(法3)
* )
* 【参    数】：无
* 【返 回 值】：无
*--------------------------------------------------------------------------*/
uint8 GetOSTUThreshold(uint8 (*img)[MT9V03X_W], uint16 start_row,uint16 end_row, uint16 start_col, uint16 end_col)
{
  int threshold1 =0;
  int32 sum_gray   =0;
  int32 sum_pix_num=0;
  int32 pl_pix_num =0;
  int32 p2_pix_mum =0;
  int32 p1_sum_gray=0;
  float m1=0;
  float m2=0;
  float V=0;
  float variance=0;
  int i,j,k=0;
  uint16 MinValue=0,MaxValue=255;
  uint16 DeleGrayClass1 = 30; //高灰度级
  uint16  HistoGram[256]={0};

  for(i = 0; i < 256; i++)
    HistoGram[i] = 0;

  for(i = start_row; i < end_row; i++)
  {
      for(j = start_col; j < end_col; j++)
      {
          HistoGram[(int)img[i][j]]++;
      }
  }

  //优化--删除灰度级顶部<x个点的灰度级  删除灰度级底部<x个点的灰度级 x==> 10-25之间
  //for(k=255;k>0;--k) {if(HistoGram[k]<=DeleGrayClass1)  HistoGram[k] = 0; else break;}
  //for(k=0;k<256;++k) {if(HistoGram[k]<=DeleGrayClass2)  HistoGram[k] = 0; else break;}

  for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; ++MinValue) ;        //获取最小灰度级
  for (MaxValue = 255; MaxValue > MinValue && HistoGram[MaxValue] == 0; --MaxValue) ; //获取最大灰度级

  for(k=MinValue;k<=MaxValue;k++)
  {
      sum_gray+=k*HistoGram[k];
      sum_pix_num+=HistoGram[k];
  }

  for(k=MinValue;k<=MaxValue;k++)
  {
      pl_pix_num+=HistoGram[k];
      p2_pix_mum=sum_pix_num-pl_pix_num;
      p1_sum_gray+=k*HistoGram[k];
      m1=(float)p1_sum_gray/pl_pix_num;
      m2=(float)(sum_gray-p1_sum_gray)/p2_pix_mum;

      V=pl_pix_num*p2_pix_mum*(m1-m2)*(m1-m2);

      if(V>variance)
      {
          variance=V;
          threshold1=k;
      }
  }
  return threshold1;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      清华上位机矫正陀螺仪
//  @param      void,float
//  @return     void,float
//  @since acc_pitch
//-------------------------------------------------------------------------------------------------------------------
void My_sendBufftoPC()
{
    data_conversion(FlyMotor.ExpectAngle,FlyMotor.RealAngle,0,0,virtual_scope_data);
    uart_putbuff(WIRELESS_UART, virtual_scope_data, sizeof(virtual_scope_data));  //数据转换完成后，使用串口发送将数组的内容发送出去
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      绝对值计算
//  @param      void,float
//  @return     void,float
//  @since
//-------------------------------------------------------------------------------------------------------------------
int32 My_abs(int32 i)
{
    if(i<0)
    {
        i = -i;
    }
    return i;
}

float My_floatabs(float i)
{
    if(i<0)
    {
        i = -i;
    }
    return i;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      接受数据
//  @param
//  @return
//  @since
//-------------------------------------------------------------------------------------------------------------------
void getData(void)
{
    if(uart_query(UART_2,&wireless_rx_buffer))
    {
        uart_getchar(UART_2,&wireless_rx_buffer);
    }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      UART FOR FLOAT
//  @param      void,float
//  @return     void,float
//  @since
//-------------------------------------------------------------------------------------------------------------------
/*void UART_Send_FloatBuff(float Buff)
{
    float a[2] = {1,3.14};

    uart_putbuff(UART_2,&a[0],sizeof(a[0]));
    uart_putbuff(UART_2,&a[1],sizeof(a[1]));
}*/


//--------------------------------------------------无线模块-----------------------------------------------//
/*
 * 无线模块 推荐使用逐飞的无线模块（usb to nrf）*购买时建议同时购买ttl，用于配置小车的无线模块
 * 波特率可以最高拉到460800 *对应小车上的程序,小车的无线模块，电脑上的无线模块，上位机波特率一致
 * 无线模块速度比wifi慢 但优点是稳定 对于灰度图传速度不太够用，但有二值化图传优化 适合边图传边示波器
 */
//--------------示波器-------------------//
//name数据标识(通道) dat:数据
//例:int a=0;put_int(0,a);
/*void put_int(uint8 name, int dat)
{
    uint8 fu = 32;
    if (dat < 0) { dat *= -1; fu = 45; }
    uint8 i = 1; uint8 j = 1;
    while (dat >= i * 10) { i *= 10; j++; }
    uint8 datc[6] = { 122, name, j, fu, 33 };
    sw_write_buffer(datc, 5);//无线转串口模块 串口发送数据
    for (; j > 0; j--)
    {
        uint8 pdat = dat / i;
        pdat %= 10;
        sw_write_byte(pdat);//用无线串口发送一个字节
        i /= 10;
    }
    sw_write_byte(10);//用无线串口发送一个字节
}*/


//--------------灰度图传-------------------//
//*image图像地址 width图像宽 height图像高
//例：seekfree_sendimg_03x2(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H);
/*void sendimg(uint8* image, uint8 width, uint8 height)
{
    uint8 mydat[6] = { 0x21, 0x7A, width, height, 0x21, 0x7A };

    seekfree_wireless_send_buff(mydat,6);
    seekfree_wireless_send_buff(image,width * height);
    //sw_write_byte(dat); //uart_putchar(UART_0, 0xA5);串口0发送0xA5
    //sw_write_buffer(dat[0],6);
    //sw_write_buffer(image, width * height);//uart_putbuff(UART_0,&a[0],5);
}*/

//--------------二值化图传-------------------//
//uint8 (*src)[COL]图像地址 width图像宽 height图像高 otu二值化阈值
//例：seekfree_sendimg_03x(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H,100);
/*void sendimg_binary(uint8* image, uint8 width, uint8 height,uint8 otu)
{
    uint8 dat[6]={0x7A,0x21,width,height,0x7A,0x21};
    seekfree_wireless_send_buff(dat,6);
    int databool=255;char lon=1;int data=255;
    uint8 line=0,col=0;
    for(line=0;line<width;line++)
        {
           for(col=0;col<height;col++)
           {
             if(*(image+line*height+col)>otu)data=255;
             else data=0;
             if(data==databool)
             {lon++;}else{uart_putchar(UART_2,lon);lon=1;}
             if(lon==190){uart_putchar(UART_2,lon-1);uart_putchar(UART_2, 0);lon=1;}
            databool=data;
           }
        }
}*/
//--------------二值化图传-------------------//
//image图像地址 width图像宽 height图像高 otu二值化阈值
//例：sendimg_binary(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H,100);
void sendimg_binary( uint8* image, uint8 width, uint8 height,uint8 otu)
{

    uint8 dat[6]={0x7A,0x21,width,height,0x7A,0x21};
    sw_write_buffer(dat,6);
    int databool=255;uint8 lon=1;int data=255;
    uint8 line=0,col=0;

    for(line=0;line<width;line++)
    {
        for(col=0;col<height;col++)
        {
            if(*(image+line*height+col)>otu)data=255;
            else data=0;
            if(data==databool)
            {lon++;}else{sw_write_byte(lon);lon=1;}
            if(lon==190){sw_write_byte(lon-1);sw_write_byte(0);lon=1;}
            databool=data;
        }
    }
}
//压缩二值图传
//uint16 dis_width, uint16 dis_height 要压缩图像大小
void sendimg_binary_zoom( uint8* image, uint8 width, uint8 height, uint8 dis_width, uint8 dis_height,uint8 otu)
{

    uint8 dat[6]={0x7A,0x21,dis_width,dis_height,0x7A,0x21};
    sw_write_buffer(dat,6);
    int databool=255;uint8 lon=1;int data=255;
    uint8 i,j;
    for(j=0;j<dis_height;j++)
    {
    for(i=0;i<dis_width;i++)
    {
    if(*(image+(j*height/dis_height)*width+i*width/dis_width)>otu)//读取像素点
    data=255;
    else data=0;
    if(data==databool)
    {lon++;}
    else{sw_write_byte(lon);lon=1;}
    if(lon==190){sw_write_byte(lon-1);sw_write_byte(0);lon=1;}
    databool=data;
    }
    }

}
//带有校验的二值图传
//chk值越大 抗干扰越强 值0-55
//请根据实际使用情况进行调整
void sendimg_binary_CHK(uint8* image, uint8 width, uint8 height,uint8 otu,uint8 chk)
{
    chk=chk>0?chk:0;
    chk=chk<56?chk:55;
    uint8 dat[7]={0x7A,0x21,width,height,0x7A,0x21,200+chk};
      sw_write_buffer(dat,7);
      int databool=255;uint8 lon=0;int data=255;
      uint8 line=0,col=0;
      int imglon=0;
      int imgdatlo=width*height/chk;
      uint8 CHK=0;
      for(line=0;line<width;line++)
          {
             for(col=0;col<height;col++)
             {imglon++;

                if(*(image+line*height+col)>otu)data=255;
                else data=0;
                if(data==databool)
                {lon++;}
                else{sw_write_byte(lon);lon=1;}

                if(imglon==imgdatlo)
                {CHK++;sw_write_byte(lon);data=255; databool=255;sw_write_byte(200+CHK);lon=0;imglon=0;}
                if(lon==190){sw_write_byte(lon);sw_write_byte(0);lon=0;}
               databool=data;
             }
          }

}
//--------------传线-------------------//
/*
 * 可以将寻线得到的左右边线，拟的中线发送到上位机查看
 * 例如：
 * 处理 图像img[H][W];得到左右边线存放在zx[H] yx[H] 拟出来的中线为w[H]
 * sendline_clear(swj_BLACK,H,W);//清屏 背景黑色
 * sendline(swj_WHITE,zx,H);//发送左边线
 * sendline(swj_WHITE,yx,H);//发送右边线
 * sendline(swj_WHITE,wx,H);//发送中线
 * 例如：
 * sendline_clear(swj_BLACK,H,W);//清屏 背景黑色
 * sendline_xy(zx,yx,H)//发送左右边线
 * sendline(swj_WHITE,wx,H);//发送中线
 *
 * 如上两个方法效果一致的，但下面可以在上位机上对应使用赛道还原功能
 * 注意：
 * ①每发送完一帧的图像边线 就要调用sendline_clear进行清屏
 * ②如果调用sendline_xy函数并使用上位机赛道还原功能时，若再调用sendline 一定放在sendline_xy后面 防止被覆盖
 * */

/*#define swj_BLACK 0
#define swj_WHITE 1
#define swj_RED 2
#define swj_GREEN 3
#define swj_BLUE 4
#define swj_PURPLE 5
#define swj_YELLOW 6
#define swj_CYAN 7
#define swj_ORANGE 8
//清空线条   color清屏后的背景颜色  uint16 width uint16 height 图像的大小
void sendline_clear( uint8 color,uint16 width, uint16 height)
{
    sw_write_byte(0x21); sw_write_byte(0x7A);
    sw_write_byte(width);sw_write_byte(height);
    sw_write_byte(color);sw_write_byte(0x21);
}*/

//图传赛道边界  uint8_t *zx:左边界   uint8_t *yx:右边界, uint32_t len发送的边线长度
//该函数与下放函数分别发送两个边线有何不同? 该函数可对应上位机还原赛道的功能*  注意先后顺序
/*void sendline_xy( uint8 *line_zx,uint8 *line_yx, uint32 len)
{
    sw_write_byte(0x21);
    sw_write_byte(9);
    sw_write_byte(len);
    sw_write_byte(255);
    sw_write_byte(255);
    sw_write_buffer(line_zx,len);
    sw_write_buffer(line_yx,len);
}*/
/*默认每行一个点*/
//绘制边线   color边线颜色  uint8_t *buff 发送的边线数组地址  len发送的边线长度
/*void sendline( uint8 color,uint8 *buff, uint32 len)
{
    sw_write_byte(0x21);
    sw_write_byte(color);
    sw_write_byte(len);
    sw_write_byte(255);
    sw_write_byte(255);
    sw_write_buffer(buff,len);
}*/
/*说明:
 * 例如有三个点 a(x1,y1)b(x2,y2)c(x3,y3)
 * 则 uint8 x[3]={x1,x2,x3};uint8 y[3]={y1,y2,y3};
 *  sendline2(swj_WHITE,x,y,3);
 *  sendline函数只能按顺序每行一点发送边界点
 *  sendline2函数针对于八邻域等 每行不固定点数的边界
  *           也适用于发送特殊点 例如拐点 灵活运用
 *
 * */
//无序绘制边线  color边线颜色 linex对应点的x坐标集合 liney对应点的y坐标集合  len发送的边线长度
/*void sendline2( uint8 color,uint8 *linex,uint8 *liney, uint32 len)
{
    sw_write_byte(0x21);
    sw_write_byte(color);
    sw_write_byte(len);
    sw_write_byte(254);
    sw_write_byte(255);
    sw_write_buffer(linex,len);
    sw_write_buffer(liney,len);

}*/
/*
 * 无线模块如何拉高波特率？ 淘宝官方店里可以下载资料 可以跟着官方手册里的教程来
 * 程序如何拉高波特率？ 串口初始化函数里
 * 速度太慢整么办？①将波特率拉到最高②推荐使用二值化图传
 * */
//--------------------------------------------------无线模块-----------------------------------------------//








//--------------------------------------------------wifi模块-----------------------------------------------//
/*
 * wifi模块 可以使用esp8266 10元左右 成本低，速度快
 * 单片机-串口-wifi-tcp(udp)-上位机 其也是将数据通过串口发送到wifi模块上
 * 其对环境要求高 容易受干扰出现丢包
 *
 */


/*void sendimg_WIFI( uint8 (*src)[COL], uint16 width, uint16 height)
{

    sw_write_byte(0x21); sw_write_byte(0x7A);
    sw_write_byte(width);sw_write_byte(height);
    sw_write_byte(0x21);sw_write_byte(0x7A);

    uint8 line=0,col=0;
    for(line=0;line<ROW;line++)
        {
        sw_write_byte(21);
        sw_write_byte(line);
        sw_write_byte(133);
           for(col=0;col<COL;col++)
           {
               sw_write_byte(src[line][col]);

           }

        }
}*/

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LQR飞轮控制
//  @param      void
//  @return     void
//  @since
//-------------------------------------------------------------------------------------------------------------------
/*float controllerLQR(float p_angle, float p_vel, float m_vel)
{
  if (abs(p_angle) > 5) //摆角大于5则进入非稳态，记录非稳态时间
  {
    last_unstable_time = millis();
    if (stable) //如果是稳态进入非稳态则调整为目标角度
    {
      //target_angle = EEPROM.readFloat(0) - p_angle;
      target_angle = EEPROM.readFloat(0);
      stable = 0;
    }
  }
  if ((millis() - last_unstable_time) > 1000 && !stable)  //非稳态进入稳态超过500ms检测，更新目标角为目标角+摆角，假设进入稳态
  {
    //target_angle  -= _sign(target_velocity) * 0.4;
    target_angle = target_angle+p_angle;
    stable = 1;
  }

  if ((millis() - last_stable_time) > 2500 && stable) { //稳态超过2000ms检测，更新目标角
    if (abs(target_velocity) > 5 ) { //稳态速度偏大校正
      last_stable_time = millis();
      target_angle  -= _sign(target_velocity) * 0.2;
    }
  }

  //Serial.println(stable);
  float u;

  if (!stable)  //非稳态计算
  {
    motor.PID_velocity.P = v_p_1;
    motor.PID_velocity.I = v_i_1;
    u = LQR_K3_1 * p_angle + LQR_K3_2 * p_vel + LQR_K3_3 * m_vel;
  }
  else
  {
    motor.PID_velocity.P = v_p_2;
    motor.PID_velocity.I = v_i_2;
    u = LQR_K4_1 * p_angle + LQR_K4_2 * p_vel + LQR_K4_3 * m_vel;
  }

  return u;
}*/

