/*
 * mtask.c
 * 多任务处理相关函数
 */
 #include "bootpack.h"

struct TASKCTL *taskctl;
struct TIMER *task_timer;


struct TASK *task_init(struct MEMMAN *memman)
/* 任务相关结构体初始化 */
{
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}

	task = task_alloc();
	task->flags = 2; //活动中的标志
	/* 
     * 关于task结构中flags成员变量的取值
     * 0是未活动 	1是休眠 	2是运行 
     */
    task->priority = 2;	//0.02秒
    task->level = 0;
	task_add(task);
	task_switchsub();
	load_tr(task->sel);	//修改TR寄存器
	task_timer = timer_alloc();	//任务切换定时器
	timer_settime(task_timer, task->priority);

	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS-1, 1);
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

void task_run(struct TASK *task, int level, int priority)
/* 运行任务 */
{
	if (level < 0)
	{
		level = task->level;
	}
	if (priority > 0)	//为0表示不更改当前的优先级
	{
		task->priority = priority;
	}
	if (task->flags == 2 && task->level != level)	//改变任务level
	{
		task_remove(task);
	}
	if (task->flags != 2)
	{
		task->level = level;
		task_add(task);
	}
	taskctl->lv_change = 1;	//下次任务切换时检查level
	return;
}

void task_switch(void)
/* 任务切换 */
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}

void task_sleep(struct TASK *task)
/* 任务休眠 */
{
	struct TASK *now_task;
	if (task->flags == 2)//任务处于唤醒状态
	{
		now_task = task_now();
		task_remove(task);
		if (task == now_task)
		{
			task_switchsub();
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}
	return;
}

struct TASK *task_now(void)
//返回现在活动中的struct TASK的内存地址
{
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

void task_add(struct TASK *task)
//向struct TASKLEVEL中添加一个任务
{
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2;
	return;
}

void task_remove(struct TASK *task)
//删除任务
{
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	//寻找task所在的位置
	for (i = 0; i < tl->running; i++)
	{
		if (tl->tasks[i] == task)
		{
			break;
		}
	}
	tl->running--;
	if (i < tl->now)
	{
		tl->now--;
	}
	if (tl->now >= tl->running)
	{
		tl->now = 0;
	}
	task->flags = 1;

	//移动
	for (; i < tl->running; i++)
	{
		tl->tasks[i] = tl->tasks[i+1];
	}
	return;
}

void task_switchsub(void)
//决定任务切换时切换到哪个LEVEL
{
	int i;
	//寻找最上层的level
	for (i = 0; i < MAX_TASKLEVELS; i++)
	{
		if (taskctl->level[i].running > 0)
		{
			break;
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}

void task_idle(void)
//闲置任务，防止没有任务执行
{
	for (;;)
	{
		io_hlt();
	}
}