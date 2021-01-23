#include "log.h"

zlog_category_t *g_log = NULL;

int log_init(char * cfg_path) {
    int ret = zlog_init(cfg_path);
    if (ret) {
        printf("init failed\n");
        return -1;
    }

    g_log = zlog_get_category("snoopy");
    if (!g_log) {
        printf("get cat fail\n");
        zlog_fini();
        return -1;
    }

    return 0;
}

int log_exit() {
    zlog_fini();
    return 0;
}