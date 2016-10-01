#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"

#define MAX_BUF 255

 int main() {
 	int c, i = 0;
 	char entry[MAX_BUF];
 	puts("Select an address to connect: ");

 	while((c = getchar()) != '\n') {
 		entry[i++] = c;
 	}
 	entry[i] = '\0';

 	printf("Connecting to %s...\n", entry);

	connection * con = connectToAddres(entry);

	if (con == NULL) {
		puts("Connection failed");
		return -1;
	}
	puts("Connected succesfully");

	while (1) {
		printf("Enter a command (--help for help):\n");

		i = 0;
		while((c = getchar()) != '\n') {
	 		entry[i++] = c;
 		}
 		entry[i] = '\0';

 		if (strcmp(entry, "--help") == 0) {
 			displayHelp();
 		}
	}

 }

 void displayHelp() {
 	puts("Commands available:");
 	puts("\tget stock [product name]");
 	puts("\tget price [product name]");
 	puts("\tbuy [product name] [quantity] [max price]");
 	puts("\tsell product name] [quantity]\n");
 }