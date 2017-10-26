#ifndef __ZTOOLS_H__
#define __ZTOOLS_H__

#define CMD_MEM_WRITE    0x81000001   
#define CMD_MEM_READ     0x81000002

#define ZTOOLS_MAJOR  117

typedef struct zdata_struct
{
	unsigned int addr;
	unsigned int len;
	union data_u
	{
		unsigned int reg32;
		unsigned char *p;
	}data;
} zdata_t;

#define z_read8(addr)           (*(volatile unsigned char *)(addr))
#define z_write8(addr,value)   (*(volatile unsigned char *)(addr) = value)
#define z_read16(addr)           (*(volatile unsigned short *)(addr))   
#define z_write16(addr,value)   (*(volatile unsigned short *)(addr) = value)
#define z_read32(addr)           (*(volatile unsigned int *)(addr)) 
#define z_write32(addr,value)   (*(volatile unsigned int *)(addr) = value)

#ifdef CONFIG_PROC
int ztools_proc_init(void);
void ztools_proc_free(void);
#endif

struct ztools_cmd_t
{
	char *name;
	char *usage;
	void (*func)(int argc, char *argv[]);
};

/*	should add section to ld
	. = ALIGN(8);
	.ztools.data : {	
		__ztools_start = .; 
		*(_ztools_cmd)				
		__ztools_end = .;
	}
	
*/

#define DEFINE_ZTOOLS_CMD(function, msg)			\
									\
struct ztools_cmd_t ztools_##function = {			\
			.name = #function,			\
			.usage = msg,		\
			.func = function,			\
};									\
struct ztools_cmd_t 		\
__attribute__((section("_ztools_cmd"))) *__ztools_##function = &ztools_##function

#define ZTOOLS_CMD(function)  &ztools_##function	
#define EXTERN_ZTOOLS_CMD(function) extern struct ztools_cmd_t ztools_##function
	
#endif