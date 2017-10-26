#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/highmem.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/kmod.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/delay.h>
#include "ztools.h"

/***********************************************���ж�****************************************/
#define SEA_SOFTIRQ 17

/*���жϴ�������*/
static void sea_softirq_action(struct softirq_action *a)
{
	printk(KERN_ALERT "sea softirq run\n");
}

/*ע�Ტ����һ�����ж�*/
static void init_softirq(void)
{
/*ע��һ�����ж�,�ں˲�û�е�����������������Ҫ�޸��ں�*/
	open_softirq(SEA_SOFTIRQ, sea_softirq_action);

/*����һ�����ж�*/
	raise_softirq(SEA_SOFTIRQ);
}

/***********************************************tasklet****************************************/
/*tasklet�����жϴ�����tasklet_action���е���*/
struct tasklet_struct tasklet_sea;

void sea_print(unsigned long val)
{
	printk("sea tasklet print\n");
	return;
}

/*ע�Ტ����һ��tasklet*/
static void init_tasklet(void)
{
	tasklet_init(&tasklet_sea, sea_print, NULL);
	tasklet_schedule(&tasklet_sea);	
}

/*******************************************************************************************/
static void softirq(int argc,char *argv[])
{
	printk("\n****************************\n\n");
	printk("irq dev init\n");	
	init_softirq();
	init_tasklet();
	printk("\n\n****************************\n");
}
DEFINE_ZTOOLS_CMD(softirq, "softirq");   
