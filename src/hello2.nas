[INSTRSET "i486p"]
[BITS 32]
	MOV		EDX,2
	MOV		EBX,msg
	INT 	0x40
	RETF
msg:
	DB		"what a fuck",0