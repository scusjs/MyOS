; Dogged-os
; TAB=4

; 有关BOOT_INFO
CYLS	EQU		0x0ff0	;设定启动区
LEDS	EQU		0x0ff1	;键盘LED指示灯状态
VMODE	EQU		0x0ff2	;颜色的位数
SCRNX	EQU		0x0ff4	;分辨率的X
SCRNY	EQU		0x0ff6	;分辨率的Y
VRAM	EQU		0x0ff8	;图像缓冲区的开始地址

	ORG		0xc200		;程序装载在内存中位置
	MOV		AL,0x13		;VGA显卡，320*200*8位色
	MOV		AH,0x00
	INT		0x10
	MOV		BYTE [VMODE],8 			;记录画面模式
	MOV		WORD [SCRNX],320
	MOV 	WORD [SCRNY],200
	MOV 	DWORD [VRAM],0x000a0000

;获取键盘上各种LED指示灯的状态
	MOV		AH,0x02
	INT		0x16			;键盘BIOS
	MOV 	[LEDS],AL


fin:
	HLT
	JMP		fin
