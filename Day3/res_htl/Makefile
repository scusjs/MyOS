TOOLPATH = ../z_tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del



default :
	$(MAKE) img



ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

dogged.sys :dogged.nas Makefile
	$(NASK) dogged.nas dogged.sys dogged.lst

dogged.img : ipl10.bin dogged.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:dogged.sys to:@: \
		imgout:dogged.img



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
	-$(DEL) ipl10.bin
	-$(DEL) ipl10.lst
	-$(DEL) dogged.sys
	-$(DEL) dogged.lst

src_only :
	$(MAKE) clean
	-$(DEL) dogged.img
