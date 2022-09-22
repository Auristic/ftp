FLAG=-Wall -Werror #编译后显示所有警告，将所有的警告当成错误进行处理
STD=-std=gnu99
CC=gcc

all:
	$(CC) $(STD) $(FLAG) client.c tools.c clifun.c -o client -lpthread
	$(CC) $(STD) $(FLAG) server.c tools.c serfun.c -o server -lpthread

clean:
	rm client server
	
	
	
	
	
	
	
	
	
		
	
