/* 
 * sheet.c
 * 叠加显示相关函数
 */

#include "bootpack.h"

#define SHEET_USE		1

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
/* 图层控制初始化 */
{
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));	//分配内存
	if (ctl == 0) {
		return ctl;
	}
	ctl->map = (unsigned char *)memman_alloc_4k(memman,xsize*ysize);
	if (ctl->map == 0)
	{
		memman_free_4k(memman, (int)ctl, sizeof(struct SHTCTL));
		return ctl;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* 一个SHEEL都没有 */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = 0; /* 标记为未使用 */
		ctl->sheets0[i].ctl = ctl; /* 记录所属 */
	}
	return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl)
/* 取得新生成的未使用图层 */
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* 标记为正在使用 */
			sht->height = -1; /* 隐藏 */
			return sht;
		}
	}
	return 0;	/* 所有的图层都处于正在使用的状态，即分配失败 */
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
/* 设置图层相关信息 */
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
/* 向map中写入图层信息 */
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid, *map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0; /* 将进行了减法计算的地址作为图层号码使用,即sid是图层在sheets0中的下标 */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (buf[by * sht->bxsize + bx] != sht->col_inv) {
					map[vy * ctl->xsize + vx] = sid;
				}
			}
		}
	}
	return;
}

void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
/* 刷新部分界面 */
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, *map = ctl->map, sid, *vram = ctl->vram;
	struct SHEET *sht;
	/* 如果refresh范围超出了画面则进行修正 */
	if (vx0 < 0)
	{
		vx0 = 0;
	}
	if (vy0 < 0)
	{
		vy0 = 0;
	}
	if (vx1 > ctl->xsize)
	{
		vx1 = ctl->xsize;
	}
	if (vy1 > ctl->ysize)
	{
		vy1 = ctl->ysize;
	}
	for (h = h0; h <= h1; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		/* 使用vx0~vy1对bx0~by1进行倒推 */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (map[vy*ctl->xsize + vx] == sid)
				{
					vram[vy*ctl->xsize + vx] = buf[by*sht->bxsize +bx];
				}
			}
		}
	}
	return;
}

void sheet_updown(struct SHEET *sht, int height)
/* 设定图层高度 */
{
	int h, old = sht->height; /* 储存设置前的高度信息 */
	struct SHTCTL *ctl = sht->ctl;

	/* 修正高度 */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* 设定高度 */

	/* 进行sheets[]的排列 */
	if (old > height) {	/* 比以前低 */
		if (height >= 0) {
			/* 把中间的向上提 */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old); /* 重新绘制 */
		} else {	/* 隐藏 */
			if (ctl->top > old) {
				/* 把上面的降下来 */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* 因为有图层被隐藏，所以图层总高度减小 */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1); /* 重新绘制 */
		}
	} else if (old < height) {	/* 比以前高 */
		if (old >= 0) {
			/* 把中间的拉下去 */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* 由隐藏变为显示 */
			/* 把上面的提上去 */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* 图层总高度加一 */
		}
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height); /* 重新绘制 */
	}
	return;
}

void sheet_refresh( struct SHEET *sht, int bx0, int by0, int bx1, int by1)
/* 刷新图层函数 */
{
	struct SHTCTL *ctl = sht->ctl;
	if (sht->height >= 0) { /* 如果图层显示才刷新 */
		sheet_refreshsub(ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

void sheet_slide( struct SHEET *sht, int vx0, int vy0)
/* 移动图层 */
{
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;	//记住原来的位置
	sht->vx0 = vx0;	//获得新的位置
	sht->vy0 = vy0;
	if (sht->height >= 0) {  //如果图层不是隐藏的则刷新 
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0+sht->bxsize, old_vy0+sht->bysize, 0);//将map中(old_vx0, old_vy0)(old_vx0 + sht->bxsize, old_vy0 + sht->bysize)定义的矩形区域重新设置
		sheet_refreshmap(ctl, vx0, vy0, vx0+sht->bxsize, vy0+sht->bysize,sht->height);
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);	//只是刷新原来的位置和新的位置
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

void sheet_free( struct SHEET *sht)
/* 删除图层 */
{
	struct SHTCTL *ctl = sht->ctl;
	if (sht->height >= 0) {
		sheet_updown(sht, -1); /* 先把图层隐藏 */
	}
	sht->flags = 0; /* 设定图层未使用 */
	return;
}


