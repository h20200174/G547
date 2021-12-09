obj-m += temp.o
 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o user userapp.c
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
