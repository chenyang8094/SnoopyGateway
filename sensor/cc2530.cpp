#include "cc2530.h"
#include "Log.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>

#define BUF_INIT_LEN 1024
#define FIN_1 0x0d
#define FIN_2 0x0a

#define RECV_STATUS_NONE 0
#define RECV_STATUS_FIN_1 1
#define RECV_STATUS_FIN_2 2

static int fd;
static pthread_t t, t2;
static int stopped = 1;

static recv_callback_t on_receive = NULL;

static void* send_thread(void* arg);

int cc2530_init(const char* uart_dev, int baud)
{
    if ((fd = serialOpen(uart_dev, baud)) < 0)
    {
        // zlog_error("unable to open serial device: %s", strerror(errno));
        return -1;
    }

    stopped = 0;
    pthread_create(&t, NULL, receive_thread, NULL);

    pthread_create(&t2, NULL, send_thread, NULL);
    return 0;
}

char recovery_cmd[] = {0XFD, 0X01, 0X13, 0XFF, 0X00};
static void* send_thread(void* arg)
{
    cc2530_send_string(recovery_cmd);
    sleep(1);
    cc2530_send_string("+++");
    sleep(1);
    cc2530_send_string("AT+RESET");
    sleep(3);
    cc2530_send_string("+++");
    cc2530_send_string("AT+DEV=C");
    cc2530_send_string("AT+PANID=FFFF");

    while (1)
    {
        cc2530_send_string("AT+DEV=?");
        cc2530_send_string("AT+JOINCNT=?");
        cc2530_send_string("AT+GROUP=?");
        cc2530_send_string("AT+PANID=?");
        cc2530_send_string("AT+CH=?");
        cc2530_send_string("AT+NWK=?");
        cc2530_send_string("AT+KEY=11131517191B1D1F10121416181A1C1D");
        cc2530_send_string("AT+KEY=?");

        sleep(3);
    }
}

static void* receive_thread(void* arg)
{
    int recv_status = RECV_STATUS_NONE;
    cc2530_buf_t buf;
    buf.base = (char *)malloc(BUF_INIT_LEN);
    buf.capacity = BUF_INIT_LEN;
    buf.len = 0;
    zlog_info(g_log, "receive_thread started");

    int bd = 0;
    while (!stopped)
    {
        while (serialDataAvail(fd) > 0)
        {
            int ch = serialGetchar(fd);
            if (ch < 0)
            {
                buf.len = 0;
                recv_status = RECV_STATUS_NONE;
                break;
            }
            if (recv_status == RECV_STATUS_NONE)
            {
                if (ch == FIN_1)
                {
                    recv_status = RECV_STATUS_FIN_1;
                }
                else
                {
                    if (buf.len + 1 >= buf.capacity)
                    {
                        buf.capacity *= 2;
                        buf.base = (char *)realloc(buf.base, buf.capacity);
                    }
                    buf.base[buf.len++] = ch;
                }
            }
            else if (recv_status == RECV_STATUS_FIN_1)
            {
                if (ch != FIN_2)
                {
                    recv_status = RECV_STATUS_NONE;
                    buf.len = 0;
                    break;
                }
                else
                {
                    recv_status = RECV_STATUS_FIN_2;
                    break;
                }
            }
        }

        if (recv_status == RECV_STATUS_FIN_2)
        {
            if (on_receive)
            {
                zlog_info(g_log, "%.*s", buf.len, buf.base);
                // on_receive(buf);
            }
            buf.len = 0;
            recv_status = RECV_STATUS_NONE;
        }
        usleep(500);
    }

    free(buf.base);
}

int cc2530_send_buf(cc2530_buf_t buf)
{
    usleep(100000);
    buf.base[buf.len] = 0;
    serialPuts(fd, buf.base);
    serialFlush(fd);
    return 0;
}

int cc2530_send_string(const char* str)
{
    usleep(100000);
    serialPuts(fd, str);
    serialFlush(fd);
    return 0;
}

int cc2530_on_receive(recv_callback_t func)
{
    on_receive = func;
}

int cc2530_exit()
{
    stopped = 1;
    // pthread_join(&t, NULL);
    serialClose(fd);
}