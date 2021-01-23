#include "bmp180.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"

#undef	DEBUG

#define	BMP180_OSS	   0
/*
 * Average sea-level pressure in hPa
 */
#define BMP180_SEA_LEVEL 1013.25

static int fd ;

// Static calibration data
//	The down-side of this is that there can only be one BMP180 in
//	a system - which is practice isn't an issue as it's I2C
//	address is fixed.

static  int16_t AC1, AC2, AC3 ;
static uint16_t AC4, AC5, AC6 ;
static  int16_t VB1, VB2 ;
static  int16_t  MB,  MC, MD ;

static double c5, c6, mc, md, x0, x1, x2, yy0, yy1, yy2, p0, p1, p2 ;

/*
 * read16:
 *	Quick hack to read the 16-bit data with the correct endian
 *********************************************************************************
 */

uint16_t read16 (int fd, int reg)
{
  return (wiringPiI2CReadReg8 (fd, reg) <<  8) | wiringPiI2CReadReg8 (fd, reg + 1) ;
}

float bmp180_temperature(){
  float fTemp;
  float tu, a ;
  float pu, s, x, y, z ;

  uint8_t data [4] ;

// Start a temperature sensor reading

  wiringPiI2CWriteReg8 (fd, 0xF4, 0x2E) ;
  delay (5) ;

// Read the raw data

  data [0] = wiringPiI2CReadReg8 (fd, 0xF6) ;
  data [1] = wiringPiI2CReadReg8 (fd, 0xF7) ;

// And calculate...

  tu = (data [0] * 256.0) + data [1] ;

  a = c5 * (tu - c6) ;
  fTemp = a + (mc / (a + md)) ;
  return fTemp;
}

float bmp180_pressure(){
  float fTemp, fPress ;
  float tu, a ;
  float pu, s, x, y, z ;

  uint8_t data [4] ;

  fTemp = bmp180_temperature();

 // Start a pressure snsor reading

  wiringPiI2CWriteReg8 (fd, 0xF4, 0x34 | (BMP180_OSS << 6)) ;
  delay (5) ;

 // Read the raw data

  data [0] = wiringPiI2CReadReg8 (fd, 0xF6) ;
  data [1] = wiringPiI2CReadReg8 (fd, 0xF7) ;
  data [2] = wiringPiI2CReadReg8 (fd, 0xF8) ;

 // And calculate...

  pu = ((double)data [0] * 256.0) + (double)data [1] + ((double)data [2] / 256.0) ;
  s = fTemp - 25.0 ;
  x = (x2 * pow (s, 2.0)) + (x1 * s) + x0 ;
  y = (yy2 * pow (s, 2.0)) + (yy1 * s) + yy0 ;
  z = (pu - x) / y ;
  fPress = (p2 * pow (z, 2.0)) + (p1 * z) + p0 ;
  return fPress;
}

float bmp180_altitude( ){
	float p, alt;
	p = bmp180_pressure( );
	alt = 44330 * (1 - pow(( (p/100) / BMP180_SEA_LEVEL),1/5.255));
	return alt;
}

/*
 * bmp180_init:
 *	Create a new instance of a PCF8591 I2C GPIO interface. We know it
 *	has 4 pins, (4 analog inputs and 1 analog output which we'll shadow
 *	input 0) so all we need to know here is the I2C address and the
 *	user-defined pin base.
 *********************************************************************************
 */

int bmp180_init (const int address)
{
  double c3, c4, b1 ;
 
  if ((fd = wiringPiI2CSetup (address)) < 0)
    return -1 ;

// Read calibration data
  AC1 = read16 (fd, 0xAA) ;
  AC2 = read16 (fd, 0xAC) ;
  AC3 = read16 (fd, 0xAE) ;
  AC4 = read16 (fd, 0xB0) ;
  AC5 = read16 (fd, 0xB2) ;
  AC6 = read16 (fd, 0xB4) ;
  VB1 = read16 (fd, 0xB6) ;
  VB2 = read16 (fd, 0xB8) ;
   MB = read16 (fd, 0xBA) ;
   MC = read16 (fd, 0xBC) ;
   MD = read16 (fd, 0xBE) ;

// Calculate coefficients

  c3 = 160.0 * pow (2.0, -15.0) * AC3 ;
  c4 = pow (10.0, -3.0) * pow(2.0,-15.0) * AC4 ;
  b1 = pow (160.0, 2.0) * pow(2.0,-30.0) * VB1 ;
  c5 = (pow (2.0, -15.0) / 160.0) * AC5 ;
  c6 = AC6 ;
  mc = (pow (2.0, 11.0) / pow(160.0,2.0)) * MC ;
  md = MD / 160.0 ;
  x0 = AC1 ;
  x1 = 160.0 * pow (2.0, -13.0) * AC2 ;
  x2 = pow (160.0, 2.0) * pow(2.0,-25.0) * VB2 ;
  yy0 = c4 * pow (2.0, 15.0) ;
  yy1 = c4 * c3 ;
  yy2 = c4 * b1 ;
  p0 = (3791.0 - 8.0) / 1600.0 ;
  p1 = 1.0 - 7357.0 * pow (2.0, -20.0) ;
  p2 = 3038.0 * 100.0 * pow (2.0,  -36.0) ;

  return 0 ;
}

int bmp180_exit(){

  return 0;
}
