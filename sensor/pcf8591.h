#ifndef _PCF8591_H_
#define _PCF8591_H_
#ifdef __cplusplus
extern "C" {
#endif

int pcf8591_init (const int address) ;
int pcf8591_exit();
int analog_read (int pin);

#ifdef __cplusplus
}
#endif
#endif
