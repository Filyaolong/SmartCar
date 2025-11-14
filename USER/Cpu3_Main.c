#include "headfile.h"
#pragma section all "cpu3_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU2的RAM中

void core3_main(void)
{
    disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //用户在此处调用各种初始化函数等
    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();
    while (TRUE)
    {
        //用户在此处编写任务代码

    }
}



#pragma section all restore
