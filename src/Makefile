OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj \
		int.obj fifo.obj mouse.obj keyboard.obj memory.obj sheet.obj timer.obj \
		mtask.obj window.obj console.obj file.obj

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/dogged/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
BIM2DOG  = $(TOOLPATH)bim2dog.exe
RULEFILE = $(TOOLPATH)dogged/dogged.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# 

default :
	$(MAKE) img

# 

ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.dog : bootpack.bim Makefile
	$(BIM2DOG) bootpack.bim bootpack.dog 0

hello.dog : hello.nas Makefile
	$(NASK) hello.nas hello.dog hello.lst

hello2.dog : hello2.nas Makefile
	$(NASK) hello2.nas hello2.dog hello2.lst

a.bim : a.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:a.bim map:a.map a.obj a_nask.obj

a.dog : a.bim Makefile
	$(BIM2DOG) a.bim a.dog 0

hello3.bim : hello3.obj a_nask.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:hello3.bim map:hello3.map hello3.obj a_nask.obj

hello3.dog : hello3.bim Makefile
	$(BIM2DOG) hello3.bim hello3.dog 0

dogged.sys : asmhead.bin bootpack.dog Makefile
	copy /B asmhead.bin+bootpack.dog dogged.sys

dogged.img : ipl10.bin dogged.sys hello.dog hello2.dog a.dog hello3.dog Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:dogged.sys to:@: \
		copy from:ipl10.nas to:@: \
		copy from:make.bat to:@: \
		copy from:hello.dog to:@: \
		copy from:hello2.dog to:@: \
		copy from:hello3.dog to:@: \
		copy from:a.dog to:@: \
		imgout:dogged.img

# 

%.gas : %.c  bootpack.h Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# 

img :
	$(MAKE) dogged.img

run :
	$(MAKE) img
	$(COPY) dogged.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install :
	$(MAKE) img
	$(IMGTOL) w a: dogged.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.dog
	-$(DEL) dogged.sys
	-$(DEL) *.dog

src_only :
	$(MAKE) clean
	-$(DEL) dogged.img
