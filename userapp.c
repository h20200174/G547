#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>		
#include<fcntl.h>              /* open */
#include<unistd.h>             /* exit */
#include<sys/ioctl.h> 	       /* ioctl */
#include "ioctl_main.h"        /* ioctl functionality*/

uint8_t data;


int lm_temp_read(int file_desc)		//reading temperature data
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_TEMP_READ, &data);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
    	printf("Temperature : %lu\n",data);
	return 0;
}
int lm_confg_read(int file_desc)	//reading configuration register
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_CONFG_READ, &data);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
    	printf("Configuration : %lu\n",data);
	return 0;
}

int lm_thyst_read(int file_desc)	//reading hystersis register
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_THYST_READ, &data);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
    	printf("Hystersis temperature : %lu\n",data);
	return 0;
}

int lm_tos_read(int file_desc)		//reading overtemperature shutdown register
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_TOS_READ, &data);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
    	printf("OverTemperature : %lu\n",data);
	return 0;
}

int lm_shutdown(int file_desc)		//Putting the sensor in shutdown mode
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_SHUTDOWN);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
	printf("Shutdown implemented\n");
	return 0;
}

int lm_resume(int file_desc)		//Resuming sensor's operation
{
	unsigned int ret_val;
	ret_val = ioctl(file_desc, LM_RESUME);
	if (ret_val < 0) 
	{
		printf("ioctl_get_msg failed:%d\n", ret_val);
		exit(-1);
        }
	printf("Operation resumed\n");
	return 0;
}

int main()
{
	int file_desc;
	int choice;
	file_desc = open(DEVICE_FILE_NAME, 0);	//Opening File
	if (file_desc < 0) 
	{
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}
	printf("1. Current Temperature\n");
	printf("2. Configuration\n");
	printf("3. Hystersis Temperature\n");
	printf("4. OverTemperature\n");
	printf("5. Shutdown Mode\n");
	printf("6. Normal Mode\n");
	printf("Enter any value between 1-6 : ");
	scanf("%d",&choice);

	switch (choice)		//choosing which functionality to display
	{
	case 1 :
		lm_temp_read(file_desc);
		break;
	case 2 :
		lm_confg_read(file_desc);
		break;
	case 3 :
		lm_thyst_read(file_desc);
		break;
	case 4 :
		lm_tos_read(file_desc);
		break;			
	case 5 :
		lm_shutdown(file_desc);
		break;
	case 6 :
		lm_resume(file_desc);
		break;
	default:
		break;
	}
	close(file_desc);	//Closing file
	return 0;
}
