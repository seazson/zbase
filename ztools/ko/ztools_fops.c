#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/fs.h>

#include "ztools.h"

static int ztools_open (struct inode *inode, struct file *file)
{
	return 0;
}

static int ztools_release (struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t ztools_read( struct file * file, char * buf,
                                size_t count, loff_t *ppos )
{
   return 0;
}

static ssize_t ztools_write( struct file * file, const char * buf,
                                size_t count, loff_t *ppos )
{
	int ret = 0;
	
    return ret;
}

static int ztools_ioctl (struct inode *inode, struct file *file,
                            unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	zdata_t zdata;
	switch(cmd)
	{
    	case CMD_MEM_WRITE:  
//			printk("write: index = %x offset = %x \r\n",write_cpld_reg.index,write_cpld_reg.offset);
//			printk("write: data = %x \r\n",write_cpld_reg.value);
	 
   	  	 
			ret = 0;
   	  break;    
	
	}
err:
	return ret;
}

struct file_operations ztools_fops = {
	.llseek		= NULL,
	.read		  = ztools_read,
	.write		= ztools_write,
	.poll		  = NULL,
#if 1
	.unlocked_ioctl	= ztools_ioctl,
#else
	.ioctl	= ztools_ioctl,
#endif
	.open		  = ztools_open,
	.release	= ztools_release,
	.mmap       = NULL,
	.fasync		= NULL,
};



