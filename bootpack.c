/*
 *bootpack.c
 *其他处理
 */
#include "bootpack.h"
#include <stdio.h>

extern struct FIFO8 keyfifo;
 
void HariMain(void)
{
	
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256], keybuf[32];
	int mx, my, i;
	
	
	init_gdtidt();
	init_pic();
	io_sti(); 
	fifo8_init(&keyfifo, 32 , keybuf);
	io_out8(PIC0_IMR, 0xf9); /* PIC1以外全部禁止(11111001) */
	io_out8(PIC1_IMR, 0xef); 

	init_palette();
	init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
	mx = (binfo->scrnx - 16) / 2; /* 放置鼠标指针的初始位置 */
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	io_out8(PIC0_IMR, 0xf9); 
	io_out8(PIC1_IMR, 0xef); 

	for (;;) {
		io_cli();	//屏蔽中断
		if (fifo8_status(&keyfifo)==0) {
			io_stihlt();	//取消屏蔽中断并执行HLT
		} else {
			//输出按键信息
			i = fifo8_get(&keyfifo);
			io_sti();
			sprintf(s, "%02X", i);
			boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
		}
	}
}
