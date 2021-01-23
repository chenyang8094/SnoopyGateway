#ifndef __BMP180_V2_H__
#define __BMP180_V2_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

/*
 * pressure oversampling modes
 */
#define BMP180_PRE_OSS0 0 // ultra low power
#define BMP180_PRE_OSS1 1 // standard
#define BMP180_PRE_OSS2 2 // high resolution
#define BMP180_PRE_OSS3 3 // ultra high resoultion

typedef struct {
	/* Eprom values */
	int ac1;
	int ac2;
	int ac3;
	int ac4;
	int ac5;
	int ac6;
	int b1;
	int b2;
	int mb;
	int mc;
	int md;
} bmp180_eprom_t;

/*
 * Shortcut to cast void pointer to a bmp180_t pointer
 */
#define TO_BMP(x)	(bmp180_t*) x

/*
 * Basic structure for the bmp180 sensor
 */
typedef struct {
	/* file descriptor */
	int fd;

	/* i2c device address */
	int address;
	
	/* BMP180 oversampling mode */
	int oss;
		
	/* Eprom values */
	int32_t ac1;
	int32_t ac2;
	int32_t ac3;
	int32_t ac4;
	int32_t ac5;
	int32_t ac6;
	int32_t b1;
	int32_t b2;
	int32_t mb;
	int32_t mc;
	int32_t md;
} bmp180_t;

int bmp180_v2_init(const int address);
int bmp180_v2_exit( );
long bmp180_v2_pressure( );
void bmp180_v2_set_oss(int oss);
float bmp180_v2_temperature( );
float bmp180_v2_altitude( );
void bmp180_v2_dump_eprom(bmp180_eprom_t *eprom);

#endif