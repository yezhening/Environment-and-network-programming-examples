#变量
targets = server client

#伪目标
.PHONY : all
all : $(targets) 

#规则
server : server.c
	gcc -o server server.c 

client : client.c
	gcc -o client client.c

#伪目标
.PHONY : clean
clean :
	rm $(targets)