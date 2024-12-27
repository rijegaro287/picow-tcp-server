#include <stdio.h>
#include "tcp_server.h"
#include "processing.h"
#include "constants.h"


bool stop_server = false;

int main() {
	stdio_init_all();

	cyw43_arch_init();
	cyw43_arch_enable_sta_mode();

	tcp_server_set_port(TCP_PORT);
	tcp_server_set_buffer_size(BUFFER_SIZE);
	tcp_server_set_processing_cb(process_data);

	TCP_SERVER_T *state = tcp_server_init();
	if(!state) {
		printf("Failed to initialize TCP server\n");
		return PICO_ERROR_INVALID_DATA;
	}

	if(tcp_server_connect(WIFI_SSID, WIFI_PASSWORD) != CYW43_STATUS_SUCCESS) {
		printf("Failed to connect to network\n");
		return PICO_ERROR_CONNECT_FAILED;
	}

	if(!tcp_server_open(state)) {
		printf("Failed to open TCP server\n");
		return PICO_ERROR_CONNECT_FAILED;
	}

	while(!stop_server) {
		busy_wait_ms(500);
	}

	cyw43_arch_deinit();

	return 0;
}
