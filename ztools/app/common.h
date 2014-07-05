#ifndef __COMMON_H__
#define __COMMON_H__

struct ztools_t
{
	char * name;
	int (*cmd_func) (int argc, char *argv[]);
	char * help;
};

#define Struct_Section  __attribute__ ((unused,section (".ztools_cmd")))

#define ZTOOLS_CMD(name,cmd,help) \
struct ztools_t __ztools_cmd_##name Struct_Section = {#name, cmd, help}

extern struct ztools_t  __ztools_cmd_start;
extern struct ztools_t  __ztools_cmd_end;

#endif
