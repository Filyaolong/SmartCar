/*
 * My_Fun.h
 *
 *  Created on: 2022年3月19日
 *      Author: Fily
 */

#ifndef CODE_MY_FUN_H_
#define CODE_MY_FUN_H_

//定义按键引脚
#define KEY1    P11_3
#define KEY2    P11_10
#define KEY3    P11_2
#define KEY4    P11_9
#define KEY5    P11_6

//开关状态变量
extern uint8 key1_status;
extern uint8 key2_status;
extern uint8 key3_status;
extern uint8 key4_status;
extern uint8 key5_status;


//上一次开关状态变量
extern uint8 key1_last_status;
extern uint8 key2_last_status;
extern uint8 key3_last_status;
extern uint8 key4_last_status;
extern uint8 key5_last_status;


//开关标志位
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
extern uint8 key5_flag;
extern uint8 key,ii;

typedef struct Fun
 {
   int16 num;//目录数组
   int8 up;//上翻
   int8 down;//下翻
   int8 right;//减小
   int8 left;//加大
   int8 enter;//确定
   void (*f)(void);//功能函数指针
 }Fun;

extern Fun const table[105];
extern uint32 YuanSuXuWei[50];
extern uint8 YuanSuJishu;
void key_use(void);
void fun_use(void);
void MenuInit(void);
void SendMESSGE(void);

void fun0(void);
void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);
void fun5(void);
void fun6(void);
void fun7(void);
void fun8(void);
void fun9(void);
void fun10(void);
void fun11(void);
void fun12(void);
void fun13(void);
void fun14(void);
void fun15(void);
void fun16(void);
void fun17(void);
void fun18(void);
void fun19(void);
void fun20(void);
void fun21(void);
void fun22(void);
void fun23(void);
void fun24(void);
void fun25(void);
void fun26(void);
void fun27(void);
void fun28(void);
void fun29(void);
void fun30(void);
void fun31(void);
void fun32(void);
void fun33(void);
void fun34(void);
void fun35(void);
void fun36(void);
void fun37(void);
void fun38(void);
void fun39(void);
void fun40(void);
void fun41(void);
void fun42(void);
void fun43(void);
void fun44(void);
void fun45(void);
void fun46(void);
void fun47(void);
void fun48(void);
void fun49(void);
void fun50(void);
void fun51(void);
void fun52(void);
void fun53(void);
void fun54(void);
void fun55(void);
void fun56(void);
void fun57(void);
void fun58(void);
void fun59(void);
void fun60(void);
void fun61(void);
void fun62(void);
void fun63(void);
void fun64(void);
void fun65(void);
void fun66(void);
void fun67(void);
void fun68(void);
void fun69(void);
void fun70(void);
void fun71(void);
void fun72(void);
void fun73(void);
void fun74(void);
void fun75(void);
void fun76(void);
void fun77(void);
void fun78(void);
void fun79(void);
void fun80(void);
void fun81(void);
void fun82(void);
void fun83(void);
void fun84(void);
void fun85(void);
void fun86(void);
void fun87(void);
void fun88(void);
void fun89(void);
void fun90(void);
void fun91(void);
void fun92(void);
void fun93(void);
void fun94(void);
void fun95(void);
void fun96(void);
void fun97(void);
void fun98(void);
void fun99(void);
void fun100(void);
void fun101(void);
/*void fun102(void);
void fun103(void);
void fun104(void);*/
#endif /* CODE_MY_FUN_H_ */

