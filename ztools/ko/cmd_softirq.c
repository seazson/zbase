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

/***********************************************软中断****************************************/
#define SEA_SOFTIRQ 17

/*软中断处理例程*/
static void sea_softirq_action(struct softirq_action *a)
{
	printk(KERN_ALERT "sea softirq run\n");
}

/*注册并触发一个软中断*/
static void init_softirq(void)
{
/*注册一个软中断,内核并没有导出这两个函数，需要修改内核*/
	open_softirq(SEA_SOFTIRQ, sea_softirq_action);

/*触发一个软中断*/
	raise_softirq(SEA_SOFTIRQ);
}

/***********************************************tasklet****************************************/
/*tasklet由软中断处理函数tasklet_action进行调度*/
struct tasklet_struct tasklet_sea;

void sea_print(unsigned long val)
{
	printk("sea tasklet print\n");
	return;
}

/*注册并触发一个tasklet*/
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
