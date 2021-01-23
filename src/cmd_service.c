#include "cmd_service.h"
#include "cjson/cJSON.h"
#include "log.h"
#include "device.h"
#include <wiringPi.h>

#define CMD_SET "set"
#define CMD_GET "get"
#define CMD_HEARTBEAT "heartbeat"

#define IF_SET_CMD(cmd) !memcmp(cmd, CMD_SET, strlen(CMD_SET))
#define IF_GET_CMD(cmd) !memcmp(cmd, CMD_GET, strlen(CMD_GET))
#define IF_HEARTBEAT_CMD(cmd) !memcmp(cmd, CMD_HEARTBEAT, strlen(CMD_HEARTBEAT))

static struct sockaddr hb_addr;

void service_on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags)
{
    if (nread < 0)
    {
        zlog_error(g_log, "read error %s", uv_err_name(nread));
        uv_close((uv_handle_t *)req, NULL);
        free(buf->base);
        return;
    }

    if (nread == 0)
        return;

    // printf("%.*s\n", buf->len, buf->base);

    cJSON *jsonobj = cJSON_ParseWithLength(buf->base, buf->len);
    if (jsonobj == NULL)
    {
        zlog_error(g_log, "json parse erro.");
        return;
    }

    cJSON *cmd = cJSON_GetObjectItem(jsonobj, "cmd");
    if (cmd && cmd->type == cJSON_String)
    {
        if (IF_SET_CMD(cmd->valuestring))
        {
            cJSON *sid = cJSON_GetObjectItem(jsonobj, "sid");
            if (sid == NULL || sid->type != cJSON_String)
            {
                zlog_error(g_log, "sid parse error.");
                goto done;
            }

            cJSON *data = cJSON_GetObjectItem(jsonobj, "data");
            if (data == NULL || data->type != cJSON_Object)
            {
                zlog_error(g_log, "data parse error.");
                goto done;
            }

            device_t *device = (device_t *)find_device_by_sid(sid->valuestring);
            if (device)
            {
                device->set(device, data);
            }
            else
            {
                zlog_error(g_log, "device %s not found.", sid->valuestring);
                goto done;
            }
        }
        else if (IF_GET_CMD(cmd->valuestring))
        {
            cJSON *sid = cJSON_GetObjectItem(jsonobj, "sid");
            if (sid == NULL || sid->type != cJSON_String)
            {
                zlog_error(g_log, "sid parse error.");
                goto done;
            }

            cJSON *data = cJSON_GetObjectItem(jsonobj, "data");
            if (data == NULL || data->type != cJSON_Object)
            {
                zlog_error(g_log, "data parse error.");
                goto done;
            }

            device_t *device = find_device_by_sid(sid->valuestring);
            if (device)
            {
                device->get(device, data);
            }
            else
            {
                zlog_error(g_log, "device %s not found.", sid->valuestring);
                goto done;
            }
        }
        else if (IF_HEARTBEAT_CMD(cmd->valuestring))
        {
            memcpy(&hb_addr, addr, sizeof(struct sockaddr));
        }
    }
    else
    {
        zlog_error(g_log, "cmd parse error");
        goto done;
    }

done:
    cJSON_Delete(jsonobj);
}

void alloc_buffer(uv_handle_t *handle,
                  size_t suggested_size,
                  uv_buf_t *buf)
{
    static char slab[65536];
    memset(slab, 0, sizeof(slab));

    buf->base = slab;
    buf->len = sizeof(slab);
}

void service_cron(uv_timer_t *req)
{
    if (lfqueue_size(&msg_queue) == 0)
    {
        return;
    }

    cJSON *report;
    while ((report = (cJSON *)lfqueue_deq(&msg_queue)) != NULL)
    {
        char *report_str = cJSON_Print(report);
        uv_buf_t w_buf = uv_buf_init(report_str, strlen(report_str));
        uv_udp_send_t *ureq = malloc(sizeof(uv_udp_send_t));
        uv_udp_send(ureq, req, &w_buf, 1, &hb_addr, NULL);
        cJSON_Delete(report);
    }
}
