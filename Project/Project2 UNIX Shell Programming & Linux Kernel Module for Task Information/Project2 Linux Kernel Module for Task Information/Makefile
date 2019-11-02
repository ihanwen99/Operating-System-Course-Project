obj-m:=pid.o  
pidmodule-objs:=module  
KDIR:=/lib/modules/$(shell uname -r)/build  
MAKE:=make  
default:  
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules  
clean:  
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean  