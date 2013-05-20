void io_hlt(void);					//
void io_cli(void);					//禁止中断
void io_out8(int port, int data);	//8位储存
int io_load_eflags(void);			//记录中断许可标志的值
void io_store_eflags(int eflags);	//保存中断许可标志
/*
 *以上函数在naskfunc.nas中
 */
void init_palette(void);//调色板
void set_palette(int start, int end, unsigned char *rgb);

void HariMain(void)
{
	int i; 
	char *p; 
	init_palette();//设定调色板

	for (i = 0xa0000; i <= 0xaffff; i++) {

		p = (char *)i;//进行类型转换 
		*p = i & 0x0f;

	}

	for (;;) {
		io_hlt();
	}
}

void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑 */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅亮蓝 */
		0xff, 0xff, 0xff,	/*  7:白 */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅暗蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, 15, table_rgb);//调用函数设置调色板
	return;

}
void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();	/* 记录中断许可标志的值 */
	io_cli(); 					/* 将中断许可标志置为0，表示禁止中断 */
	io_out8(0x03c8, start);		//0x03c8为设备号码
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);	//0x03c9为设备号码
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	/* 复原中断许可标志 */
	return;
}
