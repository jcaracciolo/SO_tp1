#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int getPrice(char * prodName, dbdata_t * dbData) {
	char ans[200];
	int length = strlen(prodName);
	char * s = malloc(length + 43); // name + search +  "'" + "'" + '\n' + '\0'

	memcpy(s, "select price from product where name = '", 40);
	memcpy(s+40, prodName, length);
	memcpy(s+40+length, "'\n", 2);
	

	write(dbData->fdin, s, s+40+length);
	read(dbData->fdout, ans, 200);

	free(s);

	return atoi(ans);
	
}

int getStock(char * prodName, dbdata_t * dbData) {
	char ans[200];
	int length = strlen(prodName);
	char * s = malloc(length + 43); // name + search +  "'" + "'" + '\n' + '\0'

	memcpy(s, "select stock from product where name = '", 40);
	memcpy(s+40, prodName, length);
	memcpy(s+40+length, "'\n", 2);
	

	write(dbData->fdin, s, s+40+length);
	read(dbData->fdout, ans, 200);

	free(s);

	return atoi(ans);
	
}

int main() {
	getPrice("papas",  "Nothing");
}
