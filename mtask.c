/*
 * mtask.c
 * 多任务处理相关函数
 */
 #include "bootpack.h"

 struct TIMER *mt_timer;
 int mt_tr;	//TR寄存器

void mt_init(void)
/* 初始化mt_timer和mt_tr的值，并将计时器设置为0.02秒 */
{
	mt_timer = timer_alloc();
	timer_settime(mt_timer, 2);
	mt_tr = 3 * 8;
	return;
}

void mt_taskswitch(void)
/* 根据当前tr值计算下一个tr值，并设置定时器实现程序切换 */
{
	if (mt_tr == 3 * 8)
	{
		mt_tr = 4 * 8;
	}
	else
		mt_tr = 3 * 8;
	timer_settime(mt_timer, 2);
	farjmp(0, mt_tr);
	return;
}