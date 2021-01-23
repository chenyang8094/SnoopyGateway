#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <cjson/cJSON.h>
#include "map.h"
#include "queue.h"

#define STRING_MAX_LEN 128

#define DEVICE_ID_MAX 88888
#define DEVICE_NAME_MAX_LEN 100

typedef enum MODEL_TYPE {
    /* light */
    SIMPLE_LIGHT,


    /* switch */
    SIMPLE_SWITCH,


} model_type_t;

typedef struct device
{
    char *friendly_name;
    char *device_sid;
    model_type_t device_type;

    int (*set)(struct device *device_p, cJSON *data);
    int (*get)(struct device *device_p, cJSON *data);
    int (*get_register_info)(struct device *device_p, cJSON * info);
} device_t;

map device_container;
lfqueue_t msg_queue;

device_t *find_device_by_sid(char *sid);
device_t *create_device(model_type_t type, char *name, char *sid);
int destory_device(char *sid);

int report(device_t * device, cJSON *data);

int device_global_init();
int device_global_exit();

#ifdef __cplusplus
}
#endif

