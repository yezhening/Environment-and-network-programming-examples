# 变量————————————————————
# 二进制文件
TARGETS = glov


# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : %.c
	gcc -o $@ $<
# 相当于：
# gcc -o glov glov.c 

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(TARGET);)
# 相当于：
# rm glov