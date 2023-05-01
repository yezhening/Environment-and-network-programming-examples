#变量
TARGETS = server client_select client_poll client_epoll
BIN_DIR = ../bin

#伪目标
.PHONY : all
all : $(TARGETS) 

#规则
$(TARGETS) : % : %.c
	gcc -o $(BIN_DIR)/$@ $< $(if $(findstring client_epoll,$@),-std=c99)

#伪目标
.PHONY : clean
clean :
	$(foreach TARGET,$(TARGETS),rm $(BIN_DIR)/$(TARGET);)