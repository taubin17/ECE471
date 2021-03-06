#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "linux/gpio.h"
#include "sys/ioctl.h"


void toggle(struct gpiohandle_data data, struct gpiohandle_request req, int speed); 

void blink(struct gpiohandle_request req, int speed) {

	struct gpiohandle_data data;
	
	data.values[0] = 0;

	while (1) {

		toggle(data, req, speed);

	}
}

void toggle(struct gpiohandle_data data, struct gpiohandle_request req, int speed) {

	int rv;


	data.values[0] = 0;
	rv = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (rv < 0) printf("Error setting value. %s\n", strerror(errno));
	printf("Toggling off\n");

	sleep((double)(speed / 2));
	data.values[0] = 1;
	
	rv = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (rv < 0) printf("Error setting value. %s\n", strerror(errno));
	printf("Toggling On\n");
	
	sleep((double)(speed / 2));

	return;

}


void initialize() {

	int fd, rv;
	struct gpiochip_info chip_info;
	struct gpiohandle_request req;

	fd = open("/dev/gpiochip0", O_RDWR);

	if (fd < 0) printf("Error opening file, exiting. %s\n", strerror(errno));

	rv = ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &chip_info);
	if (rv < 0) printf("Error calling IOCTL. %s\n", strerror(errno));

	printf("Found %s, %s, %d lines\n", chip_info.name, chip_info.label, chip_info.lines);


	memset(&req, 0, sizeof(struct gpiohandle_request));

	req.flags = GPIOHANDLE_REQUEST_OUTPUT;
	req.lines = 1;
	req.lineoffsets[0] = 18;
	req.default_values[0] = 0;
	strcpy(req.consumer_label, "ECE471");

	rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (rv < 0) printf("Error calling IOCTL. %s\n", strerror(errno));

	blink(req, 1);
	
	return;
}


int main(int argc, char **argv) {

	initialize();
	
	return 0;
}
