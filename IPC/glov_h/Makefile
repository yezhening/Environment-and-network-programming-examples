# 变量————————————————————
# 二进制文件
TARGETS = process1 process2


# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : %.c glov.c
	gcc -o $@ $^
# 相当于：
# gcc -o process1 process1.c glov.c
# gcc -o process2 process2.c glov.c

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(TARGET);)
# 相当于：
# rm process1
# rm process2