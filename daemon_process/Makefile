# 变量————————————————————
# 二进制文件
TARGETS = server client_select client_poll client_epoll	
# 二进制文件目录
BIN_DIR = ./bin
# 目标文件目录
OBJ_DIR = ./obj
# 源文件目录
SRC_DIR = ./src

# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : $(OBJ_DIR)/%.o
	gcc -o $(BIN_DIR)/$@ $<
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	gcc -c -o $@ $< $(if $(or $(findstring server,$@),$(findstring client_epoll,$@)),-std=c99)
# 相当于：
# gcc -c -o obj/server.o src/server.c -std=c99
# gcc -o ./bin/server obj/server.o
# gcc -c -o obj/client_select.o src/client_select.c 
# gcc -o ./bin/client_select obj/client_select.o
# gcc -c -o obj/client_poll.o src/client_poll.c 
# gcc -o ./bin/client_poll obj/client_poll.o
# gcc -c -o obj/client_epoll.o src/client_epoll.c -std=c99
# gcc -o ./bin/client_epoll obj/client_epoll.o

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach OBJ_FILE,$(wildcard $(OBJ_DIR)/*.o),rm $(OBJ_FILE);)
	$(foreach TARGET,$(TARGETS),rm $(BIN_DIR)/$(TARGET);)
# 相当于：
# rm ./obj/client_poll.o; rm ./obj/server.o; rm ./obj/client_epoll.o; rm ./obj/client_select.o;
# rm ./bin/server; rm ./bin/client_select; rm ./bin/client_poll; rm ./bin/client_epoll;