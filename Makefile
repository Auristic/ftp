FLAG=-Wall -Werror
STD=-std=gnu99
CC=gcc

all:
	$(CC) $(STD) $(FLAG) client.c tools.c clifun.c -o client -lpthread
	$(CC) $(STD) $(FLAG) server.c tools.c serfun.c -o server -lpthread

clean:
	rm client server
	
	
	
	
	
	
	
	
	
		
	
