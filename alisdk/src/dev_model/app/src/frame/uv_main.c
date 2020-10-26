#if USE_LIBUV
#include "uv_main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <uv.h>

#include "local_send.h"
#include "local_callback.h"
#include "cloud_receive.h"

static uv_tcp_t *tcp_client;
static uv_connect_t *g_connect;
static uv_write_t writereq;
static uv_timer_t timer_client;
static uv_signal_t g_signal;
static uv_timer_t timer_req;
static volatile int s_isConnect = 0;
static uv_buf_t w_buf;

#define RECVLEN 16384
char tcpBuf[RECVLEN + 1];

static void client_timer();

static void client_close(uv_handle_t *handle)
{
    printf("client_close\n");
    s_isConnect = 0;
    uv_tcp_init(uv_default_loop(), tcp_client);
    client_timer();
}

static void client_write_cb(uv_write_t *req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "client write error %s\n", uv_strerror(status));
        uv_close((uv_handle_t *)req->handle, client_close);
        return;
    }
    printf("client write succeed!\n");
}

static int client_write(char *data, unsigned int len)
{
    if (!s_isConnect)
    {
        fprintf(stderr, "client not connect\n");
        return -1;
    }
    w_buf = uv_buf_init(data, len);

    printf("w_buf:%p,%d\n", data, len);
    uv_write(&writereq, (uv_stream_t *)tcp_client, &w_buf, 1, client_write_cb);
    return writereq.error;
}

static void client_alloc_buf(uv_handle_t *handle,
                             size_t suggested_size,
                             uv_buf_t *buf)
{
    // *buf = uv_buf_init((char *)malloc(suggested_size), suggested_size);
    *buf = uv_buf_init(tcpBuf, RECVLEN);
}

static void client_read(uv_stream_t *stream,
                        ssize_t nread,
                        const uv_buf_t *buf)
{
    if (nread < 0)
    {
        // UV_EINTR
        // UV_ECONNRESET

        fprintf(stderr, "client read error: %s\n", uv_strerror(nread));
        uv_read_stop(stream);
        uv_close((uv_handle_t *)stream, client_close);

        // if (buf->base)
        //     free(buf->base);
        return;
    }

    buf->base[nread] = 0;
    char *tcpBuf = buf->base;
    // printf("recv %d\n", nread);
    // printf("%s\n", buf->base);
    recv_toLocal(buf->base, nread);
    // if (buf->base)
    //     free(buf->base);
}

static void on_connect(uv_connect_t *req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "connect error %s\n", uv_strerror(status));
        // error!
        return;
    }
    uv_timer_stop(&timer_client);
    s_isConnect = 1;
    write_hanyar_cmd(STR_ADD, NULL, STR_NET_CLOSE);
    write_hanyar_cmd(STR_DEVSINFO, NULL, NULL);

    struct sockaddr addrpeer;
    struct sockaddr_in addrinpeer;
    int addrlenpeer = sizeof(addrpeer);
    char socknamepeer[18] = {0};

    if (0 == uv_tcp_getpeername((uv_tcp_t *)req->handle, &addrpeer, &addrlenpeer))
    {
        addrinpeer = *((struct sockaddr_in *)&addrpeer);
        uv_ip4_name(&addrinpeer, socknamepeer, addrlenpeer);
        printf("connect to:%s:%d\n", socknamepeer, ntohs(addrinpeer.sin_port));
    }
    else
        printf("get socket fail!\n");
    //-----------------------------------------------
    uv_read_start(req->handle, client_alloc_buf, client_read);
}

static void client_timer_callback(uv_timer_t *timer)
{
    // printf("client_timer_callback\n");

    struct sockaddr_in dest;
    uv_ip4_addr("127.0.0.1", SERVER_PORT, &dest);

    uv_tcp_connect(g_connect, tcp_client, (struct sockaddr *)&dest, on_connect);

    // uv_timer_stop(timer);
}
//------------------------------------------

static void client_timer()
{
    uv_timer_again(&timer_client);
}
static int net_client_open()
{

    tcp_client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

    g_connect = (uv_connect_t *)malloc(sizeof(uv_connect_t));

    // struct sockaddr_in dest;
    // uv_ip4_addr("127.0.0.1", SERVER_PORT, &dest);
    // if (uv_tcp_connect(g_connect, tcp_client, (struct sockaddr *)&dest, on_connect) == 0)
    // {
    //     printf("uv_tcp_connect\n");
    // }
    uv_tcp_init(uv_default_loop(), tcp_client);

    uv_timer_init(uv_default_loop(), &timer_client);
    uv_timer_start(&timer_client, client_timer_callback, 100, 2000);
    return 0;
}

static void net_client_close()
{
    uv_read_stop(g_connect->handle);

    uv_close((uv_handle_t *)tcp_client, NULL);
    s_isConnect = 0;
    free(tcp_client);
    free(g_connect);
}
//------------------------------

static void timer_callback(uv_timer_t *timer)
{
    printf("HY_HEART timer_callback\n");
    write_haryan(HY_HEART, strlen(HY_HEART));
}

static int timer_open()
{
    uv_timer_init(uv_default_loop(), &timer_req);
    uv_timer_start(&timer_req, timer_callback, 5000, 60000);
    return 0;
}
static void timer_close()
{
    uv_timer_stop(&timer_req);
}
//---------------------------------------------

static void signal_handler(uv_signal_t *handle, int signum)
{

    if (signum == SIGINT || signum == SIGQUIT || signum == SIGKILL)
    {
        cloud_restart_reFactory(INT_OFFLINE);
    }
    printf("signal received: %d\n", signum);
}

static void signal_open()
{
    uv_signal_init(uv_default_loop(), &g_signal);
    uv_signal_start_oneshot(&g_signal, signal_handler, SIGINT);
    // uv_signal_start(&g_signal, signal_handler, SIGQUIT);
    // uv_signal_start(&g_signal, signal_handler, SIGKILL);
}
static void signal_close()
{
    uv_signal_stop(&g_signal);
}
//--------------------------------------------------
void uv_main_close()
{
    timer_close();
    net_client_close();
    signal_close();
}

void uv_idle_task(uv_idle_t *handle)
{
    // printf("uv_idle_task\n");
}
int uv_main_open()
{
    register_closeCallback(main_close);
    register_writeCallback(client_write);
    signal_open();
    net_client_open();
    timer_open();

    // uv_idle_t idler;
    // uv_idle_init(uv_default_loop(), &idler);
    // uv_idle_start(&idler, uv_idle_task);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    // uv_idle_stop(&idler);
    main_close();
    return 0;
}
#endif