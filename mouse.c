/*
 *mouse.c
 *鼠标相关函数
 */
#include "bootpack.h"

#define KEYCMD_SENDTO_MOUSE		0xd4 	//发送数据给鼠标
#define MOUSECMD_ENABLE			0xf4 	//鼠标答复信息

struct FIFO32 *mousefifo;
int mousedata0;


void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec)
{
	/* 将FIFO缓冲区的内容写到全局变量中 */
	mousefifo = fifo;
	mousedata0 = data0;
	/* 激活鼠标 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);

	mdec->phase = 0; /* 等待0xfa的阶段 */
	return; 
}

void inthandler2c(int *esp)
/* PS/2鼠标的中断 */
{
	int data;
	io_out8(PIC1_OCW2, 0x64);	/* 通知PIC1，IRQ-12的受理已完成 */
	io_out8(PIC0_OCW2, 0x62);	/* 通知PIC0，IRQ-02的受理已完成 */
	data = io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo, data+mousedata0);
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat)
{
	if (mdec->phase == 0) {
		/* 等待鼠标0xfa的阶段 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 等待鼠标的第1字节数据 */
		if ((dat & 0xc8)==0x08) 	//修正鼠标按键信息
		{
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 等待鼠标的第2字节数据 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 等待鼠标的第3字节数据 */
		mdec->buf[2] = dat;
		mdec->phase = 1;

		mdec->btn = mdec->buf[0] & 0x07; 	//提取按键信息
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {	//提取水平移动信息
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {	//提取竖直移动信息
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 鼠标移动的y轴方向与屏幕y轴方向相反 */


		return 1;
	}
	return -1; /* 异常返回 */
}

