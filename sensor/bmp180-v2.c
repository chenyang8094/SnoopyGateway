#include "bmp180-v2.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"

/* 
 * AC register
 */
#define BMP180_REG_AC1_H 0xAA
#define BMP180_REG_AC2_H 0xAC
#define BMP180_REG_AC3_H 0xAE
#define BMP180_REG_AC4_H 0xB0
#define BMP180_REG_AC5_H 0xB2
#define BMP180_REG_AC6_H 0xB4

/* 
 * B1 register
 */
#define BMP180_REG_B1_H 0xB6

/* 
 * B2 register
 */
#define BMP180_REG_B2_H 0xB8

/* 
 * MB register
 */
#define BMP180_REG_MB_H 0xBA

/* 
 * MC register
 */
#define BMP180_REG_MC_H 0xBC

/* 
 * MD register
 */
#define BMP180_REG_MD_H 0xBE

/* 
 * AC register
 */
#define BMP180_CTRL 0xF4

/* 
 * Temperature register
 */
#define BMP180_REG_TMP 0xF6

/* 
 * Pressure register
 */
#define BMP180_REG_PRE 0xF6

/*
 * Temperature read command
 */
#define BMP180_TMP_READ_CMD 0x2E

/*
 *  Waiting time in us for reading temperature values
 */
#define BMP180_TMP_READ_WAIT_US 5000

/*
 * Pressure oversampling modes
 */
#define BMP180_PRE_OSS0 0 // ultra low power
#define BMP180_PRE_OSS1 1 // standard
#define BMP180_PRE_OSS2 2 // high resolution
#define BMP180_PRE_OSS3 3 // ultra high resoultion

/*
 * Pressure read commands
 */
#define BMP180_PRE_OSS0_CMD 0x34
#define BMP180_PRE_OSS1_CMD 0x74
#define BMP180_PRE_OSS2_CMD 0xB4
#define BMP180_PRE_OSS3_CMD 0xF4

/* 
 * Waiting times in us for reading pressure values
 */
#define BMP180_PRE_OSS0_WAIT_US 5000
#define BMP180_PRE_OSS1_WAIT_US 8000
#define BMP180_PRE_OSS2_WAIT_US 14000
#define BMP180_PRE_OSS3_WAIT_US 26000

/*
 * Average sea-level pressure in hPa
 */
#define BMP180_SEA_LEVEL 1013.25

//#define __BMP180_DEBUG__
#ifdef __BMP180_DEBUG__
#define DEBUG(...)	printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

static void * global_bmp;

/*
 * Lookup table for BMP180 register addresses
 */
static int32_t bmp180_register_table[11][2] = {
		{BMP180_REG_AC1_H, 1},
		{BMP180_REG_AC2_H, 1},
		{BMP180_REG_AC3_H, 1},
		{BMP180_REG_AC4_H, 0},
		{BMP180_REG_AC5_H, 0},
		{BMP180_REG_AC6_H, 0},
		{BMP180_REG_B1_H, 1},
		{BMP180_REG_B2_H, 1},
		{BMP180_REG_MB_H, 1},
		{BMP180_REG_MC_H, 1},
		{BMP180_REG_MD_H, 1}
};


/*
 * Prototypes for helper functions
 */
static void bmp180_read_eprom_reg(int32_t *_data, uint8_t reg, int32_t sign);
static void bmp180_read_eprom();
static int32_t bmp180_read_raw_pressure(uint8_t oss);
static int32_t bmp180_read_raw_temperature();
static void bmp180_init_error_cleanup();

/*
 * Frees allocated memory in the init function.
 * 
 * @param bmp180 sensor
 */
static void bmp180_init_error_cleanup() {
	bmp180_t* bmp = TO_BMP(global_bmp);
	free(global_bmp);
	global_bmp = NULL;
}

/*
 * Reads a single calibration coefficient from the BMP180 eprom.
 * 
 * @param bmp180 sensor
 */
static void bmp180_read_eprom_reg(int32_t *_store, uint8_t reg, int32_t sign) {
	bmp180_t *bmp = TO_BMP(global_bmp);
    int32_t data = wiringPiI2CReadReg16(bmp->fd, reg) & 0xFFFF;
	
	// i2c_smbus_read_word_data assumes little endian 
	// but ARM uses big endian. Thus the ordering of the bytes is reversed.
	// data = 	 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15   bit position
	//          |      lsb      |          msb        |  
	
	//                 msb           +     lsb
	*_store = ((data << 8) & 0xFF00) + (data >> 8);
	
	if(sign && (*_store > 32767)) {
		*_store -= 65536;
	}
}


/*
 * Reads the eprom of this BMP180 sensor.
 * 
 * @param bmp180 sensor
 */
static void bmp180_read_eprom() {
	bmp180_t *bmp = TO_BMP(global_bmp);	
	
	int32_t *bmp180_register_addr[11] = {
		&bmp->ac1, &bmp->ac2, &bmp->ac3, &bmp->ac4, &bmp->ac5, &bmp->ac6,
		&bmp->b1, &bmp->b2, &bmp->mb, &bmp->mc, &bmp->md
	};
	
	uint8_t sign, reg;
	int32_t *data;
	int i;
	for(i = 0; i < 11; i++) {
		reg = (uint8_t) bmp180_register_table[i][0];
		sign = (uint8_t) bmp180_register_table[i][1];
		data = bmp180_register_addr[i];
		bmp180_read_eprom_reg(data, reg, sign);
	}
}

/*
 * Returns the raw measured temperature value of this BMP180 sensor.
 * 
 * @param bmp180 sensor
 */
static int32_t bmp180_read_raw_temperature() {
	bmp180_t* bmp = TO_BMP(global_bmp);
    wiringPiI2CWriteReg8(bmp->fd,BMP180_CTRL,BMP180_TMP_READ_CMD);

	usleep(BMP180_TMP_READ_WAIT_US);
    int32_t data = wiringPiI2CReadReg16(bmp->fd, BMP180_REG_TMP) & 0xFFFF;
	
	data = ((data << 8) & 0xFF00) + (data >> 8);
	
	return data;
}


/*
 * Returns the raw measured pressure value of this BMP180 sensor.
 * 
 * @param bmp180 sensor
 */
static int32_t bmp180_read_raw_pressure(uint8_t oss) {
	bmp180_t* bmp = TO_BMP(global_bmp);
	uint16_t wait;
	uint8_t cmd;
	
	switch(oss) {
		case BMP180_PRE_OSS1:
			wait = BMP180_PRE_OSS1_WAIT_US; cmd = BMP180_PRE_OSS1_CMD;
			break;
		
		case BMP180_PRE_OSS2:
			wait = BMP180_PRE_OSS2_WAIT_US; cmd = BMP180_PRE_OSS2_CMD;
			break;
		
		case BMP180_PRE_OSS3:
			wait = BMP180_PRE_OSS3_WAIT_US; cmd = BMP180_PRE_OSS3_CMD;
			break;
		
		case BMP180_PRE_OSS0:
		default:
			wait = BMP180_PRE_OSS0_WAIT_US; cmd = BMP180_PRE_OSS0_CMD;
			break;
	}
	
    wiringPiI2CWriteReg8(bmp->fd, BMP180_CTRL, cmd);

	usleep(wait);
	
	int32_t msb, lsb, xlsb, data;
    msb = wiringPiI2CReadReg8(bmp->fd, BMP180_REG_PRE) & 0xFF;
    lsb = wiringPiI2CReadReg8(bmp->fd, BMP180_REG_PRE+1) & 0xFF;
    xlsb = wiringPiI2CReadReg8(bmp->fd, BMP180_REG_PRE+2) & 0xFF;
	
	data = ((msb << 16)  + (lsb << 8)  + xlsb) >> (8 - bmp->oss);
	
	return data;
}

/*
 * Implementation of the interface functions
 */

/**
 * Dumps the eprom values of this BMP180 sensor.
 * 
 * @param bmp180 sensor
 * @param bmp180 eprom struct
 */
void bmp180_v2_dump_eprom(bmp180_eprom_t *eprom) {
	bmp180_t *bmp = TO_BMP(global_bmp);
	eprom->ac1 = bmp->ac1;
	eprom->ac2 = bmp->ac2;
	eprom->ac3 = bmp->ac3;
	eprom->ac4 = bmp->ac4;
	eprom->ac5 = bmp->ac5;
	eprom->ac6 = bmp->ac6;
	eprom->b1 = bmp->b1;
	eprom->b2 = bmp->b2;
	eprom->mb = bmp->mb;
	eprom->mc = bmp->mc;
	eprom->md = bmp->md;
}


/**
 * Creates a BMP180 sensor object.
 *
 * @param i2c device address
 * @param i2c device file path
 * @return bmp180 sensor
 */
int bmp180_v2_init(const int address) {
	void *_bmp = malloc(sizeof(bmp180_t));
	if(_bmp == NULL)  {
		DEBUG("error: malloc returns NULL pointer\n");
		return -1;
	}

	global_bmp = _bmp;

	bmp180_t *bmp = TO_BMP(_bmp);
	bmp->address = address;

	// open i2c device
    int fd ;
    if ((fd = wiringPiI2CSetup (address)) < 0)
        return -1 ;

	bmp->fd = fd;

	// setup i2c device
	bmp180_read_eprom( );
	bmp->oss = 0;
	
	return 0;
}


/**
 * Closes a BMP180 object.
 * 
 * @param bmp180 sensor
 */
int bmp180_v2_exit( ) {
	if(global_bmp == NULL) {
		return 0;
	}
	bmp180_t *bmp = TO_BMP(global_bmp);	
	free(global_bmp); // free bmp structure
	global_bmp = NULL;
} 


/**
 * Returns the measured temperature in celsius.
 * 
 * @param bmp180 sensor
 * @return temperature
 */
float bmp180_v2_temperature( ) {
	bmp180_t* bmp = TO_BMP(global_bmp);
	long UT, X1, X2, B5;
	float T;
	
	UT = bmp180_read_raw_temperature();
	
	DEBUG("UT=%lu\n",UT);
	
	X1 = ((UT - bmp->ac6) * bmp->ac5) >> 15;
	X2 = (bmp->mc << 11) / (X1 + bmp->md);
	B5 = X1 + X2;
	T = ((B5 + 8) >> 4) / 10.0;
	
	return T;
}


/**
 * Returns the measured pressure in pascal.
 * 
 * @param bmp180 sensor
 * @return pressure
 */
long bmp180_v2_pressure( ) {
	bmp180_t* bmp = TO_BMP(global_bmp);
	long UT, UP, B6, B5, X1, X2, X3, B3, p;
	unsigned long B4, B7;
	
	UT = bmp180_read_raw_temperature();
	UP = bmp180_read_raw_pressure(bmp->oss);
	
	X1 = ((UT - bmp->ac6) * bmp->ac5) >> 15;
	X2 = (bmp->mc << 11) / (X1 + bmp->md);
	
	B5 = X1 + X2;
	
	B6 = B5 - 4000;
	
	X1 = (bmp->b2 * (B6 * B6) >> 12) >> 11;
	X2 = (bmp->ac2 * B6) >> 11;
	X3 = X1 + X2;
	
	B3 = ((((bmp->ac1 * 4) + X3) << bmp->oss) + 2) / 4;
	X1 = (bmp->ac3 * B6) >> 13;
	X2 = (bmp->b1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	
	
	B4 = bmp->ac4 * (unsigned long)(X3 + 32768) >> 15;
	B7 = ((unsigned long) UP - B3) * (50000 >> bmp->oss);
	
	if(B7 < 0x80000000) {
		p = (B7 * 2) / B4;
	} else {
		p = (B7 / B4) * 2;
	}
	
	X1 = (p >> 8) * (p >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * p) >> 16;
	p = p + ((X1 + X2 + 3791) >> 4);
	
	return p;
}


/**
 * Returns altitude in meters based on the measured pressure 
 * and temperature of this sensor.
 * 
 * @param bmp180 sensor
 * @return altitude
 */
float bmp180_v2_altitude( ) {
	float p, alt;
	p = bmp180_pressure(global_bmp);
	alt = 44330 * (1 - pow(( (p/100) / BMP180_SEA_LEVEL),1/5.255));
	return alt;
}


/**
 * Sets the oversampling setting for this sensor.
 * 
 * @param bmp180 sensor
 * @param oversampling mode
 */
void bmp180_v2_set_oss(int oss) {
	bmp180_t* bmp = TO_BMP(global_bmp);
	bmp->oss = oss;
}