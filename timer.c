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
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;	//初始化计时器
	timerctl.next = 0xffffffff;
	for (i = 0; i < MAX_TIMER; i++)
	{
		timerctl.timers0[i].flags = 0;	//未使用
	}
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;	//队尾
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
	timerctl.using = 1;
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
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();
	timerctl.using++;
	t = timerctl.t0;
	if (timer->timeout <= t->timeout)
	//插在最前面的情况
	{
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	//搜索查询位置
	for (;;)
	{
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout)
		//插入到s和t之间
		{
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}

void inthandler20(int *esp)
{
	char ts = 0;
	int i;
	struct TIMER *timer;
	io_out8(PIC0_OCW2, 0x60);	//把IRQ-00信号接收完毕的消息通知给PIC
	timerctl.count++;	//每次中断计时器自加
	if (timerctl.next > timerctl.count)
	{
		return;	//未到下一时刻，结束
	}
	timer = timerctl.t0;	//将第一个地址赋值给timer
	for (i = 0; i < timerctl.using; i++)	//timers中的定时器都处于动作中，所以不确认flags
	{
		if (timer->timeout > timerctl.count)
		{
			break;	//一旦遇到未超时的定时器就跳出循环
		}
		//超时
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != mt_timer)
		{
			fifo32_put(timer->fifo, timer->data);
		}
		else
			ts = 1;
		timer = timer->next;
	}
	//移位
	timerctl.t0 = timer;

	//timerctl.next设定
	timerctl.next = timerctl.t0->timeout;	//还有活动的定时器
	if (ts != 0)
	{
		task_switch();
	}
	return;
}