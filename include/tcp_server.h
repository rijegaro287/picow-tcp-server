#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"


static bool _SERVER_RUNNING = false;
static uint16_t _PORT = 0;
static uint32_t _BUFFER_SIZE = 0;
static err_t (*_PROCESSING_CB)(void *arg, uint32_t buffer_size) = NULL;


typedef struct TCP_SERVER_T_ {
	struct tcp_pcb *server_pcb;
	struct tcp_pcb *client_pcb;
	uint8_t *buffer_sent;
	uint8_t *buffer_recv;
} TCP_SERVER_T;

extern uint32_t tcp_server_connect(const char* ssid, const char* password);
extern TCP_SERVER_T* tcp_server_init(uint16_t port,
																		 uint32_t buffer_size,
																		 err_t (*cb)(void *arg, uint32_t buffer_size));
extern bool tcp_server_open(TCP_SERVER_T *state);
extern void tcp_server_await();
extern err_t tcp_server_close(void *arg);

extern err_t tcp_server_send_data(void *arg);

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_server_err(void *arg, err_t err);
static void* tcp_server_free(TCP_SERVER_T *state);
