#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLK "\e[1;30m"
#define RED "\e[1;31m"
#define GRN "\e[1;32m"
#define YEL "\e[1;33m"
#define BLU "\e[1;34m"
#define MAG "\e[1;35m"
#define CYN "\e[1;36m"
#define WHT "\e[1;37m"

// Size represents the amount of different colors used above
#define Size 8

// Function takes in a name, and prints in a colored, helical fashion, for the amount of letters in that name
void  print_map(char * name) {

	// Get string length now to help with pointers later
	int length = strlen(name);

	// All our text colors
	char colors[Size][10] = {BLK, RED, GRN, YEL, BLU, MAG, CYN, WHT};

	// For as many letters in the name, colored print the name in a helical fashion
	for (int k = 0; k < (length); k++) {
		for (int i = 1; i < (length); i++) {
			if (strlen(name) > 1) {	
			
				name++;
				printf("%s%s\n", colors[(rand() % (Size - 1))], name);
			
			}
		}
		for (int i = 1; i < length; i++) {

			name--;
			printf("%s%s\n", colors[(rand() % (Size - 1))], name);
		
		}	
	}

	return;

}	

// Function prints a message 12 times on 12 separate lines with each character a different color
void different_color_print(char message[]) {

	// An array of predefined colors, eases use in loops
	char colors[Size][10] = {BLK, RED, GRN, YEL, BLU, MAG, CYN, WHT};


	// For each line
	for(int i = 1; i < 13; i++) {
		//Print the line number and a hashtag with a random color
		printf("%s#", colors[(rand() % 7)]);
		printf("%s%d", colors[rand() % 7], i);
		for (int j = 0; j < (strlen(message)); j++) {

			// Then print each character of the message with different colors
			printf("%s%c", colors[(rand() % 7)], message[j]);	
		
		}
		
		printf("\n");
	
	}

}


int main(int argc, char **argv) {

	// Create an array of each color. This will ease color changing later
	
	// If an argument is specified, print error
	if (argc > 1) {
		printf("Error, too many arguments\n!");
		return 0;
	}

	// String to hold user entered name
	char name[30] = "";

	char *name_ptr = name;

	printf("Here is the message with each letter printed a different color!\n");

	different_color_print("ECE471 is a radical course");

	printf("Please enter your first name\n");
	scanf("%s", name);

	print_map(name_ptr);
	return 0;
}
