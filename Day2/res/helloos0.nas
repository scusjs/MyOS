; hello-os
; TAB=4

		ORG		0x7c00			; 指明装载地址

; 以下标注FAT12格式软盘

		JMP		entry
		DB		0x90
		DB		"HELLOIPL"		
		DW		512				
		DB		1				
		DW		1				
		DB		2				
		DW		224			
		DW		2880		
		DB		0xf0			
		DW		9				
		DW		18				
		DW		2				
		DD		0				
		DD		2880			
		DB		0,0,0x29		
		DD		0xffffffff		
		DB		"HELLO-OS   "	
		DB		"FAT12   "		
		RESB	18				

; 程序核心

entry:							; 标签
		MOV		AX,0			; 初始化寄存器 注，AX累加寄存器	BX基址寄存器	CX计数寄存器	DX数据寄存器
		MOV		SS,AX 			; SS栈段寄存器
		MOV		SP,0x7c00		; SP栈指针寄存器
		MOV		DS,AX 			; DS数据段寄存器
		MOV		ES,AX 			; ES附加段寄存器

		MOV		SI,msg			; 源变址寄存器
putloop:
		MOV		AL,[SI]
		ADD		SI,1			
		CMP		AL,0 			; 比较AL与0，进而判断是否执行JE。相等则执行JE
		JE		fin 			; 如果AL==0,则跳转到fin，否则跳过这个指令  注：fin为结束标号
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BOIS  注:INT中断，用来调用BIOS函数
		JMP		putloop			; 跳转
fin:
		HLT						; 停止CPU，等待指令
		JMP		fin				; 无限循环

msg:
		DB		0x0a, 0x0a		; 换行
		DB		"hello, world"
		DB		0x0a			; 换行
		DB		"BY Shen Jinsheng"
		DB		0

		RESB	0x7dfe-$		

		DB		0x55, 0xaa

; 其他输出

		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432
