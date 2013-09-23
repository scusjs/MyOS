/* dsctbl.c
 * 关于GDT,IDT等
 * descriptor table的处理
 * 
 *
 *设置GDT 
 * sd：GDT表首址 
 * limit：段限长  
 * base:段指向代码或数据段地址 
 * ar:段管理属性  00000000(0x00)：未使用的记录表 
 *       10010010(0x92)：os用，可读写段，不可执行，ring0 
 *       10011010(0x9a)：os用，可执行段，可读不可写，ring0 
 *       11110010(0xf2)：app用，可读写，不可执行，ring3 
 *       11111010(0xfa)：app用，可执行，可读不可写，ring3 
 */

#include "bootpack.h"

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;//任意没有被占用的内存空间
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;//任意没有被占用的内存空间
	int i;

	/* GDT的初始化 */
	for (i = 0; i < 8192; i++) {//段选择子用12位表示段值，故最大8192 
		set_segmdesc(gdt + i, 0, 0, 0);//实质上内部存在转换：gdt+i*8。这里全部初始化为0
	}
	//0号段保留NULL Description  
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);//cpu能管理的全部4G内存 os数据段
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);//bootpack.hrb所在512KB内存 os代码段
	load_gdtr(LIMIT_GDT, ADR_GDT);//给GDTR赋值。将GDT表首地址加载到GDTR  

	/* IDT的初始化 */
	for (i = 0; i < 256; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);

	/* IDT的设定 */
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);

	return;
}


void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
