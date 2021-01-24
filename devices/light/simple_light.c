#include "simple_light.h"

#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

#include "log.h"

int light_set(device_t *device_p, cJSON *data)
{
  cJSON *state = cJSON_GetObjectItem(data, "state");
  if (state && state->type == cJSON_String)
  {
    if (!memcmp(state->valuestring, "ON", strlen("ON")))
    {
      zlog_info(g_log, "LEN ON");
      digitalWrite(LED, HIGH);
    }
    else
    {
      zlog_info(g_log, "LEN OFF");
      digitalWrite(LED, LOW);
    }
  }
}

int light_get(device_t *device_p, cJSON *data)
{
}

int light_register_info(device_t *device_p, cJSON *info)
{
  // cJSON * light_obj = cJSON_CreateObject();
  // cJSON_AddStringToObject(light_obj, "date_code", "20200706");
  // cJSON * definition = cJSON_AddObjectToObject(report, "definition");
  // cJSON_AddStringToObject(definition, "description", "Hue White and color ambiance Play Lightbar");
  // cJSON * exposes = cJSON_AddArrayToObject(definition, "exposes")

  // cJSON * state = cJSON_AddObjectToObject(report, "state");
  // cJSON_AddNumberToObject(state, "battery", battery++);

  //   cJSON_AddItemToArray();
}

device_t *create_light_device(char *name, char *sid)
{
  device_t *device = calloc(sizeof(device_t), 1);
  device->friendly_name = calloc(strlen(name) + 1, 1);
  memcpy(device->friendly_name, name, strlen(name));
  device->device_sid = calloc(strlen(sid) + 1, 1);
  memcpy(device->device_sid, sid, strlen(sid));
  device->device_type = SIMPLE_LIGHT;
  device->get = light_get;
  device->set = light_set;

  pinMode(LED, OUTPUT);

  return device;
}

int free_light_device(device_t *device)
{
  free(device->friendly_name);
  free(device->device_sid);
  free(device);
}