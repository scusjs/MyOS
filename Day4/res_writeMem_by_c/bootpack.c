void io_hlt(void);
void write_mem8(int addr , int data );
void HariMain(void)
{
	int i;//i为32位整数
	for (i = 0xa0000; i <= 0xaffff; i++)
	{
		//write_mem8(i,15);//在0xa0000至0xaffff（即VRAM）中填充满数据15(白色)
		//write_mem8(i,5);//绿色
		write_mem8(i,i&0x0f);//条纹
	}


	for (;;)
	{
		io_hlt();
	}

}
