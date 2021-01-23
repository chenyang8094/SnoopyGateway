#include <unistd.h>

#include "pcf8591.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"

static int fd ;

/*
 * myAnalogRead:
 *********************************************************************************
 */

int analog_read (int pin)
{
  int x ;

  wiringPiI2CWrite (fd, 0x40 | (pin & 3)) ;

  x = wiringPiI2CRead (fd) ;	// Throw away the first read
  x = wiringPiI2CRead (fd) ;

  return x ;
}

int pcf8591_init (const int address)
{
  if ((fd = wiringPiI2CSetup (address)) < 0)
    return -1 ;

  return 0 ;
}

int pcf8591_exit(){

  return 0;
}
