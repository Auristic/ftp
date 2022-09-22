/*一些常用的工具函数*/


#include "tools.h"
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

//修改终端的控制方式,干三件事,1. 取消回显、确认　２.获取数据　3.还原
int getch(void)
{
    //用于记录终端的配置信息的结构体
    struct termios old;
    //获取终端的配置信息
    tcgetattr(STDIN_FILENO, &old);
    //设置新的终端配置   
    struct termios new = old;
    //取消确认、回显
    new.c_lflag &= ~(ICANON|ECHO);//ICANON使用标准输入模式  ECHO显示输入字符  “~”表示二进制按位取反
    //设置终端配置信息
    tcsetattr(STDIN_FILENO,TCSANOW,&new);//TCSANOW表示不等数据传输完成就立即改变属性

    //在新模式下获取数据
    int key_val = 0; 
    do{
    	key_val += getchar();
    }while(stdin->_IO_read_end - stdin->_IO_read_ptr);

    // 还原配置信息
    tcsetattr(STDIN_FILENO,TCSANOW,&old); 
    return key_val; 
}

void clear_stdin(void)
{
	//清空输入缓冲区
	stdin->_IO_read_ptr = stdin->_IO_read_end;
}

char* get_str(char* str, size_t len)
{
	if(NULL == str)
	{
		puts("空指针异常~\n");
		return NULL;
	}
	
	char *in = fgets(str, len, stdin);
	if(NULL == in)
		return str;
	
	//使用fgets输入时,超过size-1个字符会残留在输入缓冲区中,影响下一次的输入,因此需要处理
	size_t cnt = strlen(str);
	if('\n' == str[cnt-1])
	{
		str[cnt-1] = '\0';
	}
	clear_stdin();//清缓存
	return str;
}

//用来限制输入,智能输入start到end之间的字符
char get_cmd(char start, char end)
{
	clear_stdin();

	printf("请输入指令:");
	while(1)
	{
		char val = getch();
		if(val >= start && val <= end)
		{
			printf("%c\n", val);
			return val;
		}
	}
}

// 获取时间,传入type来获取各种格式的时间
char* get_time(int type)
{
	time_t timep;
	static struct tm *tm_p = NULL;
	time(&timep);
	tm_p = localtime(&timep);
	
	static char now_time[20] = {};

	if(0 == type){
		snprintf(now_time, 2, " ");
	}else if(1 == type){
		snprintf(now_time, 20, "%04d-%02d-%02d", (1900 + tm_p->tm_year), (1 + tm_p->tm_mon), tm_p->tm_mday);
	}else if(2 == type){
		snprintf(now_time, 20, "%04d-%02d-%02d %02d:%02d:%02d",(1900 + tm_p->tm_year), (1 + tm_p->tm_mon), tm_p->tm_mday, 
			tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);
	}
	else if(3 == type){
		snprintf(now_time, 20, "%04d-%02d-%02d_%02d:%02d:%02d", (1900 + tm_p->tm_year), (1 + tm_p->tm_mon), 
			tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);
	}else{}
	
	return now_time;
}