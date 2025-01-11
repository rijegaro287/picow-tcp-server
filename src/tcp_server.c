#include "tcp_server.h"


extern TCP_SERVER_T* tcp_server_init(uint16_t port,
																		 uint32_t buffer_size,
																		 err_t (*cb)(void *arg, uint32_t buffer_size)) {
	_PORT = port;
	_BUFFER_SIZE = buffer_size;
	_PROCESSING_CB = cb;
	_SERVER_RUNNING = false;

	if(_BUFFER_SIZE <= 0) {
		printf("Buffer should be greater than 0\n");
		return NULL;
	}

	TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
	if(!state) {
		printf("Failed to allocate state\n");
		return tcp_server_free(state);
	}

	state->buffer_sent = (uint8_t *)malloc(_BUFFER_SIZE);
	if(!state->buffer_sent) {
		printf("Failed to allocate buffer_sent\n");
		return tcp_server_free(state);
	}

	state->buffer_recv = (uint8_t *)malloc(_BUFFER_SIZE);
	if(!state->buffer_recv) {
		printf("Failed to allocate buffer_recv\n");
		return tcp_server_free(state);
	}

	return state;
}

extern uint32_t tcp_server_connect(const char* ssid, const char* password) {
	printf("Attempting to connect to network: %s\n", ssid);
	uint32_t connection_status = CYW43_STATUS_FAIL;
	while(connection_status != CYW43_STATUS_SUCCESS) {
		connection_status = cyw43_arch_wifi_connect_blocking(ssid,
																												 password,
																												 CYW43_AUTH_WPA2_MIXED_PSK);

		switch (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA)) {
			case CYW43_LINK_DOWN:
				printf("Failed to connect to network: WiFi is disabled.\n");
				break;
			case CYW43_LINK_NONET:
				printf("Failed to connect to network: Incorrect ssid.\n");
				break;
			case CYW43_LINK_BADAUTH:
				printf("Failed to connect to network: Incorrect password.\n");
				break;
			case CYW43_LINK_JOIN:
				printf("Succesfully connected to network %s\n", ssid);
				break;
			default:
				printf("Failed to connect to network.\n");
				break;
		}
	}
	return connection_status;
}

extern bool tcp_server_open(TCP_SERVER_T *state) {
  printf("Opening TCP server...\n");
	struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if(!pcb) {
		printf("Failed to create pcb\n");
    tcp_server_close(state);
		return false;
	}

	err_t error = tcp_bind(pcb, NULL, _PORT);
	if(error) {
		printf("Failed to bind to port %u\n", _PORT);
    tcp_server_close(state);
		return false;
	}

	state->server_pcb = tcp_listen_with_backlog(pcb, 1);
	if(!state->server_pcb) {
		printf("Failed to listen\n");
    tcp_server_close(state);
		return false;
	}

	tcp_arg(state->server_pcb, state);
	tcp_accept(state->server_pcb, tcp_server_accept);
	
	ip_addr_t ip_addr = cyw43_state.netif[0].ip_addr;
	printf("====================================================\n");
	printf("Server is running in %s:%d\n", ip4addr_ntoa(&ip_addr), _PORT);
	printf("====================================================\n");

	return true;
}

extern void tcp_server_await() {
	_SERVER_RUNNING = true;
	while(_SERVER_RUNNING) {
		busy_wait_ms(1);
	}
}

extern err_t tcp_server_close(void *arg) {
  printf("Closing TCP server...\n");
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	err_t error = ERR_OK;

	if(state->client_pcb != NULL) {
		tcp_arg(state->client_pcb, NULL);
		tcp_poll(state->client_pcb, NULL, 0);
		tcp_sent(state->client_pcb, NULL);
		tcp_recv(state->client_pcb, NULL);
		tcp_err(state->client_pcb, NULL);
		
		error = tcp_close(state->client_pcb);
		if(error != ERR_OK) {
			printf("Close failed %d, calling abort\n", error);
			tcp_abort(state->client_pcb);
			error = ERR_ABRT;
		}
		state->client_pcb = NULL;
	}

	if(state->server_pcb) {
		tcp_arg(state->server_pcb, NULL);
		tcp_close(state->server_pcb);
		state->server_pcb = NULL;
	}

	tcp_server_free(state);

	_SERVER_RUNNING = false;

	printf("TCP Server closed\n");

	return error;
}

extern err_t tcp_server_send_data(void *arg) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;

	cyw43_arch_lwip_check();
	err_t error = tcp_write(state->client_pcb,
													state->buffer_sent,
													_BUFFER_SIZE,
													TCP_WRITE_FLAG_COPY);

	if(error != ERR_OK) {
		printf("tcp_write failed %d\n", error);
		return tcp_server_close(arg);
	}

	return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t error) {
  printf("TCP server accepted connection. Waiting for a message...\n");
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;

	if(client_pcb == NULL) {
		printf("Client pcb is null\n");
		return ERR_OK;
	}

	if(error != ERR_OK) {
		printf("Failure in accept function\n");
		tcp_server_close(arg);
		return error;
	}

	state->client_pcb = client_pcb;
	tcp_arg(client_pcb, state);
	tcp_poll(client_pcb, tcp_server_poll, 10);
	tcp_recv(client_pcb, tcp_server_recv);
	tcp_sent(client_pcb, tcp_server_sent);
	tcp_err(client_pcb, tcp_server_err);

	return ERR_OK;
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
  return ERR_OK;
}

static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	if(err != ERR_OK) {
		printf("Error in recv function\n");
		return tcp_server_close(arg);
	}

	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if(p == NULL) {
		return ERR_BUF;
	}

	uint32_t buffer_len = p->tot_len;
	cyw43_arch_lwip_check();
	if(buffer_len > 0) {
		pbuf_copy_partial(p, state->buffer_recv, buffer_len, 0);
		tcp_recved(tpcb, buffer_len);
	}
	pbuf_free(p);

	if(strcmp(state->buffer_recv, "exit") == 0) {
		return tcp_server_close(arg);
	}

	/* Execute custom process function */
	err_t error = _PROCESSING_CB(state, _BUFFER_SIZE);

	memset(state->buffer_recv, 0, _BUFFER_SIZE);

	tcp_close(tpcb);

	return error;
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	memset(state->buffer_sent, 0, _BUFFER_SIZE);
	return ERR_OK;
}

static void tcp_server_err(void *arg, err_t error) {
	if(error != ERR_ABRT) {
		printf("TCP Server Error: %x\n", error);
	}
  tcp_server_close(arg);
}

static void* tcp_server_free(TCP_SERVER_T *state) {
	if(state->buffer_sent) {
		free(state->buffer_sent);
		state->buffer_sent = NULL;
	}

	if(state->buffer_recv) {
		free(state->buffer_recv);
		state->buffer_recv = NULL;
	}

	if(state) {
		free(state);
		state = NULL;
	}
	
	return (void*)state;
}
