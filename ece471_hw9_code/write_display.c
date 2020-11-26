#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>


int shutdown_display(int fd);

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

		case '-':
			return 0x40;
		
		default:
			printf("Error, letter %c is non displayable!\n", character);
			exit(-1);	
	}
}

		
void write_display(int fd, double value) {


	// Perform integer conversion on value, and multiply by 10
	int converted_val = (int)value;
		
	// If the conversion result is negative (if value was negative), multiply by -1 to make positive
	if (converted_val < 0) {
		converted_val *= -1;
		printf("%d\n", converted_val);
	}
	
	// Initialize message to display nothing
	char message[4] = {0x0, 0x0, 0x0, 0x0};
	
	char ones;
	char tens;
	char hundreds;
	char tenths;



	// Case 1, temperature between 0 and 99.9*
	if (value <= 99.9 && value >= 0) {

		// Covert value to string representing two digits
		ones = '0' + converted_val % 10;
		tens = '0' + converted_val / 10;

		// Determine tenths value for potential use
		tenths = '0' + (int)(value * 10) % 10;

		// If tens place is empty, use tenths place with decimal point
		if ((converted_val / 10) == 0) {
			message[0] = 0x0;
		}
		else {
			message[0] = lookup(tens);
		}

		// Save digits to message to LED display
		message[1] = lookup(ones) + 0x80;
		message[2] = lookup(tenths);
		message[3] = lookup('*');

	}


	// Case 2, temperature between -99.9 and -0.1
	if (value >= -99.9 && value < 0) {
		
		// Convert value to string representing two digits
		ones = '0' + converted_val % 10;
		tens = '0' + converted_val / 10;

		// If there is no tens place
		if ((converted_val / 10) == 0) {
			
			// Put the decimal tenths on the display instead
			tenths = '0' + (int)(value * 10) % 10;
			
			// Put newly found tenths and a decimal point into the message
			message[1] = lookup(ones) + 0x80;
			message[2] = lookup(tenths);

		}

		else {

			// Tens place is not empty, proceed as normal
			message[1] = lookup(tens);
			message[2] = lookup(ones);

		}

		message[0] = lookup('-');
		message[3] = lookup('*');

	}

	// Case 3, Temperature between 100 and 999
	if (value >= 100.0 && value <= 999.0) {
	
		// Convert value to string representing a 3 digit number	
		ones = '0' + converted_val % 10;
		tens = '0' + (converted_val % 100) / 10;
		hundreds = '0' + converted_val / 100;

		// Message to be displayed is 3 digits plus a degree symbol at the end
		message[0] = lookup(hundreds);
		message[1] = lookup(tens);
		message[2] = lookup(ones);
		message[3] = lookup('*');

	}

	//Case 4, Temperatures that just arent cool (they're errors, or exceed range)
	if (value < -99.9 || value > 999) {
		
		//Write poop to the LED display
		message[0] = lookup('p');
		message[1] = lookup('o');
		message[2] = lookup('o');
		message[3] = lookup('p');

	}


	// Create a new buffer
	char *buffer = malloc(17 * (sizeof(char)));
	
	// Clear output said buffer	
	memset(buffer, 0, sizeof(char) * 17);

	// Set the address to display data address
	buffer[0] = 0x0;
	
	buffer[1] = message[0];
	buffer[3] = message[1];
	buffer[7] = message[2];
	buffer[9] = message[3];

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
		printf("Error closing file, %s!\n", strerror(errno));
		return -1;
	}

	return 0;

}



int init_display(void) {

	int fd, result;

	char buffer[17];

	char i2c_device[] = "/dev/i2c-1";

	/* Open i2c device */
	fd = open(i2c_device, O_RDWR);
	if (fd < 0) printf("Error opening device\n");
	
	/* Set slave address */
	result = ioctl(fd, I2C_SLAVE, 0x70);
	if (result < 0) printf("Error opening device\n");

	/* Turn on oscillator */
	buffer[0] = (0x2<<4) | (0x1);
	result = write(fd, buffer, 1);

	/* Turn on Display, No Blink */
	buffer[0] = (0x8 << 4) | (0x1);
	result = write(fd, buffer, 1);

	/* Set Brightness */
	buffer[0] = (0xE << 4) | (0xE);
	result = write(fd, buffer, 1);


	return fd;


}

