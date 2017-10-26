#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <linux/kmod.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <asm/cacheflush.h>

#include "ztools.h"

static void cache(int argc,char *argv[])
{
	unsigned int size,times,i,j;
	void *data;
	struct timeval tv_old;
	struct timeval tv_new;
	
	size = simple_strtol(argv[1],NULL,0);
	times = simple_strtol(argv[2],NULL,0);

	printk("\n****************************\n\n");
	data = kmalloc(size*1024, GFP_KERNEL);
	if(data == NULL)
		return;
	printk("cache read test size:%dKB times:%d\n",size,times);

	printk("first time read:\n");
	do_gettimeofday(&tv_old);
	for(i=0; i<times; i++)
	{
		for(j=0;j<size;j+=4)
			z_read32(data+j);
	}
	do_gettimeofday(&tv_new);
	printk("time used %ld.%6ld\n",tv_new.tv_sec-tv_old.tv_sec, tv_new.tv_usec-tv_old.tv_usec);

	printk("second time read:\n");
	do_gettimeofday(&tv_old);
	for(i=0; i<times; i++)
	{
		for(j=0;j<size;j+=4)
			z_read32(data+j);
	}
	do_gettimeofday(&tv_new);
	printk("time used %ld.%6ld\n",tv_new.tv_sec-tv_old.tv_sec, tv_new.tv_usec-tv_old.tv_usec);

	printk("with cache flush:\n");
	flush_cache_all();
	do_gettimeofday(&tv_old);
	for(i=0; i<times; i++)
	{
		for(j=0;j<size;j+=4)
			z_read32(data+j);
	}
	do_gettimeofday(&tv_new);
	printk("time used %ld.%6ld\n",tv_new.tv_sec-tv_old.tv_sec, tv_new.tv_usec-tv_old.tv_usec);

	kfree(data);
	printk("\n\n****************************\n");
}
DEFINE_ZTOOLS_CMD(cache, "cache ksize times");   
