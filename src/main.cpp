#include "CmdService.h"
#include "DeviceMgr.h"
#include "Log.h"
#include "RestApi.h"
#include "uv.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>

// TODO: make it config
#define GATEWAY_PORT 8080

int main() {
    struct sockaddr_in addr;
    uv_loop_t *loop;
    uv_udp_t server;
    uv_udp_t client;

    if (log_init("log.cfg") != 0) {
        printf("log init error.");
        return -1;
    }

    if (wiringPiSetup() != 0) {
        zlog_error(g_log, "wiringPi init error.");
        return -1;
    }

    if (restapi_init() != 0) {
        zlog_error(g_log, "restapi init error.");
        return -1;
    }

    if (DeviceManager::Instance().Start()) {
        zlog_error(g_log, "device init error.");
        return -1;
    }

    loop = uv_default_loop();
    uv_udp_init(loop, &server);
    uv_ip4_addr("0.0.0.0", GATEWAY_PORT, &addr);
    uv_udp_bind(&server, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
    if (uv_udp_recv_start(&server, alloc_buffer, service_on_read) != 0) {
        zlog_error(g_log, "udp service start error.");
        return -1;
    }

    uv_timer_t cron;
    uv_timer_init(loop, &cron);
    uv_timer_start(&cron, service_cron, 200, 0);

    zlog_info(g_log, "SnoopyGateway init success.");

    uv_run(loop, UV_RUN_DEFAULT);

    zlog_info(g_log, "SnoopyGateway will exit.");
    restapi_exit();
    log_exit();
    return 0;
}
