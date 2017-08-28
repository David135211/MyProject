#include <stdlib.h>
#include <stdio.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <fcntl.h>
     
main(void)
{
	int timeout;
	int count = 0;
	int res;
	int fd=open("/dev/watchdog", O_RDWR);
	if(fd<0){
		printf("watchdog not found\n");
		exit(-1);
	}
	res = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	if(res<0){
		printf("watchdog not support WDIOC_GETTIMEOUT\n");
		exit(-1);
	}
	printf("watchdog timeout = %d(s)\n",timeout);
	res = ioctl(fd, WDIOC_KEEPALIVE, NULL);
	if(res<0){
		printf("watchdog not support WDIOC_KEEPALIVE\n");
		exit(-1);
	}
	timeout = 30;
	printf("set watchdog timeout = %d\n",timeout);
	ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	if(res<0){
		printf("watchdog not support WDIOC_SETTIMEOUT\n");
		exit(-1);
	}
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	printf("Now watchdog timeout = %d(s)\n",timeout);
	while(1){
		printf("wait watchdog timout %d(s)\n",count);
		usleep(1000000);
		count++;
	}	
}
