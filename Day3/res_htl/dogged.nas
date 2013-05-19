; Dogged-os
; TAB=4

	ORG		0xc200		;程序装载在内存中位置
	MOV		AL,0x13		;VGA显卡，320*200*8位色
	MOV		AH,0x00
	INT		0x10
fin:
	HLT
	JMP		fin
