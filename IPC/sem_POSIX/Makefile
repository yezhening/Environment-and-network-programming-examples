# 变量————————————————————
# 二进制文件
TARGETS = process1 process2

# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : %.c
	gcc -o $@ $< -pthread
# 相当于：
# gcc -o process1 process1.c -pthread
# gcc -o process2 process2.c -pthread

# 不使用“-pthread”选项会报错：
# /tmp/ccWVDRPj.o: In function `main':
# process1.c:(.text+0x30): undefined reference to `sem_open'
# ...

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(TARGET);)
# 相当于：
# rm process1
# rm process2