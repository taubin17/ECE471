#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Function takes an integer between 1 and 16, and returns the equivalent hex character
char return_letter(uint32_t remainder) {


	switch (remainder) {

		case 10:
			return 'A';
			break;
		case 11:
			return 'B';
			break;
		case 12:
			return 'C';
			break;
		case 13:
			return 'D';
			break;
		case 14:
			return 'E';
			break;
		case 15:
			return 'F';
			break;
		// If the number in hex is the same as the current number	
		default:
			// Just return it as a character
			return remainder+'0';
	}	

}


uint32_t print_integer(uint32_t value) {

	char output[10];		/* a buffer to hold our result */
	char *output_pointer;		/* a character pointer */

	uint32_t quotient,remainder;

	printf("Decimal Value: %d\n", value);	

	/* NUL terminate the string */
	output[9]=0;

	/* point to end of string, we'll work backwards */
	output_pointer=output+8;

	while(1) {
		
		// Get the quotient for next run
		quotient = value/16;
		// Get the remainder to determine hex value
		remainder = value%16;
		
		// Determine the hex character and set the output pointer to point to it
		*output_pointer=return_letter(remainder);

		/* if quotient is 0 we are done with the number */
		if (quotient==0) break;

		/* point pointer to next digit (From right to left) */
		output_pointer--;

		/* repeat using the quotient this time, until 0 is reached */
		value=quotient;
	}

	/* print the string. */
	printf("Hexadecimal Value: 0x%s\n",output_pointer);

	return 0;
}


int main(int argc, char **argv) {

	//Variable to temp hold user value
	char user_num[9];

	uint32_t value = 0;
	//Take in user ineteger
	printf("Please enter the integer you wish to convert to hex!\n");
	scanf("%s", user_num);

	value = atoi(user_num);

	// Check if user entered something that is a number
	if (value == 0) {
		printf("You entered 0 or an invalid number (No letters please!)\n");
		return -1;
	}	
	// Message is an integer, run the program
	print_integer(value);

	return 0;
}
