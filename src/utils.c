#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "yuarel.h"
#include "logging.h"

char *utils_GetImageType(char *url) {

    struct yuarel yrl;
    struct yuarel_param params[3];
    int p;
    char *imgtype = NULL;

    // Parse url
    if (yuarel_parse(&yrl, url) < 0) {
        log_Warning("Failed to parse url.");
        return NULL;
    }

    // If query string is null
    if (yrl.query == NULL) {
        return NULL;
    }

    // Parse all query strings and find one with key of type, set its value to imgtype ptr
    p = yuarel_parse_query(yrl.query, '&', params, 3);
    while (p-- > 0) {
        if (strlen(params[p].key) == 4 && (strncmp(params[p].key, "type", 4) == 0)) {
            imgtype = params[p].val;
        }
    }

    return imgtype;

}