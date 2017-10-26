#include <linux/init.h>    
#include <linux/module.h>    
#include <linux/kernel.h>    
#include <linux/fs.h>  
#include <asm/uaccess.h>  
#include <linux/spinlock.h>  
#include <linux/sched.h>  
#include <linux/types.h>  
#include <linux/fcntl.h>  
#include <linux/hdreg.h>  
#include <linux/genhd.h>  
#include <linux/blkdev.h>  
  
#define MAXBUF 1024   
#define BLK_MAJOR 253  
  
char blk_dev_name[]="blk_dev";  
static char flash[1024*16];  
  
  
int major;  
spinlock_t lock;  
struct gendisk *gd;  
  
struct sblock_dev{
	int size;
	unsigned char *data;
	short users;
	short media_change;
	spinlock_t lock;
	struct request_queue *queue;
	struct gendisk *gd;
	struct timer_list timer;
};
  
  
/*���豸���ݴ���*/  
static void blk_transfer(unsigned long sector, unsigned long nsect, char *buffer, int write)  
{  
    int read = !write;  
    printk("blk_transfer\n");
	if(read)  
    {  
        memcpy(buffer, flash+sector*512, nsect*512);  
    }  
    else  
    {  
        memcpy(flash+sector*512, buffer, nsect*512);  
    }  
}  
  
/*���豸��������*/  
static void blk_request_func(struct request_queue *q)  
{  
    struct request *req;  
	unsigned int block, nsect;
    while((req = blk_fetch_request(q)) != NULL)    
    {  
        if(req->cmd_type != REQ_TYPE_FS)  
        {  
//            blk_end_request(req, 0, 0);  
            continue;  
        }  
        
		block = blk_rq_pos(req);
		nsect = blk_rq_sectors(req); 
		
        blk_transfer(block, nsect, req->buffer, rq_data_dir(req));  
        /*rq_data_dir��request������ݴ��͵ķ���*/  
        /*req->current_nr_sectors �ڵ�ǰ���н���ɵ�������*/  
        /*req->sector ���ύ����һ������*/  
        blk_end_request(req, 0, nsect*512);  
    }  
}  
  
/*strcut block_device_operations*/  
static  int blk_ioctl(struct block_device *dev, fmode_t no, unsigned cmd, unsigned long arg)  
{  
       return -ENOTTY;  
}  
  
static int blk_open (struct block_device *dev , fmode_t no)  
{  
    printk("blk mount succeed\n");  
    return 0;  
}  
static int blk_release(struct gendisk *gd , fmode_t no)  
{  
    printk("blk umount succeed\n");  
    return 0;  
}  
struct block_device_operations blk_ops=  
{  
    .owner = THIS_MODULE,  
    .open = blk_open,  
    .release = blk_release,  
    .ioctl = blk_ioctl,  
};  
  
//-----------------------------------------------  
  
static int __init block_module_init(void)  
{  
    if(!register_blkdev(BLK_MAJOR, blk_dev_name)) //ע��һ�����豸  
    {  
        major = BLK_MAJOR;    
        printk("regiser blk dev succeed\n");  
    }  
    else  
    {  
        return -EBUSY;  
    }  
    spin_lock_init(&lock); //��ʼ��һ��������  

    gd = alloc_disk(10);  //����һ��gendisk��������һ��  
    gd->major = major;    
    gd->first_minor = 0;   //��һ�����豸��  
    gd->fops = &blk_ops;   //������������  
  
    gd->queue = blk_init_queue(blk_request_func, &lock); //��ʼ��������в�������gendisk  
      snprintf(gd->disk_name, 32, "blk%c", 'a');    
    set_capacity(gd, 32);  //���ÿ��豸��С 512*32=16K  
    add_disk(gd);  
    printk("gendisk init success!\n");  

    return 0;  
}  
static void __exit block_module_exit(void)  
{  
    blk_cleanup_queue(gd->queue);  
    del_gendisk(gd);   
    unregister_blkdev(BLK_MAJOR, blk_dev_name);  
    printk("block module exit succeed!\n");  
}  
  
module_init(block_module_init);  
module_exit(block_module_exit);  
  
MODULE_LICENSE("GPL");  
MODULE_AUTHOR("gec");  