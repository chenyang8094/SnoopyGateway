#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "hmc5883l.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>


#define PI 3.141592657

static int fd;

int hmc5833l_read(int *x,int * y,int * z,double *angle){
  uint8_t msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_X);
  uint8_t lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_X);
  *x = msb << 8 | lsb;

  msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_Y);
  lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_Y);
  *y = msb << 8 | lsb;

  msb = wiringPiI2CReadReg8(fd, HMC5883L_REG_MSB_Z);
  lsb = wiringPiI2CReadReg8(fd, HMC5883L_REG_LSB_Z);
  *z = msb << 8 | lsb;

  // *angle = (double)(atan2((double) y, (double)x) * (180 / PI) + 180);
}

int hmc5833l_init(const int address) {
  if ((fd = wiringPiI2CSetup(address)) < 0)
      return -1 ;

  wiringPiI2CWriteReg8(fd, HMC5883L_REG_MODE, HMC5883L_MODE_CONTINUOUS);

  return 0;
}

int hmc5833l_exit(){
   
   return 0;
}