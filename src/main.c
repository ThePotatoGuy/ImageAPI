#include <stdlib.h>
#include <stdio.h>

#include "server.h"

int main(int argc, char *argv[]) {

    if (server_Start("imapi.sock") < 0) {
        printf("Failed to start server.\n");
        return 1;
    }
    return 0;

}
