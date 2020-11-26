#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

#include "read_temp.h"
#include "write_display.h"


int main(int argc, char **argv){

	int fd;
	double temp;

	// Must read temperature 1 time per second
	while(1) {
		fd = init_display();
		temp = read_temp();
		write_display(fd, temp);
	}

	return 0;


}

