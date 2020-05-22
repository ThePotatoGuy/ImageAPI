#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *getTimeStr() {

    char *buf = malloc(20);
    struct tm *sTm = NULL;

    time_t now = time(0);
    sTm = gmtime(&now);

    strftime(buf, 20, "%Y/%m/%d %H:%M:%S", sTm);
    return buf;

}

void log_msg(FILE *fp, char x, char *msg) {

    char *timeStr = getTimeStr();
    fprintf(fp, "[%s][%c] %s\n", timeStr, x, msg);
    free(timeStr);

}

void log_Info(char *msg) {
    log_msg(stdout, 'I', msg);
}

void log_Warning(char *msg) {
    log_msg(stderr, 'W', msg);
}

void log_Debug(char *msg) {
#ifdef DEBUG
    log_msg(stdout, 'D', msg);
#endif
}
