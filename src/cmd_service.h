#pragma once
#include "uv.h"

void service_on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void service_cron(uv_timer_t *req);