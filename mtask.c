/*
 * mtask.c
 * 多任务处理相关函数
 */
 #include "bootpack.h"

 struct TIMER *mt_timer;
 int mt_tr;	//TR寄存器



struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *memman)
/* 任务相关结构体初始化 */
{
	int i;
	struct TASK *task;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof(struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) 
	{
		/* 初始化所有的任务的task结构 */
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;	//选择子初始化
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32); //描述符初始化
	}
	task = task_alloc();
	task->flags = 2; //活动中的标志
	/* 
     * 关于task结构中flags成员变量的取值
     * 0是未活动 	1是休眠 	2是运行 
     */
	taskctl->running = 1;
	taskctl->now = 0;
	taskctl->tasks[0] = task;
	load_tr(task->sel);	//修改TR寄存器
	task_timer = timer_alloc();	//任务切换定时器
	timer_settime(task_timer, 2);
	return task;
}

struct TASK *task_alloc(void)
/* 任务分配 */
{
	int i;
	struct TASK *task;
	/* 遍历所有的task结构 */
	for (i = 0; i < MAX_TASKS; i++)
	{
		if (taskctl->tasks0[i].flags == 0) //未活动
		{
			task = &taskctl->tasks0[i];
			task->flags = 1; //正在使用
			task->tss.eflags = 0x00000202; //IF = 1
			/* 初始化各个寄存器 */
			task->tss.eax = 0; 
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			return task;
		}
	}
	return 0; //全部正在使用
}

void task_run(struct TASK *task)
/* 运行任务 */
{
	task->flags = 2; //活动中
	taskctl->tasks[taskctl->running] = task;
	taskctl->running++;
	return;
}

void task_switch(void)
/* 任务切换 */
{
	timer_settime(task_timer, 2);
	if (taskctl->running >= 2) {
		taskctl->now++;
		if (taskctl->now == taskctl->running) {
			taskctl->now = 0;
		}
		farjmp(0, taskctl->tasks[taskctl->now]->sel);
	}
	return;
}