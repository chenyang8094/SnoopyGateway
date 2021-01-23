#include "device.h"
#include <stdlib.h>
#include "light/light.h"
#include "uv.h"

static pthread_mutex_t container_lock;

device_t *find_device_by_sid(long sid) {
    device_t * device;
    pthread_mutex_lock(&container_lock);
    device = device_container[sid];
    pthread_mutex_unlock(&container_lock);
    return device;
}

device_t *create_device(model_type_t type, char *name, long sid) {
    device_t *device = NULL;

    if (name == NULL || strlen(name) > DEVICE_NAME_MAX_LEN) {
        // TODO: log
        return NULL;
    }

    if (sid > DEVICE_ID_MAX) {
        return NULL;
    }

    pthread_mutex_lock(&container_lock);
    if (device_container[sid] != NULL) {
        pthread_mutex_unlock(&container_lock);
        return NULL;
    }
    pthread_mutex_unlock(&container_lock);

    switch (type)
    {
    case SIMPLE_LIGHT:
        device = create_light_device(name, sid);
        break;
    
    default:
        break;
    }

    if (device) {
        pthread_mutex_lock(&container_lock);
        device_container[sid] = device;
        pthread_mutex_unlock(&container_lock);
    }

    return device;
}

int destory_device(long sid) {
    pthread_mutex_lock(&container_lock);
    if (device_container[sid] == NULL) {
        pthread_mutex_unlock(&container_lock);
        return 0;
    }
    switch (device_container[sid]->device_type)
    {
    case SIMPLE_LIGHT:
        free_light_device(device_container[sid]);
        device_container[sid] = NULL;
        break;
    
    default:
        break;
    }

    pthread_mutex_unlock(&container_lock);
}

int device_global_init() {
    if (pthread_mutex_init(&container_lock, NULL) != 0){
        return -1;
    }
    return 0;
}

int device_global_exit() {
    pthread_mutex_destroy(&container_lock);
    memset(device_container, 0, DEVICE_ID_MAX);
    return 0;
}
