#include "imu_header.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              /* open */
#include <unistd.h>             /* exit */
#include <sys/ioctl.h>          /* ioctl */


/* Wrapper function declaration for the Accelerometer values */
int ioctl_get_accx(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_ACCx,message);
    if (ret_val < 0) {
        printf("ioctl_get_accx failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Acc_x:%s\n", message);
    return 0;
}

int ioctl_get_accy(int file_desc)
{
    int ret_val;
    char message[64];


    ret_val = ioctl(file_desc,IOCTL_GET_ACCy,message);

    if (ret_val < 0) {
        printf("ioctl_get_accy failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Acc_y:%s\n", message);
    return 0;
}

int ioctl_get_accz(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_ACCz,message);
    if (ret_val < 0) {
        printf("ioctl_get_accz failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Acc_z:%s\n", message);
    return 0;
}

/* Wrapper function definition for the Gyroscope values */
int ioctl_get_gyrox(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_GYROx,message);
    if (ret_val < 0) {
        printf("ioctl_get_gyrox failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Gyro_x:%s\n", message);
    return 0;
}

int ioctl_get_gyroy(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_GYROy,message);
    if (ret_val < 0) {
        printf("ioctl_get_gyroy failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Gyro_y:%s\n", message);
    return 0;
}

int ioctl_get_gyroz(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_GYROz,message);
    if (ret_val < 0) {
        printf("ioctl_get_gyroz failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Gyro_z:%s\n", message);
    return 0;
}

/* Wrapper function definition for the Magnetometer values */
int ioctl_get_magx(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_MAGx,message);
    if (ret_val < 0) {
        printf("ioctl_get_magx failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Mag_x:%s\n", message);
    return 0;
}

int ioctl_get_magy(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_MAGy,message);
    if (ret_val < 0) {
        printf("ioctl_get_magy failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Mag_y:%s\n", message);
    return 0;
}

int ioctl_get_magz(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_MAGz,message);
    if (ret_val < 0) {
        printf("ioctl_get_magz failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Mag_z:%s\n", message);
    return 0;
}

/* Wrapper function definition for the Pressure value */
int ioctl_get_press(int file_desc)
{
    int ret_val;
    char message[8];

    ret_val = ioctl(file_desc,IOCTL_GET_PRESS,message);
    if (ret_val < 0) {
        printf("ioctl_get_press failed : %d\n", ret_val);
        exit(-1);
    }

    printf("Press:%s\n", message);
    return 0;
}

/*
-----------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------
*/

/*
User function definition for IMU with 10 DOF
MPU 9255 Output :- (9 DOF)

Accelerometer(Ax,Ay,Az) : Each value is of 16 bits
Gyroscope(Gx,Gy,Gz) : Each value is of 16 bits
Magnetometer(Hx,Hy,Hz) : Each value is of 16 bits

BMP 280 Output :- (1 DOF)
Pressure(P) : 20 bits
*/

int main()
{
    int file_desc, ret_val;
    char *msg = "Message passed by ioctl\n";

    file_desc = open(DEVICE_FILE_NAME, 0);
    if (file_desc < 0) {
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

    ioctl_get_accx(file_desc);
    ioctl_get_accy(file_desc);
    ioctl_get_accz(file_desc);

    ioctl_get_gyrox(file_desc);
    ioctl_get_gyroy(file_desc);
    ioctl_get_gyroz(file_desc);

    ioctl_get_magx(file_desc);
    ioctl_get_magy(file_desc);
    ioctl_get_magz(file_desc);

    ioctl_get_press(file_desc);

    close(file_desc);
    return 0;
}
