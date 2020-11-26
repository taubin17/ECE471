#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>


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
			return 0x6C;

		case 'U':
		case 'u':
			return 0x3E;

		case 'Y':
		case 'y':
			return 0x6E;

		case 'Z':
		case 'z':
			return 0x5B;

		case '*':
			return 0x63;


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
		
		default:
			printf("Error, letter %c is non displayable!\n", character);
			exit(-1);	
	}
}

		
void write_display(int fd, double value) {


	// Perform integer conversion on value, and multiply by 10
	int converted_val = (int)value * 10;

	int is_decimal;

	if (converted_val > 1000){

		is_decimal = 0;

	}
	else {
	
		is_decimal = 1;

	}

	char message[3];

	for (int i = 0; i < 2; i++) {

		message[i] = (converted_val % ((i + 1) * 10));

	}
	
	// Create a new buffer
	char *buffer = malloc(17 * (sizeof(char)));
	
	// Clear output said buffer	
	memset(buffer, 0, sizeof(char) * 17);

	// Set the address to display data address
	buffer[0] = 0x0;
	
	// Begin writing our message ([1] represents leftmost digit, [3] represents second leftmost digit, etc.)
	buffer[1] = lookup(message[2]);
	buffer[3] = lookup(message[1]);
	buffer[7] = lookup(message[0]);
	
	// If number is decimal, include decimal point. Otherwise, get rid of it
	if (is_decimal == 1) {

		buffer[9] = lookup('*');

	}
	else {

		buffer[9] = 0x0;

	}
	// Write the message to the display
	write(fd, buffer, 17);

	// Keep the message on for however many seconds the user specifies
	sleep(1);

	// Now clear the buffer 
	for (int k = 0; k < 16; k++) {
		buffer[1+k] = 0x00;
	}
	
	// And remove the message from the screen
	write(fd, buffer, 17);

	// Shutdown the display
	shutdown_display(fd);

	return;
	
}


int shutdown_display(int fd) {

	// Close file descriptor
	int success = close(fd);

	// If close occurs error, report and close
	if (success == -1) {
		printf("Error closing file!\n");
		return -1;
	}

	return 0;

}



int init_display(void) {

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


	return fd;


}

