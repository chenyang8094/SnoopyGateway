#ifndef _HMC5883L_H_
#define _HMC5883L_H_

#define HCM5883L_REG_CONFIG_A (0x00)
#define HCM5883L_REG_CONFIG_B (0x01)
#define HMC5883L_REG_MODE     (0x09)
#define HMC5883L_REG_MSB_X    (0x01)
#define HMC5883L_REG_LSB_X    (0x00)
#define HMC5883L_REG_MSB_Z    (0x05)
#define HMC5883L_REG_LSB_Z    (0x04)
#define HMC5883L_REG_MSB_Y    (0x03)
#define HMC5883L_REG_LSB_Y    (0x02)
#define HMC5883L_REG_STATUS   (0x09)
#define HMC5883L_REG_ID_A     (0x0a)
#define HMC5883L_REG_ID_B     (0x0b)
#define HMC5883L_REG_ID_C     (0x0c)

#define HMC5883L_MODE_CONTINUOUS (0x1d)
#define HMC5883L_MODE_SINGLE     (0x01)

int hmc5833l_read(int *x,int * y,int * z,double *angle);
int hmc5833l_init(const int address) ;
int hmc5833l_exit();

#endif  