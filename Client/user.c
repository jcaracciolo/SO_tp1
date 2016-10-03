#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Server/Coms/coms.h"
#include "../Server/Marshalling/marsh.h"
#include "../Server/DB/UUID_DataBase/data_types.h"
#include "client.h"

#define MAX_BUF 255


int readArgs3(char * args, char * arg1, char * arg2, char * arg3);
voiddisplayHelp();
int isNumber(char * str);
productInfo_t * getProduct(productInfo_t * products, char * prodName);

 int main() {
 	int c, i = 0;
 	char entry[MAX_BUF];
    int pid = getpid();
    int money = 20;

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


	productInfo_t products[MAX_PRODUCTS];
	initProducts(products);

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

 		} else if (len >= 10 && strncmp(entry, "can i buy ", 10) == 0) {
 			char prodName[MAX_BUF];
 			i = 10;
 			int j = 0;
 			while(entry[i] != '\0') {
 				prodName[j++] = entry[i++];
 			}
 			prodName[j] = '\0';
			if (isProdInDB(con, prodName, pid)) {
				printf("Yes, you can buy %ss.\n", prodName, stock == 1 ? "" : "s");
			} else {
				printf("No, you can't buy %ss.\n", prodName, stock == 1 ? "" : "s");
			}
			

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

 			if (readArgs3(entry + 4, &prodName, &quantity, &maxPrice) != 0 || !isNumber(quantity) || !isNumber(maxPrice)) {
 				puts("Error reading arguments");
 				continue;
 			}

 			int totalPrice=0, ack;
   			ack = sendBuyTransaction(con, prodName, atoi(quantity), atoi(maxPrice), stock, &totalPrice,pid);
   			if (ack < 0 || money < totalPrice) {
   				printError(ack);
   			} else {
   				printf("You bought %s %s%s for a total cost of %i gold coins.\n", quantity, prodName, atoi(quantity) == 1 ? "" : "s", totalPrice);
   				money -= totalPrice;
   			}


 		} else if (len >= 5 && strncmp(entry, "sell ", 5) == 0) {
 			char prodName[MAX_BUF], quantity[MAX_BUF], minPrice[MAX_BUF];

 			if (readArgs3(entry + 5, &prodName, &quantity, &minPrice) != 0 || !isNumber(quantity) || !isNumber(minPrice)) {
 				puts("Error reading arguments");
 				continue;
 			}

 			productInfo_t * prod = getProduct(products, prodName);
 			if (prod == NULL) {
 				puts("Product does not exists.");
 				continue;
 			}

 			int profit, res;
  			int ack = sendSellTransaction( con, prod->prodName, atoi(quantity), atoi(quantity) * atoi(minPrice), prod->stock, &profit, pid);
   			if (ack < 0 || profit < 0) {
				printError(ack);
   			} else {
   				printf("You sold %s %s%s and you gained %i gold coins.\n", quantity, prodName, atoi(quantity) == 1 ? "" : "s", profit);
   				money += profit;
   			}
   		

   		} else {
   			puts("Unknown command");
   		}
	}

 }

char* conerrormsg[]={
		"Conection lost",
		"Insuficient amount of products",
		"Insuficient stock",
		"Maximun amount of UUIDs per transaction exceded",
		"Money provided not enough to concrete purchase",
		"Transaction revenue not enough to reach minimal payment",
		"Invalid UUIDs",
		"No such element available"};

void printError(conerrors_t error){
	if(error > NOSUCHELEMENT){
		puts("Not know error");
	}else{
		puts(conerrormsg[error-NOCONECTION]);
	}
}

productInfo_t * getProduct(productInfo_t * products, char * prodName) {
	int i;
	for (i= 0; i < MAX_PRODUCTS; i++) {
		if (strcmp(products[i].prodName, prodName) == 0) {
			return &(products[i]);
		}
	}
	return NULL;
}

int initProducts(productInfo_t * products){
  int i ;
  for (i= 0; i < MAX_PRODUCTS; i++) {
    strcpy(products[i].prodName,validProd[i]);
    products[i].newPrice = -1;
    products[i].prevPrice = -1;
    products[i].priceTrend = 0;
    products[i].remoteStock = -1;
    products[i].stock = malloc(sizeof(UUIDStock));
    products[i].stock->size = 0;
    products[i].stock->last = 0;
    products[i].stock->uuids = NULL;
  }
}

int isNumber(char * str) {
	int i = 0;
	while (str[i] != '\0') {
		if (!isdigit(str[i++]))
			return 0;
	}
	return 1;
}

int readArgs3(char * args, char * arg1, char * arg2, char * arg3) {
	int i = 0, j = 0;
	while(args[i] != ' ') {
		if (args[i] == '\0') {
			return -1;
		}
		arg1[j++] = args[i++];
	}
	arg1[j] = '\0';
	i++;

	j = 0;
	while(args[i] != ' ') {
		if (args[i] == '\0') {
			return -1;
		}
		arg2[j++] = args[i++];
	}
	arg2[j] = '\0';
	i++;

	j = 0;
	while(args[i] != '\0') {
		arg3[j++] = args[i++];
	}
	arg3[j] = '\0';
	return 0;
 }


 void displayHelp() {
 	puts("Commands available:");
 	puts("\tcan i buy [product name]");
 	puts("\tget stock [product name]");
 	puts("\tget price [product name]");
 	puts("\tbuy [product name] [quantity] [max total price]");
 	puts("\tsell product name] [quantity] [min total price]");
 	puts("\t--help");
 	puts("\texit");
 }