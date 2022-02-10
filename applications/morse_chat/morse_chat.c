/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tock.h"
#include "network.h"
#include "buzzer.h"
#include "timer.h"
#include "ipc.h"

#include "morse_chat.h"


#define USERS_URL "http://3.69.223.241:8000/users"
#define MESSAGES_URL "http://3.69.223.241:8000/messages"

char display_buff[64] __attribute__((aligned(64)));
bool display_done = false;
int user_id;

/*
 * Function to parse received serial morse data
 * Returns the data parsed into a struct message_t "msg"
 * Sets "num_messages"
 */

struct message_t *parse_data(char *data, int *num_messages)
{
	char *messages = data + 2;
	struct message_t *msg;

	/* Get number of messages from buffer */
	sscanf(data, "%d", num_messages);

	/* Alloc struct for parsing data */
	msg = calloc(*num_messages, sizeof(struct message_t));
	for (int i = 0; i < *num_messages; i++)
	{
		msg[i].morse = calloc(MORSE_MAX_LEN, sizeof(char));
	}

	// char *messages = calloc(128, sizeof(char));

	printf("Avem de citit %d mesaje!\n", *num_messages);
	printf("Mesajele de citit: %s\n", messages);

	for (int i = 0; i < *num_messages; i++)
	{
		sscanf(messages, "%d,%s", &(msg[i].uid), msg[i].morse);
		printf("%d,%s\n", msg[i].uid, msg[i].morse);
		messages += strlen(msg[i].morse) + 2;
	}

	return msg;
}

static void display_ipc_callback(int pid, int len, int arg2, void *ud)
{
	display_done = true;
}

/* IPC Discover Display Service */
static int setup_display_ipc(int *display_service)
{
	int ret;
	ret = ipc_discover("org.tockos.applications.morse_display",
				display_service);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to discover Display Service\n");
		return ret;
	}

	ret = ipc_register_client_callback(*display_service, display_ipc_callback, NULL);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to register Display Service callback\n");
		return ret;
	}

	ret = ipc_share(*display_service, display_buff, 64);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to share buffer with Display Service\n");
		return ret;
	}

	return 0;
}

int register_user()
{
	int user_id;
	char *data = network_get(USERS_URL, 1024);
    if (!data) {
        printf("Couldn't receive data\n");
        return -2;
    }

	sscanf(data, "%d", &user_id);

	free(data);

	return user_id;
}

char *get_messages(int user_id)
{
	char *endpoint = calloc(128, sizeof(char));
	sprintf(endpoint, "%s/%d", MESSAGES_URL, user_id);
	char *data = network_get(endpoint, 1024);
	if (!data) {
		printf("Couldn't receive data\n");
		return NULL;
	}

	free(endpoint);

	return data;
}

static void main_ipc_callback(int pid, int len, int buf,
					__attribute__ ((unused)) void* ud)
{

	/* Get message from Button Service */
	char *buffer = (char *) buf;
	if (!len) {
		printf("Invalid length on received from button\n");
		return;
	}
	char *payload = calloc(128, sizeof(char));
	char *url = calloc(128, sizeof(char));
	
	/* Add sender ID to it */
	// snprintf(url, 128, NETWORK_POST "/%d", sender_ID);
	snprintf(payload, 128, "{\"uid\": \"%d\", \"message\": \"%s\"}", sender_ID, buffer);

	/* Do a network POST with it */
	network_post(NETWORK_POST, payload);

	/* Let the Button service know we're done */
	ipc_notify_client(pid);

	free(payload);
}

int main(void)
{
	struct message_t *msg;
	int ret;
	int display_service;
	int num_messages;

	if (!driver_exists(DRIVER_NUM_NETWORK)) {
		printf("No network driver\n");
		return -1;
	}

  /* Act as client towards morse_display service */
	ret = setup_display_ipc(&display_service);
	if (ret < 0) {
		printf("Couldn't setup Display Service\n");
		return ret;
	}

  /* Act as server towards morse_button service */
	ipc_register_service_callback(main_ipc_callback, NULL);

	user_id = register_user();

	while (1) {
		char *messsage_buffer = get_messages(user_id);
		if (!messsage_buffer) {
			printf("Couldn't receive data\n");
			return -2;
		}

		printf("Mesaj de la server: %s\n", messsage_buffer);

		msg = parse_data(messsage_buffer, &num_messages);


		/* For each received message */
		for (int i = 0; i < num_messages; i++)
		{
			display_done = false;
				
			/* Call the display service */
			snprintf(display_buff, 64, "%d %s", msg[i].uid, msg[i].morse);
			ret = ipc_notify_service(display_service);
			yield_for(&display_done);
      delay_ms(1000);
		}

		free(messsage_buffer);

		delay_ms(2000);
	}

	return 0;
}
