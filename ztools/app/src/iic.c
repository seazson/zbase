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

#define I2C_SLAVE	      0x0703	/* Use this slave address */
#define I2C_TENBIT	      0x0704	

static int dev_addr    = 0;
static int reg_addr    = 0;
static int data_count  = 0;

/*********************************************************************
º¯ Êi Ãû£ºiic_read
¹¦ÄÜÃèÊö£º
ÊäÈë²ÎÊi£ºÎS
Êä³ö²ÎÊi£ºÎS
·µ »Ø Öµ£º´íÎóÀàGÍ
*********************************************************************/
static int iic_read(unsigned int count)
{
    char buf[128];
    int tmp = -1;
	int i2cdevfd;
    /*»ñÈ¡i2cdevfd*/

	if ((i2cdevfd = open("/dev/i2c-0", O_RDWR)) < 0) 
    {
         printf("open failed \n");     
 	     return   -1;
    }   
  
    if (ioctl(i2cdevfd,I2C_TENBIT,0)< 0) 
    {
    	 return   -1;
    }

    if (ioctl(i2cdevfd, I2C_SLAVE, dev_addr) < 0) 
    {
         printf("i2c_read set DEV_ADDR %d error!\n ",dev_addr);
	  return   -1;
    }

	for(tmp=0;tmp<count;tmp++,reg_addr++)
	{
	    if (write(i2cdevfd,&(reg_addr),1) != 1)                  
	    {
	         printf("i2c write register addr %x error!\r\n",reg_addr);
	         return -1;
	    }
	    if (read(i2cdevfd,buf,1) != 1) 
	    {
	    	 return   -1;
	    }

		printf("0x%x : %d [0x%x]\n",reg_addr,buf[0],buf[0]);
	}
	close(i2cdevfd);
            
    return   0;	
}

/*********************************************************************
º¯ Êi Ãû£ºiic_write
¹¦ÄÜÃèÊö£º
ÊäÈë²ÎÊi£ºÎS
Êä³ö²ÎÊi£ºÎS
·µ »Ø Öµ£º´íÎóÀàGÍ
*********************************************************************/
static int iic_write(unsigned int data)
{
    char buf[128];
    int tmp = -1;
	int i2cdevfd;
    /*»ñÈ¡i2cdevfd*/

	if ((i2cdevfd = open("/dev/i2c-0", O_RDWR)) < 0) 
    {
         printf("open failed \n");     
 	     return   -1;
    }   
  
    if (ioctl(i2cdevfd,I2C_TENBIT,0)< 0) 
    {
    	 return   -1;
    }

    if (ioctl(i2cdevfd, I2C_SLAVE, dev_addr) < 0) 
    {
         printf("i2c_read set DEV_ADDR %d error!\n ",dev_addr);
	  return   -1;
    }

    if (write(i2cdevfd,&(reg_addr),1) != 1)                  
    {
         printf("i2c write register addr %x error!\r\n",reg_addr);
         return -1;
    }
    if (write(i2cdevfd,&(data),1) != 1) 
    {
         printf("i2c write register data %x error!\r\n",data);
    	 return -1;
    }

	printf("0x%x : %d [0x%x]\n",reg_addr,data,data);

	close(i2cdevfd);
            
    return   0;	
}

/*********************************************************************
º¯ Êi Ãû£ºtest_monitor
¹¦ÄÜÃèÊö£º
ÊäÈë²ÎÊi£ºÎS
Êä³ö²ÎÊi£ºÎS
·µ »Ø Öµ£º´íÎóÀàGÍ
*********************************************************************/
int cmd_iic(int argc, char *argv[])
{
    int ret = 0;
    int data = 0;
	char cmd_buf[128];
    char *p;

	{
		if(strncmp(argv[1],"read",sizeof("read")) == 0)
		{
	//dev addr
			if( NULL == argv[2])
			{
				goto disp_help;
			}
			dev_addr = simple_strtoul(argv[2],NULL,0);
	//reg addr
			if( NULL == argv[3])
			{
				goto disp_help;
			}
			reg_addr = simple_strtoul(argv[3],NULL,0);
	//count
			if( NULL == argv[4])
			{
				goto disp_help;
			}
			data_count = simple_strtoul(argv[4],NULL,0);

			printf("read 0x%x 0x%x 0x%x\n",dev_addr,reg_addr,data_count);

			iic_read(data_count);

		}
		else if(strncmp(argv[1],"write",sizeof("write")) == 0)
		{
	//dev addr
			if( NULL == argv[2])
			{
				goto disp_help;
			}
			dev_addr = simple_strtoul(argv[2],NULL,16);
	//reg addr
			if( NULL == argv[3])
			{
				goto disp_help;
			}
			reg_addr = simple_strtoul(argv[3],NULL,16);
	//count
			if( NULL == argv[4])
			{
				goto disp_help;
			}
			data_count = simple_strtoul(argv[4],NULL,16);
			printf("write 0x%x 0x%x with 0x%x\n",dev_addr,reg_addr,data_count);
			iic_write(data_count);

		}
		else
		{
disp_help:
			printf("Use   : read  device_addr reg_addr len\n"); 
			printf("Use   : write device_addr reg_addr data\n"); 
		}
	}
    return 0;
	
}

ZTOOLS_CMD(iic, cmd_iic,"\tUse   : read  device_addr reg_addr len\n" \
						"\tUse   : write device_addr reg_addr data\n");

