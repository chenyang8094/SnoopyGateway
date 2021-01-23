
#include "ir.h"
#include <wiringPi.h>
#include <stdio.h>

static int fd;

static char device_conn[] = {0x7E, 0x05, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x80};
static char device_conn_res[] = {0x7E, 0x08, 0x00, 0x6F, 0x38, 0xFF, 0xFF, 0x02, 0x6F, 0x38, 0x01, 0xD5};

static char air_conditioner_on_cold_27_lowwind[] = {0x7E, 0x07, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x14, 0x01, 0x00, 0xE8}; // 1 seq
static char air_conditioner_on_cold_27_lowwind_res[] = {0x7E, 0x08, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x15, 0x01, 0x00, 0x01, 0xEB};
static char air_conditioner_off[] = {0x7E, 0x07, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x14, 0x02, 0x00, 0xE9}; // 2 seq
static char air_conditioner_off_res[] = {0x7E ,0x08 ,0x00 ,0x6F ,0x38 ,0x6F ,0x38 ,0x15 ,0x02 ,0x00 ,0x01 ,0xEC};
static char air_conditioner_on_cold_27_midwind[] = {0x7E, 0x07, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x14, 0x03, 0x00, 0xEA}; // 3 seq
static char air_conditioner_on_cold_27_midwind_res[] = {0x7E, 0x08, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x15, 0x03, 0x00, 0x01, 0xEC};
static char air_conditioner_on_cold_27_highwind[] = {0x7E, 0x07, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x14, 0x04, 0x00, 0xEB}; // 4 seq
static char air_conditioner_on_cold_27_highwind_res[] = {0x7E, 0x08, 0x00, 0x6F, 0x38, 0x6F, 0x38, 0x15, 0x04, 0x00, 0x00, 0xED};


static char air_conditioner_on_hot_30_lowwind[] = {0x7E , 0x07 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x14 , 0x14 , 0x00 , 0xFB}; // 20 seq
static char air_conditioner_on_hot_30_lowwind_res[] = {0x7E , 0x08 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x15 , 0x14 , 0x00 , 0x01 , 0xFE};
static char air_conditioner_on_hot_30_midwind[] = {0x7E , 0x07 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x14 , 0x14 , 0x00 , 0xFB}; // 21 seq
static char air_conditioner_on_hot_30_midwind_res[] = {0x7E , 0x08 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x15 , 0x14 , 0x00 , 0x01 , 0xFE};
static char air_conditioner_on_hot_30_highwind[] = {0x7E , 0x07 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x14 , 0x14 , 0x00 , 0xFB}; // 22 seq
static char air_conditioner_on_hot_30_highwind_res[] = {0x7E , 0x08 , 0x00 , 0x6F , 0x38 , 0x6F , 0x38 , 0x15 , 0x14 , 0x00 , 0x01 , 0xFE};


static int sendCmd(char *cmd, int len)
{
    for (int i = 0; i < len; i++)
    {
        serialPutchar(fd, cmd[i]);
    }

    return 0;
}

static int readResponse(char *res, int expect_len)
{
    int n = 0;
    int escap_cnt = 0;

    do{
        n = serialDataAvail(fd);
        delay(1);
    }while(n > 0 && n != expect_len && escap_cnt++ < 10);

    if(n == expect_len){
        for(int i = 0;i < n;++i){
          res[i] = serialGetchar(fd);
        }
        return 0;
    }

    return -1;
}

int air_conditioner_cold_27_lowwind()
{
    char res_buf[1024];
    int ret;
    sendCmd(air_conditioner_on_cold_27_lowwind, sizeof(air_conditioner_on_cold_27_lowwind));
    ret = readResponse(res_buf, sizeof(air_conditioner_on_cold_27_lowwind_res));
    if (ret || memcmp(res_buf, air_conditioner_on_cold_27_lowwind_res, sizeof(air_conditioner_on_cold_27_lowwind_res)))
    {
        return 1;
    }

    return 0;
}

int air_conditioner_turnoff()
{
    char res_buf[1024];
    int ret;

    sendCmd(air_conditioner_off, sizeof(air_conditioner_off));
    ret = readResponse(res_buf, sizeof(air_conditioner_off_res));
    if (ret || memcmp(res_buf, air_conditioner_off_res, sizeof(air_conditioner_off_res)))
    {
        return 1;
    }

    return 0;
}

int air_conditioner_cold_27_midwind()
{
    char res_buf[1024];
    int ret;
    sendCmd(air_conditioner_on_cold_27_midwind, sizeof(air_conditioner_on_cold_27_midwind));
    ret = readResponse(res_buf, sizeof(air_conditioner_on_cold_27_midwind_res));
    if (ret || memcmp(res_buf, air_conditioner_on_cold_27_midwind_res, sizeof(air_conditioner_on_cold_27_midwind_res)))
    {
        return 1;
    }

    return 0;
}

int air_conditioner_cold_27_highwind()
{
    char res_buf[1024];
    int ret;
    sendCmd(air_conditioner_on_cold_27_highwind, sizeof(air_conditioner_on_cold_27_highwind));
    ret = readResponse(res_buf, sizeof(air_conditioner_on_cold_27_highwind_res));
    if (ret || memcmp(res_buf, air_conditioner_on_cold_27_highwind_res, sizeof(air_conditioner_on_cold_27_highwind_res)))
    {
        return 1;
    }

    return 0;
}

int ir_init(const int address)
{
    char res_buf[1024];
    int res_len;

    if ((fd = serialOpen("/dev/ttyS0", 9600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device\n");
        return 1;
    }

    sendCmd(device_conn, sizeof(device_conn));
    delay(1);
    readResponse(res_buf, sizeof(device_conn_res));
    if (res_len != sizeof(device_conn_res) || memcmp(res_buf, device_conn_res, res_len))
    {
        return 1;
    }

    return 0;
}

int ir_exit()
{
    serialClose(fd);
}