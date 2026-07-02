//=================================================================
// config_loader.cpp
// Note: Config parser for the MiniScheduler daemon.
//       Parses a "key=value" text line. Keys not present in the file will be assigned defaults.
//       Lines starting with '#' or blank lines are ignored.
//=================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/config/config.h"
#include "../../include/config/config_loader.h"
#include "../../include/ipc/def.h"

//----------------------------------------------------------
// Note: Parse file_path config file into config.
//
int config_load(const char *file_path, minisched_config_t *config)
{
    // Use defaults value if the key is missing.
    config->num_workers = MINISCHED_DEFAULT_NUM_WORKERS;
    strncpy(config->socket_path, MINISCHED_DEFAULT_SOCKET_PATH, sizeof(config->socket_path) - 1);
    config->socket_path[sizeof(config->socket_path)-1] = '\0';

    FILE* fp = fopen(file_path, "r");
    if(!fp) {
        // If the file is not found, use defaults and return success.
        log_error("[%s][%s:%d] Config file not found, using defaults.", __FILE__,__func__,__LINE__);
        return 0;
    }

    char line[512];
    while(fgets(line, sizeof(line), fp)) {
        // Skip blank lines and comments.
        if( line[0] == '#' || line[0] == '\r' || line[0] == '\n' ) {
            continue;
        }

        char key[128] = {0};
        char value[256] = {0};

        // Parse "key=value".
        // %127[^=] means read up to 127 characters that are not '=' into key.
        // = is a required literal character.
        if( sscanf(line, "%127[^=]=%255[^\r\n]", key, value) != 2 ) {
            continue;
        }

        if(strcmp(key,"socket_path") == 0) {
            strncpy(config->socket_path, value, sizeof(config->socket_path) - 1);
            config->socket_path[sizeof(config->socket_path) - 1] = '\0';
        } else if(strcmp(key, "num_workers") == 0) {
            int n = atoi(value);
            if( n >= 0 ) {
                config->num_workers = n;
            }
        }
    }

    fclose(fp);
    return 0;
}