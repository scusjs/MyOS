; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式	
[INSTRSET "i486p"]				; 表示适用于486，即使用32位寄存器
[BITS 32]						; 制作32位模式用的机器语言


; 制作目标文件的信息

[FILE "naskfunc.nas"]			; 源文件名信息

		GLOBAL	_io_hlt,_write_mem8			; 程序中包含的函数


; 实际程序

[SECTION .text]		; 目标文件中写了这些后再写程序

_io_hlt:	; void io_hlt(void);
		HLT
		RET


_write_mem8:				; 等同于C语言的void write_mem8(int addr,int data)
	MOV		ECX,[ESP+4]		; 地址addr赋值到ECX
	MOV		AL,[ESP+8]		; 数据data读到AL
	MOV		[ECX],AL		; AL中数据放到addr中
	RET
