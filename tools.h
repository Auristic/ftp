/*工具函数的声明*/

#ifndef TOOL_H

#define TOOL_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

//修改终端的控制方式，1.取消回显、确认　２.获取数据　3.还原
int getch(void);
//清除输入缓存
void clear_stdin(void);
//获取字符串
char* get_str(char* str, size_t len);
//返回性别, w女m男
char get_sex(void);
//返回介于start和end之间的合法字符
char get_cmd(char start, char end);
//返回处理后的密码
char* get_pw(char* passwd, bool is_show, size_t size);
// 获取时间，传入type来获取各种时间
char* get_time(int type);

#endif//TOOL_h
