; naskfunc
; TAB=4

[FORMAT "WCOFF"]				; 制作目标文件的模式	
[INSTRSET "i486p"]				; 表示适用于486，即使用32位寄存器
[BITS 32]						; 制作32位模式用的机器语言


; 制作目标文件的信息

[FILE "naskfunc.nas"]			; 源文件名信息

		GLOBAL	_io_hlt, _io_cli, _io_sti, _io_stihlt
		GLOBAL	_io_in8,  _io_in16,  _io_in32
		GLOBAL	_io_out8, _io_out16, _io_out32
		GLOBAL	_io_load_eflags, _io_store_eflags
		GLOBAL	_load_gdtr, _load_idtr
		GLOBAL	_load_cr0, _store_cr0
		GLOBAL	_load_tr
		GLOBAL	_farjmp, _farcall
		GLOBAL	_start_app
		GLOBAL	_asm_cons_putchar, _asm_dogged_api
		GLOBAL	_asm_inthandler20, _asm_inthandler21, _asm_inthandler27, _asm_inthandler2c
		GLOBAL	_memtest_sub
		EXTERN	_inthandler20, _inthandler21, _inthandler27, _inthandler2c
		EXTERN	_cons_putchar, _dogged_api

[SECTION .text]

_io_hlt:	; void io_hlt(void);
		HLT
		RET

_io_cli:	; void io_cli(void);
		CLI 						; 置IF=0，CPU禁止响应外部中断 
		RET

_io_sti:	; void io_sti(void);
		STI 						; 置IF=1，使CPU允许向应外部中断 
		RET

_io_stihlt:	; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:	; int io_in8(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_io_in16:	; int io_in16(int port);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_io_in32:	; int io_in32(int port);
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_io_out8:	; void io_out8(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_io_out16:	; void io_out16(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_io_out32:	; void io_out32(int port, int data);
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_io_load_eflags:	; int io_load_eflags(void);
		PUSHFD		; PUSH EFLAGS 
		POP		EAX
		RET

_io_store_eflags:	; void io_store_eflags(int eflags);
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD		; POP EFLAGS 
		RET
_load_gdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]			; 将源操作数中的值加载到全局描述符表格寄存器 (GDTR) 
		RET

_load_idtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]			; 将源操作数中的值加载到中断描述符表格寄存器 (IDTR) 
		RET

_load_cr0:		; int load_cr0(void);
		MOV		EAX,CR0
		RET

_store_cr0:		; void store_cr0(int cr0);
		MOV		EAX,[ESP+4]
		MOV		CR0,EAX
		RET

_load_tr:		;void load_tr(int tr);
		LTR 	[ESP+4]		;tr
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		AX,SS
		CMP 	AX,1*8
		JNE 	.from_app

		MOV		EAX,ESP
		PUSH 	SS
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler20

		ADD 	ESP,8
		POPAD
		POP		DS
		POP		ES
		IRETD

.from_app:
;当应用程序活动时发生的中断
		MOV 	EAX,1*8
		MOV 	DS,AX
		MOV 	ECX,[0xfe4] 	;操作系统的esp
		ADD 	ECX,-8
		MOV 	[ECX+4],SS
		MOV 	[ECX],ESP
		MOV 	SS,AX
		MOV 	ES,AX
		MOV 	ESP,ECX
		CALL 	_inthandler20	
		POP 	ECX
		POP 	EAX
		MOV 	SS,AX
		MOV 	ESP,ECX
		POPAD
		POP 	DS
		POP 	ES
		IRETD	


_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		AX,SS
		CMP 	AX,1*8
		JNE 	.from_app

		MOV		EAX,ESP
		PUSH 	SS
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler21

		ADD 	ESP,8
		POPAD
		POP		DS
		POP		ES
		IRETD

.from_app:
;当应用程序活动时发生的中断
		MOV 	EAX,1*8
		MOV 	DS,AX
		MOV 	ECX,[0xfe4] 	;操作系统的esp
		ADD 	ECX,-8
		MOV 	[ECX+4],SS
		MOV 	[ECX],ESP
		MOV 	SS,AX
		MOV 	ES,AX
		MOV 	ESP,ECX
		CALL 	_inthandler21	
		POP 	ECX
		POP 	EAX
		MOV 	SS,AX
		MOV 	ESP,ECX
		POPAD
		POP 	DS
		POP 	ES
		IRETD	

_asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		AX,SS
		CMP 	AX,1*8
		JNE 	.from_app

		MOV		EAX,ESP
		PUSH 	SS
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler27

		ADD 	ESP,8
		POPAD
		POP		DS
		POP		ES
		IRETD

.from_app:
;当应用程序活动时发生的中断
		MOV 	EAX,1*8
		MOV 	DS,AX
		MOV 	ECX,[0xfe4] 	;操作系统的esp
		ADD 	ECX,-8
		MOV 	[ECX+4],SS
		MOV 	[ECX],ESP
		MOV 	SS,AX
		MOV 	ES,AX
		MOV 	ESP,ECX
		CALL 	_inthandler27	
		POP 	ECX
		POP 	EAX
		MOV 	SS,AX
		MOV 	ESP,ECX
		POPAD
		POP 	DS
		POP 	ES
		IRETD	

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		AX,SS
		CMP 	AX,1*8
		JNE 	.from_app

		MOV		EAX,ESP
		PUSH 	SS
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	_inthandler2c

		ADD 	ESP,8
		POPAD
		POP		DS
		POP		ES
		IRETD

.from_app:
;当应用程序活动时发生的中断
		MOV 	EAX,1*8
		MOV 	DS,AX
		MOV 	ECX,[0xfe4] 	;操作系统的esp
		ADD 	ECX,-8
		MOV 	[ECX+4],SS
		MOV 	[ECX],ESP
		MOV 	SS,AX
		MOV 	ES,AX
		MOV 	ESP,ECX
		CALL 	_inthandler2c	
		POP 	ECX
		POP 	EAX
		MOV 	SS,AX
		MOV 	ESP,ECX
		POPAD
		POP 	DS
		POP 	ES
		IRETD	

_memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		PUSH	EDI						; （由于还要使用EBX, ESI, EDI）
		PUSH	ESI
		PUSH	EBX
		MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
		MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
		MOV		EAX,[ESP+12+4]			; i = start;
mts_loop:
		MOV		EBX,EAX
		ADD		EBX,0xffc				; p = i + 0xffc;
		MOV		EDX,[EBX]				; old = *p;
		MOV		[EBX],ESI				; *p = pat0;
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
		JNE		mts_fin
		XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
		CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
		JNE		mts_fin
		MOV		[EBX],EDX				; *p = old;
		ADD		EAX,0x1000				; i += 0x1000;
		CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET
mts_fin:
		MOV		[EBX],EDX				; *p = old;
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_farjmp:		; void farjmp(int eip, int cs);
		JMP		FAR [ESP+4]		;eip,cs
		RET

_asm_cons_putchar:
		STI
		PUSHAD
		PUSH	1
		AND		EAX,0xff	; 将AH和EAX的高位置置1，将EAX置为已存入字符编码的状态
		PUSH	EAX
		PUSH	DWORD [0x0fec]	; 读取内存并PUSH该值
		CALL	_cons_putchar
		ADD		ESP,12		; 将栈中的数据丢弃
		POPAD
		IRETD

_farcall:		; void farcall(int eip, int cs);
		CALL	FAR [ESP+4]
		RET

_asm_dogged_api:	; void dogged_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
		STI
		PUSH 	DS
		PUSH 	ES
		PUSHAD
		MOV 	EAX,1*8
		MOV 	DS,AX 	;将DS设定为操作系统用
		MOV 	ECX,[0xfe4]
		ADD 	ECX,-40
		MOV 	[ECX+32],ESP 	;保护应用程序的ESP
		MOV 	[ECX+36],SS
		;将PUSHAD后的值复制到系统栈
		MOV		EDX,[ESP]
		MOV 	EBX,[ESP+4]
		MOV 	[ECX],EDX
		MOV 	[ECX+4],EBX
		MOV		EDX,[ESP+8]
		MOV 	EBX,[ESP+12]
		MOV 	[ECX+8],EDX
		MOV 	[ECX+12],EBX
		MOV		EDX,[ESP+16]
		MOV 	EBX,[ESP+20]
		MOV 	[ECX+16],EDX
		MOV 	[ECX+20],EBX
		MOV		EDX,[ESP+24]
		MOV 	EBX,[ESP+28]
		MOV 	[ECX+24],EDX
		MOV 	[ECX+28],EBX
		MOV 	ES,AX 	;剩余段寄存器设为操作系统使用
		MOV 	SS,AX
		MOV 	ESP,ECX
		STI
		CALL	_dogged_api
		MOV 	ECX,[ESP+32]
		MOV 	EAX,[ESP+36] 	;取应用程序SS
		CLI
		MOV 	SS,AX
		MOV 	ESP,ECX
		POPAD
		POP 	ES
		POP 	DS
		IRETD

_start_app:		;void start_app(int eip, int cs, int esp, int ds);
		PUSHAD
		MOV 	EAX,[ESP+36]	;应用程序用EIP
		MOV 	ECX,[ESP+40]	;应用程序用CS
		MOV 	EDX,[ESP+44]	;应用程序用ESP
		MOV 	EBX,[ESP+48]	;应用程序用DS/SS
		MOV 	[0xfe4],ESP 	;操作系统用ESP
		CLI
		MOV 	ES,BX
		MOV 	SS,BX
		MOV 	DS,BX
		MOV 	FS,BX
		MOV 	GS,BX
		MOV 	ESP,EDX
		STI
		PUSH 	ECX 	;用于far_CALL的push(cs)
		PUSH 	EAX 	;用于far_CALL的push(eip)
		CALL 	FAR [ESP] 	;调用应用程序
		MOV 	EAX,1*8 	;操作系统用DS/SS
		CLI
		MOV 	ES,AX
		MOV 	SS,AX
		MOV 	DS,AX
		MOV 	FS,AX
		MOV 	GS,AX
		MOV 	ESP,[0xfe4]
		STI
		POPAD
		RET
