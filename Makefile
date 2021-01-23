CC       = gcc
CFLAGS	 = -Wall -g

TARGET := SnoopyGateway

TOPDIR := .

SOURCEDIR := ${TOPDIR}/src
SOURCE_FILES_C := $(shell find $(SOURCEDIR) -name "*.c")
SOURCE_OBJS := $(SOURCE_FILES_C:.c=.o)

SENSORDIR := ${TOPDIR}/sensor
SENSOR_SOURCE_FILES_C := $(shell find $(SENSORDIR) -name "*.c")
SENSOR_OBJS := $(SENSOR_SOURCE_FILES_C:.c=.o)

DEVICEDIR := ${TOPDIR}/devices
DEVICE_FILES_C := $(shell find $(DEVICEDIR) -name "*.c")
DEVICE_OBJS := $(DEVICE_FILES_C:.c=.o)
DEVICE_HEAD := $(shell find $(DEVICEDIR) -type d)

DEPS_DIR := ${TOPDIR}/deps
DEPS_INCLUDE_DIR := ${DEPS_DIR}/lib/include
DEPS_LIB_DIR := ${DEPS_DIR}/lib/lib

INCLUDE  = -I ${TOPDIR} -I ${SOURCEDIR} -I ${DEPS_INCLUDE_DIR} -I ${SENSORDIR} $(addprefix -I,$(DEVICE_HEAD))

LIBVAR	+= -lwiringPi \
		   -lwiringPiDev\
           -lpthread \
           -lrt  \
		   -lm \
		   -lcrypt \
		   ${DEPS_LIB_DIR}/libuv_a.a \
		   ${DEPS_LIB_DIR}/libzlog.a \
		   ${DEPS_LIB_DIR}/libcjson.a \
		   -ldl

LIBPATH =  -L ${DEPS_LIB_DIR}

PK = a1vaGzoafKg
PS = mHth6DvRcNBNxY5C
DN = raspi
DS = LyJGa9pbnhnZynV4SpZsODT1XypuIxiQ

DOMAIN = iot-as-mqtt.cn-shanghai.aliyuncs.com
DID = -DDEVICE_NAME=\"${DN}\" \
	  -DPRODUCT_KEY=\"${PK}\" \
	  -DDEVICE_SECRET=\"${DS}\" \
	  -DMQTT_DOMAIN=\"${DOMAIN}\"\
	  -DPRODUCT_SECRET=\"${PS}\"

.PHONY:all
all:  $(SOURCE_OBJS) ${SENSOR_OBJS} ${DEVICE_OBJS} 	
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TARGET) $(SOURCE_OBJS) ${SENSOR_OBJS} ${DEVICE_OBJS} ${LIBPATH} ${LIBVAR}

VPATH  = ${SOURCEDIR}  
%.o : %.c
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@  

.PHONY:clean
clean:
	-rm -rf $(TARGET) $(SOURCE_OBJS) ${SENSOR_OBJS} ${DEVICE_OBJS}
