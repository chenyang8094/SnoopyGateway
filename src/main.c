#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>
#include "uv.h"
#include "device.h"
#include "restapi.h"
#include "cjson/cJSON.h"

#define GATEWAY_PORT 8080

#define LED 7

#define CMD_SET "set"
#define CMD_GET "get"
#define CMD_HEARTBEAT "heartbeat"

#define IF_SET_CMD(cmd) !memcmp(cmd, CMD_SET, strlen(CMD_SET))
#define IF_GET_CMD(cmd) !memcmp(cmd, CMD_GET, strlen(CMD_GET))
#define IF_HEARTBEAT_CMD(cmd) !memcmp(cmd, CMD_HEARTBEAT, strlen(CMD_HEARTBEAT))
    
void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
    if (nread < 0) {
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) req, NULL);
        free(buf->base);
        return;
    }
    if (nread == 0) return;

    // printf("%.*s\n", buf->len, buf->base);

    cJSON * jsonobj = cJSON_ParseWithLength(buf->base, buf->len);
    if (jsonobj == NULL) {
        printf("parse error\n");
        return;
    }

    cJSON * cmd = cJSON_GetObjectItem(jsonobj , "cmd");
    if (cmd && cmd->type == cJSON_String) {
        if (IF_SET_CMD(cmd->valuestring)) {
            cJSON * sid =  cJSON_GetObjectItem(jsonobj , "sid");
            if (sid && sid->type == cJSON_String) {
                printf("sid is: %s\n", sid->valuestring);
            } else {
                printf("sid error\n");
                goto err;
            }
            cJSON * data = cJSON_GetObjectItem(jsonobj, "data");
            if (data && data->type == cJSON_Object) {
                cJSON * state = cJSON_GetObjectItem(data, "state");
                if (state && state->type == cJSON_String) {
                    if (!memcmp(state->valuestring, "ON", strlen("ON"))) {
                        printf("LEN ON\n");
                        digitalWrite(LED, HIGH);
                    } else {
                        printf("LEN OFF\n");
                        digitalWrite(LED, LOW);
                    }
                }  
            } else {
                printf("data error\n");
                goto err;
            }
        } else if (IF_GET_CMD(cmd->valuestring)) {

        } else if (IF_HEARTBEAT_CMD(cmd->valuestring)) {
            static int battery = 0;
            if (battery == 101) battery = 0;

            // var data = '{"cmd": "report","sid": "0x00158d000414385c","state":{"battery":' + baterry + '}}';
            cJSON * report = cJSON_CreateObject();
            cJSON_AddStringToObject(report, "cmd", "report"); 
            cJSON_AddStringToObject(report, "sid", "0x00158d000414385c"); 
            cJSON * state = cJSON_AddObjectToObject(report, "state");
            cJSON_AddNumberToObject(state, "battery", battery++);
            char * report_str = cJSON_Print(report);

            uv_buf_t w_buf = uv_buf_init(report_str, strlen(report_str));
            uv_udp_send_t* ureq = malloc(sizeof(uv_udp_send_t));
            uv_udp_send(ureq, req, &w_buf,1, addr, NULL);
        }
    } else {
        printf("cmd error\n");
        goto err;
    }

err:     
    cJSON_Delete(jsonobj);
}

static void alloc_buffer(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
  static char slab[65536];
  memset(slab, 0, sizeof(slab));
  
  buf->base = slab;
  buf->len = sizeof(slab);
}

int main()
{
    struct sockaddr_in addr;
    uv_loop_t* loop;
    uv_udp_t server;
    uv_udp_t client;
    
    wiringPiSetup();
    restapi_init();
    device_global_init();
    
    loop = uv_default_loop();
    uv_udp_init(loop, &server);
    uv_ip4_addr("0.0.0.0", GATEWAY_PORT, &addr);
    uv_udp_bind(&server, (const struct sockaddr*) &addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&server, alloc_buffer, on_read);

    pinMode (LED, OUTPUT) ;

    // create_device(SINGLE_LIGHT, "test_light", 3);
    
    uv_run(loop, UV_RUN_DEFAULT);

    restapi_exit();

    return 0;
}

