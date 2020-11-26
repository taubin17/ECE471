#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <linux/gpio.h>

#define samples 100000


void light_led(struct gpiohandle_request req2, int state) {
	
	// Create data structure to hold state of LED and rv to hold errors returned by IOCTL
	struct gpiohandle_data data;
	int rv;

	// Clear the struct
	memset(&data, 0, sizeof(data));	

	// Set GPIO 18 to the state input
	data.values[0] = state;
		
	// Send input to GPIO 18
	rv = ioctl(req2.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
	if (rv < 0) printf("Error in IOCTL. %s\n", strerror(errno));
	
	return;

}	


int read_switch(struct gpiohandle_request req) {

	// Create a struct to store GPIO data, and integer to store IOCTL errors	
	struct gpiohandle_data data;
	int rv;
	
	// Clear data structure
	memset(&data, 0, sizeof(data));

	// Call IOCTL to read line and check for proper execution
	rv = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
	if (rv < 0) printf("Error in IOCTL. %s\n", strerror(errno));

	// Return values from the line
	return data.values[0];
}


int main(int argc, char **argv) {

	// File descriptor for GPIO chip 0 and IOCTL error storage variable
	int fd, rv;
	int state = 0;

	// Array of current and previous states, used for software debouncing
	int list_of_states[samples];
	
	// Fill array with zeroes
	for (int k = 0; k < samples; k++) {
		list_of_states[samples] = 0;
	}

	// Counters for consistent low readings, high readings, and last known switch position
	int stable_low = 0;
	int stable_high = 0;
	int switch_high = 0;

	// Struct to talk to GPIO 17
	struct gpiohandle_request req;

	// Struct to talk to GPIO 18
	struct gpiohandle_request req2;
	
	// Open GPIO file
	fd = open("/dev/gpiochip0", O_RDWR);
	if (fd < 0) printf("Error opening file. %s\n", strerror(errno));

	// Clear memory in struct
	memset(&req, 0, sizeof(struct gpiohandle_request));
	memset(&req2, 0, sizeof(struct gpiohandle_request));

	// Only one line to be read, line 17
	req.flags = GPIOHANDLE_REQUEST_INPUT;
	req.lines = 1;
	req.lineoffsets[0] = 17;

	// Set flags for GPIO 18
	req2.flags = GPIOHANDLE_REQUEST_OUTPUT;
	req2.lines = 1;
	req2.lineoffsets[0] = 18;

	
	// Request pin 17, and check if successful
	rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (rv < 0) printf("Error in IOCTL. %s\n", strerror(errno));

	// Request pin 18, and check if successful
	rv = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req2);
	if (rv < 0) printf("Error in IOCTL. %s\n", strerror(errno));

	close(fd);	
	
	while(1) {
		// Move all elements in the array down by one, and remove last element
		for (int i = samples-1; i > 0; i--) {
			list_of_states[i-1] = list_of_states[i];
		}
	
		// Store newest data in last element	
		list_of_states[samples-1] = read_switch(req);

		// Check if all bytes of the array hold the same value
		for (int j = 0; j < samples; j++) {
			
			if (list_of_states[j] == 1) {
				stable_high += 1;
			}
			else {
				stable_low += 1;
			}
		}
		
		// If the switch's last known position was high and all readings are low, switch off	
		if (stable_low == samples && switch_high == 1) {
			printf("Switch is off!\n");
			switch_high = 0;
			light_led(req2, 0);
		}

		// If switch's last known position was low and all readings are high, switch on
		if (stable_high == samples && switch_high == 0) {
			printf("Switch is on!\n");
			switch_high = 1;
			light_led(req2, 1);
		}

		// Reset counters
		stable_high = 0;
		stable_low = 0;
			
	}
	return 0;
}
