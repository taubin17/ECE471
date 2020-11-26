#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int fd;
	int result;

	char i2c_device[]="/dev/i2c-1";

	unsigned char buffer[17];

	/* Open i2c device */

	fd = open(i2c_device, O_RDWR);
	if (fd < 0) printf("Error opening device\n");
	
	/* Set slave address */

	result = ioctl(fd, I2C_SLAVE, 0x70);
	if (result < 0) printf("Error opening device\n");
	printf("Slave address set!\n");
	/* Turn on oscillator */

	buffer[0] = (0x2<<4) | (0x1);
	result = write(fd, buffer, 1);
	printf("Oscillator on!\n");

	/* Turn on Display, No Blink */

	buffer[0] = (0x8 << 4) | (0x1);
	result = write(fd, buffer, 1);
	printf("Display On!\n");

	/* Set Brightness */
	
	buffer[0] = (0xE << 4) | (0xE);
	result = write(fd, buffer, 1);
	printf("Display dimmed!\n");

	/* Write 1s to all Display Columns */

	buffer[0] = 0x0;
	for(int i = 0; i < 16; i++) {
		buffer[1+i] = 0xFF;
	}
	write(fd, buffer, 17);
	printf("Display Filled!\n");

	/* Close device */

	close(fd);
	printf("Device Closed!\n");
	return 0;
}
