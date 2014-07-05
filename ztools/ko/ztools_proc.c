#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/version.h>

#include "ztools.h"
#include <linux/seq_file.h>

struct proc_dir_entry *ztools_dir = NULL;

static int ztools_write_fun_cmd(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len;
	int i=0;
	char p[100];
	char *argv[30];
	
	unsigned int reg_addr;
	unsigned int reg_data;
	unsigned int phy_addr;
	unsigned int vir_addr;

	
	data = p;
	memset(p,0,sizeof(p));
	if(copy_from_user(data,buffer,count))
		return -EFAULT;
	printk("cmdline is %s\n",(char *)data);

/*解析命令行*/
	for(len = 0;len < count;)
	{
		if(*(p+len) == ' ')
			while(*(p + (++len)) != ' ');

		argv[i] = p+len;
		i++;
		while(*(p+len) != ' ')
		{
			if(*(p+len) == 0)
			{
				goto done;
			}
			len++;
			
		}
		*(p+(len++)) = 0;
	}
	
done:
	for(len=0;len<i;len++)
		printk("%d %s\n",len,argv[len]);
	
	if(strncmp("init",argv[0],4) == 0)
	{

	}
	else if(strncmp("memread",argv[0],7) == 0)
	{
		reg_addr = simple_strtol(argv[1],NULL,0);
		len = simple_strtol(argv[2],NULL,0);

		printk("\n****************************\n\n");
		printk("Read addr[0x%08x],len[%d] : ",reg_addr,len);
		for(i=0; i<len; i++)
		{
			if(i%4 == 0)
				printk("\n 0x%08x : ",reg_addr+i*4);
			printk("0x%08x ",z_read32(reg_addr+i*4));
		}
		printk("\n\n****************************\n");

	}
	else if(strncmp("memwrite",argv[0],8) == 0)
	{
		reg_addr = simple_strtol(argv[1],NULL,0);
		reg_data = simple_strtol(argv[2],NULL,0);

		printk("\n****************************\n\n");
		printk("Write addr[0x%x],data[0x%x] : ",reg_addr,reg_data);
		z_write32(reg_addr,reg_data);
		printk("\n\n****************************\n");
	}
	else if(strncmp("ioremap",argv[0],7) == 0)
	{
		phy_addr = simple_strtol(argv[1],NULL,0);
		len = simple_strtol(argv[2],NULL,0);

		printk("\n****************************\n\n");
		vir_addr = ioremap(phy_addr,len);
		printk("mmap faddr[0x%08x],vaddr[0x%08x] : ",phy_addr,vir_addr);
		printk("\n\n****************************\n");
	}
	
	return count;	
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
/**/

int val=10;

/*首先实现基本的读写函数*/
static int ztools_read_fun_cmd(char *page, char **start, off_t off, int count, int *eof, void *data)
{  
   int ret = 0;
  
    ret = sprintf(page + ret, "val = %d\n", val);

   *eof=1;  
   return ret;
}


/*创建文件*/
static void ztools_create_proc_entry_cmd(void)
{
	struct proc_dir_entry *entry = NULL;
	entry = create_proc_entry("cmd", S_IFREG | S_IRUGO | S_IWUSR, ztools_dir);
	
	if(!entry)
	{
		printk("err create proc/dir\n");		
	}
	else
	{
		entry->read_proc  = ztools_read_fun_cmd;
		entry->write_proc = ztools_write_fun_cmd;
	}
}

/*******************************************************************************************/
static int ztools_help_fun(char *page, char **start, off_t off, int count, int *eof, void *data)
{  
   int ret = 0;
  
    ret = sprintf(page + ret, "Ztools help v1.0\n");
    ret += sprintf(page + ret, "Usage:\n");
    ret += sprintf(page + ret, "\techo cmdline > /proc/ztools/cmd\n");
    ret += sprintf(page + ret, "\techo memread addr len\n");
    ret += sprintf(page + ret, "\techo memwrite addr data\n");
    ret += sprintf(page + ret, "\techo ioremap addr len\n");

   *eof=1;  
   return ret;
}

/*******************************************************************************************/
int ztools_proc_init(void)
{
	printk("ztools proc init\n");	

/*核心函数*/
	ztools_dir = proc_mkdir("ztools",NULL);  /*创建目录*/
	ztools_create_proc_entry_cmd();

/*帮助文件*/
	create_proc_read_entry("help", 0, ztools_dir, ztools_help_fun, NULL);

	return 0;
}

void ztools_proc_free(void)
{
	printk("ztools dev free\n");

	remove_proc_entry("cmd", ztools_dir);
	remove_proc_entry("help", ztools_dir);

	remove_proc_entry("ztools", NULL);

	return;
}
#else

static int c_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Ztools help v1.0\n");
    seq_printf(m, "Usage:\n");
    seq_printf(m, "\techo cmdline > /proc/ztools/cmd\n");
    seq_printf(m, "\techo memread addr len\n");
    seq_printf(m, "\techo memwrite addr data\n");
    seq_printf(m, "\techo ioremap addr len\n");

	return 0;
}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	return *pos < 1 ? (void *)1 : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return NULL;
}

static void c_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations ztools_proc_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= c_show
};

static int ztools_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ztools_proc_op);
}

static const struct file_operations proc_ztools_help_operations = {
	.open		= ztools_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static const struct file_operations proc_ztools_cmd_operations = {
	.open		= ztools_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
	.write		= ztools_write_fun_cmd,
};

int ztools_proc_init(void)
{
	printk("ztools proc init\n");	

	ztools_dir = proc_mkdir("ztools",NULL);
	proc_create("help", 0, ztools_dir, &proc_ztools_help_operations);
	proc_create("cmd", 0, ztools_dir, &proc_ztools_cmd_operations);
	
	return 0;
}
 void ztools_proc_free(void)
{
	printk("ztools dev free\n");

	remove_proc_entry("cmd", ztools_dir);
	remove_proc_entry("help", ztools_dir);
	remove_proc_entry("ztools", NULL);
}
#endif