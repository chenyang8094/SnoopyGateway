#include "cc2530.h"
#include "log.h"

#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define BUF_INIT_LEN 1024
#define FIN_1 0x0d
#define FIN_2 0x0a

#define RECV_STATUS_NONE 0
#define RECV_STATUS_FIN_1 1
#define RECV_STATUS_FIN_2 2

static int fd;
static pthread_t t;
static int stopped = 1;

static recv_callback_t on_receive = NULL;

int cc2530_init(const char *uart_dev, int baud)
{
    if ((fd = serialOpen(uart_dev, baud)) < 0)
    {
        zlog_error("unable to open serial device: %s", strerror(errno));
        return 1;
    }

    stopped = 0;
    pthread_create(&t, NULL, receive_thread, NULL);
}

static void *receive_thread(void *arg)
{
    int recv_status = RECV_STATUS_NONE;
    cc2530_buf_t buf;
    buf.base = malloc(BUF_INIT_LEN);
    buf.capacity = BUF_INIT_LEN;
    buf.len = 0;
    while (!stopped)
    {
        while (serialDataAvail(fd))
        {
            int ch = serialGetchar(fd);

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
                        buf.base = realloc(buf.base, buf.capacity);
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
                on_receive(buf);
                buf.len = 0;
            }
        }
    }

    free(buf.base);
}

int cc2530_send(cc2530_buf_t buf)
{
    for (int i = 0; i < buf.len; i++)
    {
        serialPutchar(fd, buf.base[i]);
    }
}

int cc2530_on_receive(recv_callback_t func)
{
    on_receive = func;
}

int cc2530_exit()
{
    stopped = 1;
    pthread_join(&t, NULL);
    serialClose(fd);
}