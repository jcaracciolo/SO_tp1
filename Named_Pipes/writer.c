#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "coms.h"

int main() {
    write(3, "Hii", sizeof("Hii"));
	connection * con = connect(NULL);

    int fd;
    char * myfifo = "/tmp/server_fifo";

    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY);
    //write(fd, "Hi", sizeof("Hi"));
    close(fd);

    return 0;
}
