/*
 * timer.c
 * 定时器相关 
 */

#include "bootpack.h"

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

struct TIMERCTL timerctl;

void init_pit(void)
{
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;	//初始化计时器
	timerctl.timeout = 0; //初始化超时信号变量
	return;
}

void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2, 0x60);	//把IRQ-00信号接收完毕的消息通知给PIC
	timerctl.count++;	//每次中断计时器自加
	if (timerctl.timeout > 0)	//如果已经设置超时
	{
		timerctl.timeout--;		//超时时间自减
		if (timerctl.timeout == 0)	//时间到
		{
			fifo8_put(timerctl.fifo, timerctl.data);	//向队列发送超时信号
		}
	}
	return;
}

void settimer(unsigned int timeout, struct FIFO8 *fifo, unsigned char data)
/* 设定超时 */
{
	int eflags;
	eflags = io_load_eflags();
	io_cli();	/* 关闭所有可屏蔽中断(所有的IRQ中断都是可屏蔽的) */
	timerctl.timeout = timeout;
	timerctl.fifo = fifo;
	timerctl.data = data;
	io_store_eflags(eflags);	/* 恢复eflags寄存器的值 */
	return;
}