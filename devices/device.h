#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <cjson/cJSON.h>

#define STRING_MAX_LEN 128

#define DEVICE_ID_MAX 88888
#define DEVICE_NAME_MAX_LEN 100

typedef enum MODEL_TYPE {
    /* light */
    SIMPLE_LIGHT,


    /* switch */
    SIMPLE_SWITCH,


} model_type_t;

typedef enum TYPE {
    NUMERIC,
    BINARY,
    SWITCH,
    LOCK,
    ENUM,
    TEXT,
    COMPOSITE,
    LIGHT,
    COVER,
    FAN,
    CLIMATE,
} device_type_t;

// export interface ExposesEntry {
//   type: string;
//   name?: string;
//   endpoint?: string;
//   access?: number;
//   property?: string;
//   values?: string[];
//   value_off?: string | boolean | number;
//   value_on?: string | boolean | number;
//   value_min?: number;
//   value_max?: number;
// }

// export interface DeviceDefinition {
//   vendor: string;
//   model: string;
//   exposes: ExposesEntry[];
// }

// export interface DeviceListEntry {
//   definition?: DeviceDefinition | null;
//   friendly_name: string;
//   sid: string;
//   supported: boolean;
//   software_build_id?: string;
//   date_code?: string;
// }

// export enum ExposesAccessLevel {
//   PUBLISHED = 0x1,
//   SET = 0x2,
//   GET = 0x4,
// }

// export enum ExposesKnownTypes {
//   NUMERIC = 'numeric',
//   BINARY = 'binary',
//   SWITCH = 'switch',
//   LOCK = 'lock',
//   ENUM = 'enum',
//   TEXT = 'text',
//   COMPOSITE = 'composite',
//   LIGHT = 'light',
//   COVER = 'cover',
//   FAN = 'fan',
//   CLIMATE = 'climate',
// }

// typedef struct exposes_entry {
//     enum TYPE type;
//     char * name;
//     char * endpoint;
//     int access;
//     char * property;
//     values?: string[];
//     value_off?: string | boolean | number;
//     value_on?: string | boolean | number;
//     value_min?: number;
//     value_max?: number;
// } exposes_entry_t;

typedef struct device
{
    char *friendly_name;
    char *device_sid;
    model_type_t device_type;

    int (*write)(struct device *device_p, char *data);
    int (*read)(struct device *device_p, char *buf, int len);
    int (*get_register_info)(struct device *device_p, cJSON * info);
}device_t;

device_t* device_container[DEVICE_ID_MAX];


device_t *find_device_by_sid(long sid);
device_t *create_device(model_type_t type, char *name, long sid);
int destory_device(long sid);

int device_global_init();
int device_global_exit();

#ifdef __cplusplus
}
#endif

