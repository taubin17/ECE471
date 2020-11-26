#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

// Lookup table for 7Seg characters 
char lookup(char character) {
	
	switch (character){
		case 'A':
		case 'a':
			return 0x77;
		
		case 'B':
		case 'b':
			return 0x5F;
		
		case 'C':
		case 'c':
			return 0x39;
		
		case 'D':
		case 'd':
			return 0x1F;

		case 'E':
		case 'e':
			return 0x79;

		case 'F':
		case 'f':
			return 0x71;

		case 'G':
		case 'g':
			return 0x5F;

		case 'H':
		case 'h':
			return 0x75;

		case 'I':
		case 'i':
			return 0x30;

		case 'J':
		case 'j':
			return 0x3F;

		case 'K':
		case 'k':
			return 0x75;

		case 'L':
		case 'l':
			return 0x38;

		case 'N':
		case 'n':
			return 0x54;

		case 'O':
		case 'o':
			return 0x3F;

		case 'P':
		case 'p':
			return 0x73;
		
		case 'Q':
		case 'q':
			return 0x67;

		case 'R':
		case 'r':
			return 0x57;

		case 'S':
		case 's':
			return 0x6D;

		case 'U':
		case 'u':
			return 0x3E;

		case 'Y':
		case 'y':
			return 0x6E;

		case 'Z':
		case 'z':
			return 0x5B;



		case '0':
			return 0x3F;

		case '1':
			return 0x06;

		case '2':
			return 0x5B;

		case '3':
			return 0x4F;

		case '4':
			return 0x66;

		case '5':
			return 0x6D;

		case '6':
			return 0x7D;

		case '7':
			return 0x07;

		case '8':
			return 0x7F;

		case '9':
			return 0x6F;
		case ' ':
			return 0x00;
		
		default:
			printf("Error, letter %c is non displayable!\n", character);
			exit(-1);	
	}
}

		
void display_message(char * message, int time_to_display, int fd) {
	
	char buffer[17];
	int buffer_indexes[4] = {1, 3, 7, 9};
	int limiter = 0;
	

	// Condition checking if string has been fully scrolled
	if (strlen(message) == 0) {
		return;
	}
	
	// Keep address 0 always 0
	buffer[0] = 0x0;
	buffer[2] = 0x0;
	buffer[4] = 0x0;
	buffer[5] = 0x0;
	buffer[6] = 0x0;
	buffer[8] = 0x0;
	buffer[10] = 0x0;


	// If there are more characters than 7Segs, limit it to 4 letters. Otherwise just do string size
	if (strlen(message) > 4) {
		limiter = 4;
	}
	else {
		limiter = strlen(message);
	}

	for (int i = 0; i < limiter; i++) {
		buffer[buffer_indexes[i]] = lookup(message[i]);	
	}

	for (int j = strlen(message); j < 4; j++) {
		
		buffer[buffer_indexes[j]] = 0x00;

	}
	// Write user message to the display
	write(fd, buffer, 17);
	
	// Keep message on for user specified time before scrolling
	usleep((time_to_display * 1000000));
	
	// Fill buffer with zeroes and clear display
	for (int k = 0; k < 16; k++) {
		buffer[1+k] = 0x00;
	}
	
	
	write(fd, buffer, 17);

	//Remove first letter
	message++;

	// Call it with message incremented a letter (take off first letter)
	display_message(message, time_to_display, fd);
	
	return;
	


}


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


	// Finally display our message with scrolling	
	display_message("ECE 471 is a cool class", 1, fd);


	/* Close device */

	close(fd);
	printf("Device Closed!\n");
	return 0;
}
