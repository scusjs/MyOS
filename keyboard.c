/*
 *keyboard.c
 *键盘相关函数
 */
#include "bootpack.h"

#define PORT_KEYDAT				0x0060
#define PORT_KEYSTA				0x0064
#define PORT_KEYCMD				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60 	//模式设定
#define KBC_MODE				0x47 	//鼠标模式

#define PORT_KEYDAT		0x0060
struct FIFO32 *keyfifo;
int keydata0;

void wait_KBC_sendready(void)
{
	/* 等待键盘控制电路准备完毕 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(struct FIFO32 *fifo, int data0)
{
	/* 将FIFO缓冲区的内容写到全局变量中 */
	keyfifo = fifo;
	keydata0 = data0;
	
	/* 初始化键盘控制电路 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}


 void inthandler21(int *esp)
 /* 来自PS/2键盘的中断 */
 {
 	
 	int data;
 	io_out8(PIC0_OCW2, 0x61);	//通知PIC“IRQ—01已经受理完毕”
 	data = io_in8(PORT_KEYDAT);
 	fifo32_put(&keyfifo,data);
	return;
 }