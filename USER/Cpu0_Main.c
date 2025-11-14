#include "Cpu0_Main.h"
#include "headfile.h"
#include "Init.h"
#include "My_Fun.h"
#include "MyTool.h"
#include "Camera.h"
#include "My_Flash.h"
#pragma section all "cpu0_dsram"

int core0_main(void)
{
    get_clk();//获取时钟频率  务必保留
    //用户在此处调用各种初始化函数等

    Initcar();
    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();
    while (TRUE)
    {
        if(mt9v03x_finish_flag)
        {
            //Chunkthreshold(mt9v03x_image[0],Camera.CalculateLine.image[0],MT9V03X_W,MT9V03X_H,ThresholdChunkwidth,ThresholdChunkheight);
            //adaptiveThreshold(mt9v03x_image[0],Camera.CalculateLine.image[0],188,120,7,5);
            binaryzation();
            calculatemiddleline();
            mt9v03x_finish_flag = 0;
        }
        //ips114_displayimage032_zoom(Camera.se[0], MT9V03X_W, MT9V03X_H,188,120);
        //SendMESSGE();
    }
}

#pragma section all restore
