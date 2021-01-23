#ifndef _BMP180_H_
#define _BMP180_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

int bmp180_init (const int address);
int bmp180_exit();
float bmp180_temperature();
float bmp180_pressure();
float bmp180_altitude();

#ifdef __cplusplus
}
#endif
#endif
