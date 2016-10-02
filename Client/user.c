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
    int pid = getpid();
    int money = 50000;

	// Connecting to Server
 	puts("Select an address to connect ('default' to connect locally):");

 	while((c = getchar()) != '\n') {
 		entry[i++] = c;
 	}
 	entry[i] = '\0';

 	if (strcmp(entry, "default") == 0) {
 		strcpy(entry, "127.0.0.1:5000/localhost");
 	}

 	printf("Connecting to %s...\n", entry);

	connection * con = connectToAddres(entry);

	if (con == NULL) {
		puts("Connection failed");
		return -1;
	}
	puts("Connected succesfully");



    // Creating UUIDStock
    UUIDStock * stock = malloc(sizeof(UUIDStock));
    stock->size = 0;
    stock->last = 0;
    stock->uuids = NULL;


	// Interaction loop
	while (1) {
		printf("\nYou have %i gold coins\nEnter a command (--help for help):\n", money);

		i = 0;
		while((c = getchar()) != '\n') {
	 		entry[i++] = c;
 		}
 		entry[i] = '\0';

 		int len = strlen(entry);

 		if (strcmp(entry, "--help") == 0) {
 			displayHelp();


 		} else if (strcmp(entry, "exit") == 0) {
 			puts("Disconecting...");
 			disconnect(con);
 			return 0;


 		} else if (len >= 10 && strncmp(entry, "get stock ", 10) == 0) {
 			char prodName[MAX_BUF];
 			i = 10;
 			int j = 0;
 			while(entry[i] != '\0') {
 				prodName[j++] = entry[i++];
 			}
 			prodName[j] = '\0';
			int stock = getStockFromDB(con,prodName,pid);
			printf("There are %i %s%s available to buy.\n", stock, prodName, stock == 1 ? "" : "s");

 		} else if (len >= 10 && strncmp(entry, "get price ", 10) == 0) {
 			char prodName[MAX_BUF];
 			i = 10;
 			int j = 0;
 			while(entry[i] != '\0') {
 				prodName[j++] = entry[i++];
 			}
 			prodName[j] = '\0';
			int price = getPriceFromDB(con,prodName,pid);
			printf("One %s costs %i gold coin%s.\n", prodName, price, price == 1 ? "" : "s");
 		

 		} else if (len >= 4 && strncmp(entry, "buy ", 4) == 0) {
 			char prodName[MAX_BUF], quantity[MAX_BUF], maxPrice[MAX_BUF];
 			i = 4;
 			int j = 0;
			while(entry[i] != ' ') {
 				if (entry[i] == '\0') {
 					puts("Error on command arguments.");
 					break;
 				}
 				prodName[j++] = entry[i++];
 			}
 			prodName[j] = '\0';
 			i++;

 			j = 0;
 			while(entry[i] != ' ') {
 				if (entry[i] == '\0') {
 					puts("Error on command arguments.");
 					break;
 				}
 				quantity[j++] = entry[i++];
 			}
 			quantity[j] = '\0';
 			i++;

 			j = 0;
 			while(entry[i] != '\0') {
 				maxPrice[j++] = entry[i++];
 			}
 			maxPrice[j] = '\0';

 			int totalPrice, res;
   			res = sendBuyTransaction(con, prodName, atoi(quantity), atoi(maxPrice), stock, &totalPrice,pid);
   			if (res < 0) {
   				puts("Buy operation cancelled, please try again.");
   			} else {
   				printf("You bought %s %s%s for a total cost of %i gold coins.\n", quantity, prodName, atoi(quantity) == 1 ? "" : "s", totalPrice);
   			}


 		 } //else if (len >= 5 && strncmp(entry, "sell ", 5) == 0) {
 		// 	char prodName[MAX_BUF], quantity[MAX_BUF], maxPrice[MAX_BUF];
 		// 	i = 5;
 		// 	int j = 0;
			// while(entry[i] != ' ') {
 		// 		if (entry[i] == '\0') {
 		// 			puts("Error on command arguments");
 		// 			break;
 		// 		}
 		// 		prodName[j++] = entry[i++];
 		// 	}
 		// 	prodName[j] = '\0';
 		// 	i++;

 		// 	j = 0;
 		// 	while(entry[i] != ' ') {
 		// 		if (entry[i] == '\0') {
 		// 			puts("Error on command arguments");
 		// 			break;
 		// 		}
 		// 		quantity[j++] = entry[i++];
 		// 	}
 		// 	quantity[j] = '\0';
 		// 	i++;

 		// 	j = 0;
 		// 	while(entry[i] != '\0') {
 		// 		maxPrice[j++] = entry[i++];
 		// 	}
 		// 	maxPrice[j] = '\0';

 		// 	int totalPrice, res;
   // 			res = sendSellTransaction(con, prodName, atoi(quantity), atoi(maxPrice), stock, &totalPrice,pid);
   // 			if (res < 0) {
   // 				puts("Buy operation cancelled, please try again.");
   // 			} else {
   // 				printf("You bought %s %ss for a total cost of %i\n", quantity, prodName, totalPrice);
   // 			}
	}

 }


 void displayHelp() {
 	puts("Commands available:");
 	puts("\tget stock [product name]");
 	puts("\tget price [product name]");
 	puts("\tbuy [product name] [quantity] [max price]");
 	puts("\tsell product name] [quantity]");
 	puts("\t--help");
 	puts("\texit");
 }