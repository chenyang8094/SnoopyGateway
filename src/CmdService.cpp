#include "CmdService.h"
#include "DeviceMgr.h"
#include "Log.h"
#include "json.hpp"

#include <wiringPi.h>

#define CMD_SET "set"
#define CMD_GET "get"
#define CMD_HEARTBEAT "heartbeat"

#define IF_SET_CMD(cmd) !memcmp(cmd, CMD_SET, strlen(CMD_SET))
#define IF_GET_CMD(cmd) !memcmp(cmd, CMD_GET, strlen(CMD_GET))
#define IF_HEARTBEAT_CMD(cmd) !memcmp(cmd, CMD_HEARTBEAT, strlen(CMD_HEARTBEAT))

static struct sockaddr hb_addr;

void service_on_read(uv_udp_t* req, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    if (nread < 0)
    {
        zlog_error(g_log, "read error %s", uv_err_name(nread));
        uv_close((uv_handle_t*)req, NULL);
        free(buf->base);
        return;
    }

    if (nread == 0)
        return;

    // printf("%.*s\n", buf->len, buf->base);

    try
    {
        auto jsonobj = nlohmann::json::parse(std::string(buf->base, buf->len));
        if (!jsonobj.is_object()) {
            return;
        }

        if (jsonobj.contains("cmd") && jsonobj["cmd"].is_string()) {
            std::string cmd,sid;
            jsonobj["cmd"].get_to(cmd);

            if (cmd == "set") {
                if (jsonobj.contains("sid") && jsonobj["sid"].is_string()) {
                    jsonobj["sid"].get_to(sid);
                }

                if (jsonobj.contains("data") && jsonobj["data"].is_object()) {
                    auto device = DeviceManager::Instance().FindDevice(sid);
                    device->SetCommand(jsonobj["data"]);
                }

            } else if (cmd == "get") {

            } else {

            }
        }

    } catch (nlohmann::json::exception &e) {

    }
    
}

void alloc_buffer(uv_handle_t* handle,
                  size_t suggested_size,
                  uv_buf_t* buf)
{
    static char slab[65536];
    memset(slab, 0, sizeof(slab));

    buf->base = slab;
    buf->len = sizeof(slab);
}

void service_cron(uv_timer_t* req)
{
     

    // cJSON* report;
    // while ((report = (cJSON*)lfqueue_deq(&msg_queue)) != NULL)
    // {
    //     char* report_str = cJSON_Print(report);
    //     uv_buf_t w_buf = uv_buf_init(report_str, strlen(report_str));
    //     uv_udp_send_t* ureq = malloc(sizeof(uv_udp_send_t));
    //     uv_udp_send(ureq, req, &w_buf, 1, &hb_addr, NULL);
    //     cJSON_Delete(report);
    // }
}
