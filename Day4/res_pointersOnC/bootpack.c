void io_hlt(void);

void HariMain(void)
{
	int i; 
	char *p; 

	for (i = 0xa0000; i <= 0xaffff; i++) {

		p = (char *)i;//进行类型转换 
		*p = i & 0x0f;

	}

	for (;;) {
		io_hlt();
	}
}
