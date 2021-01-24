#pragma once

#include <stdlib.h>

typedef struct cc2530_buf_t {
  char* base;
  size_t len;
  size_t capacity;
} cc2530_buf_t;

typedef void (*recv_callback_t)(cc2530_buf_t buf);

static void *receive_thread(void *arg);
int cc2530_init(const char * uart_dev, int baud);
int cc2530_send(cc2530_buf_t buf);
int cc2530_on_receive(recv_callback_t func);
int cc2530_exit();