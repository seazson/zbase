#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

char console_buffer[256];
char *cmd_argv[32];
int  cmd_argc;

int version()
{
	int i;
	printf("%s %s\n",__DATE__,__TIME__);
	for(i=0;i<cmd_argc; i++)
	{
		printf("%d : %s\n",i,cmd_argv[i]);	
	}
	return 0;
}

int show_menu()
{
	struct ztools_t *cmdtp;
    printf("********************************************\n");
    printf("*                 UNIX STD                 *\n");
    printf("********************************************\n");
    for (cmdtp = &__ztools_cmd_start; cmdtp != &__ztools_cmd_end; cmdtp++) {
		printf("%s----------------\n%s\n",cmdtp->name, cmdtp->help);
	}
	return 0;
}

int cmd_help(int cmd_argc, char *cmd_argv[])
{
	return show_menu();
}
ZTOOLS_CMD(help, cmd_help,"\tUse   : cmd help");


int main(int argc, char * argv[])
{
	int len;
	int list;
	char *p=" ";
	struct ztools_t *cmdtp;
	
	show_menu();
	while(1)	
	{
		printf("============================================\n");
		printf("Enter your cmd : ");	
		len = get_cmdline(console_buffer);
		if(len==-1)
			return 0;
		else if(len==0)
			continue;
		
		printf("You enter %d words : %s\n",len, console_buffer);
		printf("----------------------------------------->>>\n");
		cmd_argv[0] = strtok(console_buffer,p);
		cmd_argc=0;
		while(cmd_argv[(cmd_argc++)+1]=strtok(NULL," "));
		
		if(strcmp(cmd_argv[0],"quit") == 0)
		{
			return 0;
		}
		
	    for (cmdtp = &__ztools_cmd_start; cmdtp != &__ztools_cmd_end; cmdtp++) 
		{
			if(strcmp(cmd_argv[0], cmdtp->name) == 0)
			{
				cmdtp->cmd_func(cmd_argc,cmd_argv);
			}	
		}
	}
	return 0;
}
