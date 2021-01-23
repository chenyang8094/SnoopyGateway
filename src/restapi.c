#include "restapi.h"
#define HTTPSERVER_IMPL
#include "http.h"

#include <pthread.h>
#include "device.h"

static pthread_t g_rest_process_thread;

static int request_target_is(struct http_request_s* request, char const * target) {
  http_string_t url = http_request_target(request);
  int len = strlen(target);
  return len == url.len && memcmp(url.buf, target, url.len) == 0;
}

void handle_request(struct http_request_s* request) {
  http_request_connection(request, HTTP_AUTOMATIC);
  struct http_response_s* response = http_response_init();
  if (request_target_is(request, "/control")) {
   
    
    http_response_header(response, "Content-Type", "text/plain");
    char * res = "{\"status\": \"OK\"}";
    http_response_body(response, res, strlen(res));
  }

resp:
  http_respond(request, response);
}

void *rest_service(void *args) {
  http_server_listen((struct http_server_s*)args);
}

int restapi_init() {
  struct http_server_s* server = http_server_init(8080, handle_request);
  int res = pthread_create(&g_rest_process_thread, NULL, rest_service, server);
  if (res < 0) {
    printf("pthread_create http_server_listen failed: %d\n", res);
    return -1;
  }
  return 0;
}

int restapi_exit() {

}
