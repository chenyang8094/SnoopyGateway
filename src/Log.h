#pragma once

#include "zlog.h"

extern zlog_category_t *g_log;

int log_init(char * cfg_path);
int log_exit();

