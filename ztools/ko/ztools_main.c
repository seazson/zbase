#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>

#include "ztools.h"

static struct cdev ztools_cdev;
extern struct file_operations ztools_fops;

static int ztools_init(void)
{
	int ret;
    int devno = MKDEV(ZTOOLS_MAJOR,1);

	printk(KERN_ALERT "init ztools ver: %s %s\n",__DATE__,__TIME__);
        
    cdev_init(&ztools_cdev, &ztools_fops);
	ztools_cdev.owner = THIS_MODULE;
	  
    ret = cdev_add(&ztools_cdev, devno, 1);
    if(ret)
    {      
    	  printk("cannot register ext_mem dev\n"); 
          return ret;  
    }   
#ifdef CONFIG_PROC
	ztools_proc_init();
#endif
	
	return 0;
}

static void ztools_exit(void)
{
	printk(KERN_ALERT "exit");
#ifdef CONFIG_PROC
	ztools_proc_free();
#endif

}

module_init(ztools_init);
module_exit(ztools_exit);

