#include <stdio.h>
#include "pico/cyw43_arch.h"

#include "tcp_server.h"
#include "processing.h"
#include "constants.h"


const char *SSID = "Familia Gatgens";
const char *PASSWORD = "adita123";

int main() {
	stdio_init_all();

	cyw43_arch_init();
	cyw43_arch_enable_sta_mode();

	TCP_SERVER_T *state = tcp_server_init(TCP_PORT,
																			  BUFFER_SIZE,
																				process_data);
	if(!state) {
		printf("Failed to initialize TCP server\n");
		return PICO_ERROR_INVALID_DATA;
	}

	if(tcp_server_connect(SSID, PASSWORD) != CYW43_STATUS_SUCCESS) {
		printf("Failed to connect to network\n");
		return PICO_ERROR_CONNECT_FAILED;
	}

	if(!tcp_server_open(state)) {
		printf("Failed to open TCP server\n");
		return PICO_ERROR_CONNECT_FAILED;
	}

	tcp_server_await();

	cyw43_arch_deinit();

	return 0;
}
