; dogged-os
; TAB=4
		CYLS	EQU		10		; 定义CYLS=10

		ORG		0x7c00			; 指明装载地址

; 以下标注FAT12格式软盘

		JMP		entry
		DB		0x90
		DB		"DOGGED  "		;启动区的名称 ,8字节		
		DW		512				;每个扇区大小，必须是512字节		
		DB		1				;簇的大小
		DW		1				;FAT的起始位置		
		DB		2				;FAT的个数	
		DW		224				;根目录大小
		DW		2880			;该磁盘大小
		DB		0xf0			;磁盘种类
		DW		9				;FAT长度
		DW		18				;1个磁道有几个扇区
		DW		2				;磁头数，必须为2
		DD		0				;不使用分区
		DD		2880			;重写一次磁盘大小
		DB		0,0,0x29		
		DD		0xffffffff		;卷标号码
		DB		"HELLO-OS   "	;磁盘名称
		DB		"FAT12   "		;磁盘格式名称
		RESB	18		;空出18个字节，并置为0

; 程序核心

entry:							; 标签
		MOV		AX,0			; 初始化寄存器 注，AX累加寄存器	BX基址寄存器	CX计数寄存器	DX数据寄存器
		MOV		SS,AX 			; SS栈段寄存器
		MOV		SP,0x7c00		; SP栈指针寄存器
		MOV		DS,AX 			; DS数据段寄存器

; 读盘
		MOV		AX,0x0820 		; 装载到内存0x0820，因为0x0800以后内存为空且0x0800-0x08ff已经被ipl占用
		MOV		ES,AX
		MOV		CH,0			; 柱面0
		MOV		DH,0			; 磁头0
		MOV		CL,2			; 扇区2
readloop:
		MOV 	SI,0 			; SI寄存器记录失败次数

retry:


		MOV		AH,0x02			; AH=0x02 : 读盘
		MOV		AL,1			; 1个扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用磁盘BIOS
		JNC 	next 			; 没有错误则跳转到next
		ADD 	SI,1 			; SI加一
		CMP 	SI,5 			; 比较SI和5，大于等于5则跳转到error
		JAE 	error 			
		MOV		AH,0x00
		MOV 	DL,0x00 		; A驱动器
		INT 	0x13 			; 重置驱动
		JMP 	retry
next:
		MOV		AX,ES			; 把内存地址后移0x200
		ADD		AX,0x0020
		MOV		ES,AX
		ADD 	CL,1 			; CL加一，进行比较
		CMP		CL,18
		JBE		readloop
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2 			; DH小于2则跳转
		JB 		readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB 		readloop




; 调用磁盘

		MOV		[0x0ff0],CH		; IPL装载地址告诉dogged.sys
		JMP		0xc200

error:
		MOV		SI,msg		
;success:
;		MOV		SI,msg	
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


；以下为输出信息


msg:
		DB		0x0a,0x0a			; 换行
		DB 		"Load error"
		DB 		0x0a
		DB		"BY Shen Jinsheng"
		DB		0

		RESB	0x7dfe-$		

		DB		0x55, 0xaa
