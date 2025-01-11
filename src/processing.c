#include "processing.h"

extern err_t process_data(void *arg, uint32_t buffer_size) {
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;

	char *message = malloc(buffer_size);
	sprintf(message, "Hello from Pico! You sent this data: \"%s\"", state->buffer_recv);

	printf("%s\n", message);

	err_t error = tcp_server_send_data(state);
	
	free(message);
	message = NULL;

	return error;
}
