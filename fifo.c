/*
 *fifo.c文件
 *FOFO缓冲区相关函数
 */

#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

void fifo32_init(struct FIFO32 *fifo, int size, int *buf)
/* fifo缓冲区的初始化 */
{
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size; /* 初始化时空闲大小等于总大小 */
	fifo->flags = 0;
	fifo->p = 0; /* 下一个数据写入位置 */
	fifo->q = 0; /* 下一个数据读取位置 */
}

int fifo32_put(struct FIFO32 *fifo, int data)
/* 向FIFO传送数据并保存 */
{
	if (fifo->free == 0) {
		/* 没有空余空间，发生溢出 */
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
/* 从FIFO取得一个数据 */
{
	int data;
	if (fifo->free == fifo->size) {
		/* 缓冲区为空，返回-1 */
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
/* 获取缓冲区使用量 */
{
	return fifo->size - fifo->free;
}
