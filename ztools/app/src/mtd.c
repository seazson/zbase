#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#include "common.h"
/**************************.h********************************/
typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;


#define MTD_ABSENT		0
#define MTD_RAM			1
#define MTD_ROM			2
#define MTD_NORFLASH		3
#define MTD_NANDFLASH		4
#define MTD_DATAFLASH		6
#define MTD_UBIVOLUME		7

#define MTD_WRITEABLE		0x400	/* Device is writeable */
#define MTD_BIT_WRITEABLE	0x800	/* Single bits can be flipped */
#define MTD_NO_ERASE		0x1000	/* No erase necessary */
#define MTD_POWERUP_LOCK	0x2000	/* Always locked after reset */

// Some common devices / combinations of capabilities
#define MTD_CAP_ROM		0
#define MTD_CAP_RAM		(MTD_WRITEABLE | MTD_BIT_WRITEABLE | MTD_NO_ERASE)
#define MTD_CAP_NORFLASH	(MTD_WRITEABLE | MTD_BIT_WRITEABLE)
#define MTD_CAP_NANDFLASH	(MTD_WRITEABLE)

/* ECC byte placement */
#define MTD_NANDECC_OFF		0	// Switch off ECC (Not recommended)
#define MTD_NANDECC_PLACE	1	// Use the given placement in the structure (YAFFS1 legacy mode)
#define MTD_NANDECC_AUTOPLACE	2	// Use the default placement scheme
#define MTD_NANDECC_PLACEONLY	3	// Use the given placement in the structure (Do not store ecc result on read)
#define MTD_NANDECC_AUTOPL_USR 	4	// Use the given autoplacement scheme rather than using the default

/* OTP mode selection */
#define MTD_OTP_OFF		0
#define MTD_OTP_FACTORY		1
#define MTD_OTP_USER		2

struct mtd_info_user {
	__u8 type;
	__u32 flags;
	__u32 size;	 // Total size of the MTD
	__u32 erasesize;
	__u32 writesize;
	__u32 oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	__u32 ecctype;
	__u32 eccsize;
};


struct erase_info_user {
	__u32 start;
	__u32 length;
};

#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)
#define MEMERASE		_IOW('M', 2, struct erase_info_user)
#define MEMWRITEOOB		_IOWR('M', 3, struct mtd_oob_buf)
#define MEMREADOOB		_IOWR('M', 4, struct mtd_oob_buf)
#define MEMLOCK			_IOW('M', 5, struct erase_info_user)
#define MEMUNLOCK		_IOW('M', 6, struct erase_info_user)
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)
#define MEMSETOOBSEL		_IOW('M', 9, struct nand_oobinfo)
#define MEMGETOOBSEL		_IOR('M', 10, struct nand_oobinfo)
#define MEMGETBADBLOCK		_IOW('M', 11, __kernel_loff_t)
#define MEMSETBADBLOCK		_IOW('M', 12, __kernel_loff_t)
#define OTPSELECT		_IOR('M', 13, int)
#define OTPGETREGIONCOUNT	_IOW('M', 14, int)
#define OTPGETREGIONINFO	_IOW('M', 15, struct otp_info)
#define OTPLOCK			_IOR('M', 16, struct otp_info)
#define ECCGETLAYOUT		_IOR('M', 17, struct nand_ecclayout)
#define ECCGETSTATS		_IOR('M', 18, struct mtd_ecc_stats)
#define MTDFILEMODE		_IO('M', 19)


typedef struct mtd_info_user mtd_info_t;
typedef struct erase_info_user erase_info_t;
/**********************************************************************/
static int offset = 0;
static mtd_info_t g_mtd_info;

static char *mtd_type[]= {  "MTD_ABSENT",
	                 "MTD_RAM",
	                 "MTD_ROM",
	                 "MTD_NORFLASH",
	                 "MTD_NANDFLASH",
	                 "MTD_UNKNOWN",
	                 "MTD_DATAFLASH",
	                 "MTD_UBIVOLUME"
	                 };

static int getmtd(const char * devname, mtd_info_t* mtd)
{
	int dev_fd;
      
    if( devname == NULL )
    {
        printf("%s(): Parameter Error!\r\n", __FUNCTION__);
        return -1;
    }

    /* open mtd device */
    dev_fd = open (devname, O_SYNC | O_RDWR);
    if(dev_fd < 0)
    {
        printf("%s(): Can't open %s! errno=%d\r\n", __FUNCTION__, devname, errno);
        return -1;
    }

    /* read mtd device info */
    if (ioctl (dev_fd, MEMGETINFO, mtd) < 0)
    {
        printf ("%s(): This doesn't seem to be a valid MTD flash device! errno=%d\r\n", __FUNCTION__, errno);
        close(dev_fd);  
        return -1;
    }

    close(dev_fd);

	printf("**************MTD INFO****************\r\n");	
	printf("*MTD Type      : %s \r\n",mtd_type[mtd->type]);	
	printf("*MTD Size      : 0x%x \r\n",mtd->size);	
	printf("*MTD Erasesize : 0x%x \r\n",mtd->erasesize);	
	printf("**************************************\r\n");	

    return 0;	
}

/*********************************************************************

*********************************************************************/
static int readmtd(const char * devname, char * data, unsigned int len)
{
    int fd;
	int i,j;
	unsigned char print_buf[100];
	
    if ((fd = open (devname, O_RDONLY)) < 0)
    {
        printf("%s(): Can't open %s! errno=%d\r\n", __FUNCTION__, devname, errno);
		return -1;
    }

	if (lseek (fd, offset, SEEK_SET) < 0)
    {
	    printf("%s(): Error while seeking to start of %s\r\n", __FUNCTION__, devname);
        goto err;
    }

	if ((unsigned int)read (fd, data, len) != len)
    {
        printf("%s(): Read data error! errno=%d\r\n", __FUNCTION__, errno);
        goto err;
	}

    close(fd);

	memset(print_buf,0,100);
	for(i = 0; i < len; i++)
	{
		if(i%16==0)
		{
			printf("%s",print_buf);
			memset(print_buf,0,100);
			sprintf(print_buf,"\n|0x%08x : ",i);
		}

		sprintf(print_buf+15+(i%16)*3,"%02x ", *(data+i));

		if(*(data+i) >= 0x20 && *(data+i) < 128)
			print_buf[15+3*16+1+(i%16)] = *(data+i);
		else
			print_buf[15+3*16+1+(i%16)] = '.';

		if(i%16==15)
		{
			print_buf[15+3*16] = '|';
			print_buf[15+3*16+1+16] = '|';
		}
		
	}
	printf("%s",print_buf);
	printf("\n");
	
    return 0;

err:
	close (fd);
    return -1;
}

/*********************************************************************
*********************************************************************/
static int writemtd(const char * devname, char * data, unsigned int len)
{
    int dev_fd;
    int result;
   	erase_info_t erase;
    mtd_info_t mtd;
  
    if(devname == NULL || data == NULL || len == 0)
    {
        printf("%s(): Parameter Error!\r\n", __FUNCTION__);
        return -1;
    }

    /* open mtd device */
    dev_fd = open (devname, O_SYNC | O_RDWR);
    if(dev_fd < 0)
    {
        printf("%s(): Can't open %s! errno=%d\r\n", __FUNCTION__, devname, errno);
        return -1;
    }

    if ( len > g_mtd_info.size)
    {
        printf ("%s(): \"len\" too large!\r\n", __FUNCTION__);
        goto err;
    }

	if (lseek (dev_fd, offset, SEEK_SET) < 0)
    {
	    printf ("%s(): Error while seeking to start of %s\r\n", __FUNCTION__, devname);
        goto err;
    }

    /* write to device */
    result = write (dev_fd, data, len);
    if (result < 0)
    {
        printf ("%s(): writing data error! errno=%d\r\n", __FUNCTION__, errno);
        goto err;
    }

    if (len != (unsigned int)result)
    {

        printf ("%s(): Short write count returned while writing to flash.\r\n", __FUNCTION__);
        goto err;
    }

	close (dev_fd);
    return 0;

err:
    close(dev_fd);
    return -1;

}

static int erasemtd(const char * devname,unsigned int start, unsigned int len)
{
	int dev_fd;
   	erase_info_t erase;

    if( devname == NULL )
    {
        printf("%s(): Parameter Error!\r\n", __FUNCTION__);
        return -1;
    }

    /* open mtd device */
    dev_fd = open (devname, O_SYNC | O_RDWR);
    if(dev_fd < 0)
    {
        printf("%s(): Can't open %s! errno=%d\r\n", __FUNCTION__, devname, errno);
        return -1;
    }

    if(start % g_mtd_info.erasesize != 0)
    {
        printf ("%s(): MTD erase start addr err\r\n", __FUNCTION__);
        goto err;
    }
	else
	{
		erase.start = start;
	}

    if(len % g_mtd_info.erasesize != 0)
    {
    	erase.length = (len / g_mtd_info.erasesize + 1) * g_mtd_info.erasesize;    /* ±ß½ç¶ÔÆë */
    }
    else
    {
        erase.length = len;
    }
    
    if (ioctl (dev_fd, MEMERASE, &erase) < 0)
    {
        printf ("%s(): MTD erase error! errno=%d\r\n", __FUNCTION__, errno);
        goto err;
    }

	if (lseek (dev_fd, 0, SEEK_SET) < 0)
    {
	    printf ("%s(): Error while seeking to start of %s\r\n", __FUNCTION__, devname);
        goto err;
    }
   

	close (dev_fd);
    return 0;

err:
    close(dev_fd);
    return -1;
	
}

/*********************************************************************
*********************************************************************/
int cmd_mtd(int argc, char *argv[])
{
    int ret = 0;
    int data = 0;
	int len =0;
	char cmd_buf[128];
    char *p;
	char *name;
	char *buf;

	{
		if(strncmp(argv[1],"read",sizeof("read")) == 0)
		{
	//name
			name = argv[2];
			if( NULL == name)
			{
				goto disp_help;
			}
	//addr
			offset = simple_strtoul(argv[3],NULL,0);
	//len
			len = simple_strtoul(argv[4],NULL,0);

			ret = getmtd(name,&g_mtd_info);
			if(ret ==0)
			{
				buf = malloc(len);
				if(0 == readmtd(name, buf, len))
				{
					printf("read %s 0x%x-0x%x ok\n",name,offset,len);
				}
				free(buf);
			}
		}
		else if(strncmp(argv[1],"write",sizeof("write")) == 0)
		{
	//name
			name = argv[2];
			if( NULL == name)
			{
				goto disp_help;
			}
	//addr
			offset = simple_strtoul(argv[3],NULL,0);
	//len
			len = simple_strtoul(argv[4],NULL,0);
	//data
			data = simple_strtoul(argv[5],NULL,0);

			ret = getmtd(name,&g_mtd_info);
			if(ret ==0)
			{
				buf = malloc(len);
				memset(buf,data,len);
				if(0 == writemtd(name, buf, len))
				{
					printf("write %s 0x%x-0x%x with 0x%x ok\n",name,offset,len,data);
				}
				free(buf);
			}

		}
		else if(strncmp(argv[1],"erase",sizeof("erase")) == 0)
		{
	//name
			name = argv[2];
			if( NULL == name)
			{
				goto disp_help;
			}
	//offset and len
			p = argv[2];
			if(p == NULL)
			{
				offset = 0;
				len = g_mtd_info.size;
			}
			else
			{
				offset = simple_strtoul(p,NULL,0);
				len = simple_strtoul(argv[3],NULL,0);
			}
			
			ret = getmtd(name,&g_mtd_info);
			if(ret ==0)
			{
				if(0 == erasemtd(name, offset, len))
				{
					printf("erase %s 0x%x-0x%x ok\n",name,offset,len);
				}
			}
		}
		else if(strncmp(argv[1],"info",sizeof("info")) == 0)
		{
	//name
			name = argv[2];
			if( NULL == name)
			{
				goto disp_help;
			}
			ret = getmtd(name,&g_mtd_info);
			if(ret < 0)
				return ret;
			printf("========%s=========\n",name);
			printf("type      : %d\n",g_mtd_info.type);
			printf("size      : 0x%x[%d]\n",g_mtd_info.size,g_mtd_info.size);
			printf("erasesize : 0x%x[%d]\n",g_mtd_info.erasesize,g_mtd_info.erasesize);
			printf("writesize : 0x%x[%d]\n",g_mtd_info.writesize,g_mtd_info.writesize);
			printf("oobsize   : 0x%x[%d]\n",g_mtd_info.oobsize,g_mtd_info.oobsize);
		}
		else
		{
disp_help:
			printf("Use   : read   /dev/mtd? addr len\n"); 
			printf("Use   : write  /dev/mtd? addr len data\n"); 
			printf("Use   : erase  /dev/mtd? addr len\n"); 
			printf("Use   : info   /dev/mtd? \n"); 
		}
	}
    return 0;
	
}

ZTOOLS_CMD(mtd, cmd_mtd,"\tUse   : read  /dev/mtd? addr len\n" \
							    "\tUse   : write  /dev/mtd? addr len data\n" \
								"\tUse   : erase  /dev/mtd? addr len\n" \
								"\tUse   : info   /dev/mtd? \n");

								