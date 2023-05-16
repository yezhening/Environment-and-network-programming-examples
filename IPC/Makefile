# 变量————————————————————
# 内层Makefile文件的路径
INNER_MAKEFILES = glov_h/Makefile glov_s/Makefile file_pv/Makefile\
pipe/Makefile fifo/Makefile\
sig/Makefile sem_XSI/Makefile sem_POSIX/Makefile\
mq/Makefile shm/Makefile\
sock/Makefile\
mmap/Makefile

# 伪目标
# 生成所有
.PHONY : all
all : 
	$(foreach INNER_MAKEFILE,$(INNER_MAKEFILES),$(MAKE) -C $(dir $(INNER_MAKEFILE)) -f $(notdir $(INNER_MAKEFILE)) all;)
	
# 清除所有
clean:
	$(foreach INNER_MAKEFILE,$(INNER_MAKEFILES),$(MAKE) -C $(dir $(INNER_MAKEFILE)) -f $(notdir $(INNER_MAKEFILE)) clean;)