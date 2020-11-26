#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


static double read_temp(char *filename) {

	// Variables to hold "Yes" as well as "t=24125"
	char string1[256], string2[256];
	char *test;

	FILE *fd;

	// Open a file for our 1 Wire device
	fd = fopen(filename, "r");

	// Determine if the sensor is ready by checking last field of first string (it will say yes if ready(
	fscanf(fd, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", string1);
	// Check to see if it returns "yes"
	if (strcmp(string1, "YES") == 0){
		// Find the temperature
		fscanf(fd, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %s", string2);
		
		// Remove the t= fluff
		test = string2;
		test += 2;

		// Close our file
		fclose(fd);

		// Return the temperature
		return (atoi(test)/1000);
	}
	else {
		
		// Close our file due to device not ready, and return an error
		fclose(fd);
		printf("Error, device not ready\n");
		return 0;

	}


}

int main(int argc, char **argv) {

	double temp1;

	if (argc < 2) {
	
		printf("Error, no device specified!\n");
		return 0;
	}
	if (argc > 2) {
		printf("Error, too many devices specified!\n");
		return 0;
	}

	while(1) {

		temp1=read_temp(argv[1]);
		printf("%.2lfC\n",temp1);
		sleep(1);
	}

	return 0;
}
