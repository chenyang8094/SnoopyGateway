#pragma once

#include "device.h"

#define LED 7

device_t * create_light_device(char *name, char *sid);
int free_light_device(device_t *device);