# 变量————————————————————
# 二进制文件
TARGETS = sender receiver

# 伪目标————————————————————
# 生成所有
.PHONY : all
all : $(TARGETS) 

#规则————————————————————
$(TARGETS) : % : %.c
	gcc -o $@ $<
# 相当于：
# gcc -o sender sender.c
# gcc -o receiver receiver.c

# 伪目标————————————————————
# 清除所有
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(TARGET);)
# 相当于：
# rm sender
# rm receiver