#include "device_mgr.h"
#include <stdlib.h>
#include "light/simple_light.h"
#include "uv.h"
#include "log.h"
#include "cc2530.h"

static pthread_mutex_t container_lock;

static void cc2530_recv_callback(cc2530_buf_t buf) {
    cJSON * data = cJSON_ParseWithLength(buf.base, buf.len);
    if (data == NULL || data->type != cJSON_Object) {
        zlog_error(g_log, "data parse error.");
        return;
    }
    
}

device_t *find_device_by_sid(char *sid)
{
    device_t *device = NULL;
    pthread_mutex_lock(&container_lock);
    device = map_get(device_container, sid);
    pthread_mutex_unlock(&container_lock);
    return device;
}

device_t *create_device(model_type_t type, char *name, char *sid)
{
    device_t *device = NULL;

    pthread_mutex_lock(&container_lock);
    if (map_get(device_container, sid) != NULL)
    {
        pthread_mutex_unlock(&container_lock);
        return NULL;
    }
    pthread_mutex_unlock(&container_lock);

    switch (type)
    {
    case SIMPLE_LIGHT:
        device = create_light_device(name, sid);
        break;

    case SIMPLE_SWITCH:

        break;
    default:
        break;
    }

    if (device)
    {
        pthread_mutex_lock(&container_lock);
        map_set(device_container, sid, (void *)device);
        pthread_mutex_unlock(&container_lock);
    }

    return device;
}

int destory_device(char *sid)
{
    pthread_mutex_lock(&container_lock);
    device_t *device = map_get(device_container, sid);
    if (device == NULL)
    {
        pthread_mutex_unlock(&container_lock);
        return 0;
    }
    switch (device->device_type)
    {
    case SIMPLE_LIGHT:
        free_light_device(device);
        map_remove(device_container, sid);
        break;

    default:
        break;
    }

    pthread_mutex_unlock(&container_lock);
}

int report(device_t *device, cJSON *data)
{
    lfqueue_enq(&msg_queue, (void *)data);
}

int device_global_init()
{
    if (pthread_mutex_init(&container_lock, NULL) != 0)
    {
        return -1;
    }

    device_container = map_create();
    lfqueue_init(&msg_queue);

    create_device(SIMPLE_LIGHT, "test", "0x0017880106117ff9");

    if (cc2530_init("/dev/ttyAMA0", 115200) < 0) {
        zlog_error(g_log, "cc2530 init failed.");
        return -1;
    }

    cc2530_on_receive(cc2530_recv_callback);
    return 0;
}

int device_global_exit()
{
    pthread_mutex_destroy(&container_lock);
    map_destroy(device_container);
    lfqueue_destroy(&msg_queue);
    return 0;
}
