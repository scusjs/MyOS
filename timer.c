/*
 * timer.c
 * 定时器相关 
 */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;
#define	TIMER_FLAGS_ALLOC	1	//已配置状态
#define	TIMER_FLAGS_USING	2	//定时器运行中

void init_pit(void)
/* 初始化PIT */
{
	int i;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;	//初始化计时器
	timerctl.using = 0;
	timerctl.next = 0xffffffff;
	for (i = 0; i < MAX_TIMER; i++)
	{
		timerctl.timers0[i].flags = 0;	//未使用
	}
	return;
}

struct TIMER *timer_alloc(void)
/* 定时器配置 */
{
	int i;
	for (i = 0; i < MAX_TIMER; i++)
	{
		if (timerctl.timers0[i].flags == 0)
		{
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
/* 回收定时器 */
{
	timer->flags = 0;
	return;
}

void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned char data)
/* 初始化定时器 */
{
	timer->fifo = fifo;
	timer->data = data;
	return;
}

void timer_settime(struct TIMER *timer, unsigned int timeout)
/* 设置定时器 */
{
	int e, i ,j;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	//搜索注册位置
	for (i = 0; i < timerctl.using; i++)
	{
		if (timerctl.timers[i]->timeout >= timer->timeout)
		{
			break;
		}
	}
	//i号之后全部后移
	for (j = timerctl.using; j>i; j--)
	{
		timerctl.timers[j] = timerctl.timers[j-1];
	}
	timerctl.using++;
	//插到空位上
	timerctl.timers[i] = timer;
	timerctl.next = timerctl.timers[0]->timeout;
	io_store_eflags(e);
	return;
}

void inthandler20(int *esp)
{
	int i,j;
	io_out8(PIC0_OCW2, 0x60);	//把IRQ-00信号接收完毕的消息通知给PIC
	timerctl.count++;	//每次中断计时器自加
	if (timerctl.next > timerctl.count)
	{
		return;	//未到下一时刻，结束
	}
	for (i = 0; i < timerctl.using; i++)	//timers中的定时器都处于动作中，所以不确认flags
	{
		if (timerctl.timers[i]->timeout > timerctl.count)
		{
			break;	//一旦遇到未超时的定时器就跳出循环
		}
		//超时
		timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
		fifo32_put(timerctl.timers[i]->fifo, timerctl.timers[i]->data);
	}
	timerctl.using -=i;	//正好有i个定时器超时, 其余的进行移位
	for (j = 0; j < timerctl.using; j++)
	{
		timerctl.timers[j] = timerctl.timers[i+j];
	}
	if (timerctl.using > 0)
	{
		timerctl.next = timerctl.timers[0]->timeout;	//还有活动的定时器
	}
	else
	{
		timerctl.next = 0xffffffff;
	}
	return;
}
