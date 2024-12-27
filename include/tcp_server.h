#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/tcp.h"
#include "lwip/pbuf.h"


static uint16_t _PORT = 0;
static uint32_t _BUFFER_SIZE = 0;
static err_t (*_PROCESSING_CB)(void *arg, uint32_t buffer_size) = NULL;

typedef struct TCP_SERVER_T_ {
	struct tcp_pcb *server_pcb;
	struct tcp_pcb *client_pcb;
	uint8_t *buffer_sent;
	uint8_t *buffer_recv;
} TCP_SERVER_T;

extern bool stop_server;

extern uint32_t tcp_server_connect(const char* ssid, const char* password);
extern TCP_SERVER_T* tcp_server_init(void);
extern bool tcp_server_open(TCP_SERVER_T *state);
extern err_t tcp_server_close(void *arg);
extern void tcp_server_set_port(uint16_t port);
extern void tcp_server_set_buffer_size(uint32_t size);
extern void tcp_server_set_processing_cb(err_t (*cb)(void *arg, uint32_t buffer_size));

extern err_t tcp_server_send_data(void *arg, char* message);

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_server_err(void *arg, err_t err);
static void* tcp_server_free(TCP_SERVER_T *state);
