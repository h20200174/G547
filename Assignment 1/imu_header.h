#ifndef IMUDEV_H
#define IMUDEV_H

#include <linux/ioctl.h>

#define MAJOR_NUM 240

/*
 * Set the message of the device driver
 */
/*
 * _IOR means that we're creating an ioctl command
 * number for passing information from a user process
 * to the kernel module.
 *
 * The first arguments, MAJOR_NUM, is the major device
 * number we're using.
 *
 * The second argument is the number of the command
 * (there could be several with different meanings).
 *
 * The third argument is the type we want to get from
 * the process to the kernel.
 */


#define IOCTL_GET_ACCx _IOR(MAJOR_NUM, 0, char *)       /*IOCTL calls for reading the accelerometer values in all three directions */
#define IOCTL_GET_ACCy _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_GET_ACCz _IOR(MAJOR_NUM, 2, char *)

#define IOCTL_GET_GYROx _IOR(MAJOR_NUM, 3, char *)      /*IOCTL calls for reading the gyroscope values in all three directions */
#define IOCTL_GET_GYROy _IOR(MAJOR_NUM, 4, char *)
#define IOCTL_GET_GYROz _IOR(MAJOR_NUM, 5, char *)

#define IOCTL_GET_MAGx _IOR(MAJOR_NUM, 6, char *)       /*IOCTL calls for reading the magnetometer values in all three directions */
#define IOCTL_GET_MAGy _IOR(MAJOR_NUM, 7, char *)
#define IOCTL_GET_MAGz _IOR(MAJOR_NUM, 8, char *)

#define IOCTL_GET_PRESS _IOR(MAJOR_NUM, 9, char *)      /*IOCTL calls for reading the pressure value */



/*
 * The name of the device file
 */
#define DEVICE_FILE_NAME "/dev/imu_char"

#endif
