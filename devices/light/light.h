#pragma once

#include "device.h"

device_t * create_light_device(char *name, long sid);
int free_light_device(device_t *device);