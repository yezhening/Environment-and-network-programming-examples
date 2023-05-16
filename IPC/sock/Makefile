# 变量————————————————————
# 二进制文件
TARGETS = server client

# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : %.c
	gcc -o $@ $<
# 相当于：
# gcc -o server server.c
# gcc -o client client.c

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(TARGET);)
# 相当于：
# rm server
# rm client