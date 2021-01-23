#include "light.h"

#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

static int light_write(device_t *device_p, char *data) {
    // ZIGBEE WRITE
    // if (value) {
      //   digitalWrite(BEEP, HIGH);
      // } else {
      //   digitalWrite(BEEP, LOW);
      // }
}

static int light_read(device_t *device_p, char *buf, int len) {
    // ZIGBEE READ
}


static int light_register_info(device_t *device_p, cJSON * info) {
        // cJSON * light_obj = cJSON_CreateObject();
        // cJSON_AddStringToObject(light_obj, "date_code", "20200706"); 
        // cJSON * definition = cJSON_AddObjectToObject(report, "definition"); 
        // cJSON_AddStringToObject(definition, "description", "Hue White and color ambiance Play Lightbar"); 
        // cJSON * exposes = cJSON_AddArrayToObject(definition, "exposes")
        


        // cJSON * state = cJSON_AddObjectToObject(report, "state");
        // cJSON_AddNumberToObject(state, "battery", battery++);

        //   cJSON_AddItemToArray();
}

#define BEEP 7

device_t * create_light_device(char *name, long sid) {
  device_t * device = calloc(sizeof(device_t), 1);
  memcpy(device->friendly_name, name, strlen(name));
  device->device_sid = sid;
  // device->device_type = "SINGLE_LIGHT";
  device->read = light_read;
  device->write = light_write;

  pinMode(BEEP, OUTPUT);
  return device;
}

int free_light_device(device_t *device) {

}