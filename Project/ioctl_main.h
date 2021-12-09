#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

#define MAJOR_NUM 239

#define TEMP	0
#define CONFG	1
#define THYST	2
#define TOS	3
#define SHUTDOWN	4
#define RESUME	5



#define LM_TEMP_READ _IOR(MAJOR_NUM,TEMP,char *)
#define LM_CONFG_READ _IOR(MAJOR_NUM,CONFG,char *)
#define LM_THYST_READ _IOR(MAJOR_NUM,THYST,char *)
#define LM_TOS_READ _IOR(MAJOR_NUM,TOS,char *)
#define LM_SHUTDOWN _IO(MAJOR_NUM,SHUTDOWN)
#define LM_RESUME _IO(MAJOR_NUM,RESUME)


#define DEVICE_FILE_NAME "/dev/LM75"

#endif