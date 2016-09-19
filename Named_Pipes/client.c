#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "coms.h"

int main() {
	connection * con = connect("190.server.com");
	printf("CLIENT DONE\n");
	while(1) {
		int a = 0;
		a++;
	}
}