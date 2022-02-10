
#include <stdio.h>
#include <string.h>

#include <button.h>
#include <ipc.h>

#include "morse_button.h"

#define MESSAGE_LEN (10)

bool fst_btn_pushed = false;
bool send_mode_active = false;
bool send_mode_sequence = false;
bool buffer_full = false;
int idx = 0;
char message[MESSAGE_LEN + 1] = { 0 };

char main_buff[64] __attribute__((aligned(64)));
bool main_done = false;
int main_service;

static void reset_state(void)
{
	fst_btn_pushed = false;
	send_mode_active = send_mode_sequence = false;
	buffer_full = false;
	idx = 0;
	memset(message, 0, (MESSAGE_LEN + 1) * sizeof(char));
}

static void button_callback(
	int btn_num, int val, int arg2 __attribute__ ((unused)),
	void *user_data __attribute__ ((unused)) )
{
	// printf("Am intrat in callback: %d %d\n", btn_num, val);
	// if (buffer_full)
	// 	return;

	if (!send_mode_active) {
		
		/* ON PRESS */
		if (val == ON_PRESS) {

			/* Check which button has been pushed */
			/* Left Button -> Initialize send mode sequence */
			if (btn_num == LEFT_BUTTON) {
				fst_btn_pushed = true;

			/* Right button + Left button -> Send mode sequence complete */
			} else if (btn_num == RIGHT_BUTTON && fst_btn_pushed) {
				send_mode_sequence = true;
				printf("Send mode sequence TRUE, now release left + right\n");
			}

		/* ON RELEASE */
		} else {
			if (btn_num == LEFT_BUTTON) {
				fst_btn_pushed = false;
				printf("LEFT button released\n");
			} else if (btn_num == RIGHT_BUTTON && !fst_btn_pushed &&
					   send_mode_sequence) {
				reset_state();
				send_mode_active = true;
				printf("Send mode activated\n");
			}
		}

	} else {


		if (!buffer_full) {
			
			/* ON PRESS -> Check for early ending of message */
			if (!send_mode_sequence && val == ON_PRESS) {
				if (btn_num == LEFT_BUTTON) {
					fst_btn_pushed = true;
				
				} else if (btn_num == RIGHT_BUTTON && fst_btn_pushed) {
					send_mode_sequence = true;
					printf("Send mode sequence TRUE, now release left + right\n");
				}
				return;
			}

			/* ON RELEASE -> Check for send mode sequence complete */
			/* 				or record characters in buffer until full */
			if (send_mode_sequence) {
				if (btn_num == LEFT_BUTTON) {
					fst_btn_pushed = false;
				
				} else if (btn_num == RIGHT_BUTTON && !fst_btn_pushed) {
					/* Early ending -> send message */
					printf("Early ending: %s\n", message);
					
					main_done = false;
					memcpy(main_buff, message, 11);
					ipc_notify_service(main_service);
					yield_for(&main_done);

					/* Reset state machine */
					reset_state();
				}

			} else {
				/* Record pressed buttons in a string on RELEASE */
				/* Button 0 -> "." */
				if (btn_num == LEFT_BUTTON && val == ON_RELEASE) {
					message[idx++] = '.';
					fst_btn_pushed = false;
					printf("Buffer state: %s\n", message);

				/* Button 1 -> "-" */
				} else if (btn_num == RIGHT_BUTTON && val == ON_RELEASE) {
					message[idx++] = '-';
					printf("Buffer state: %s\n", message);
				}

				/* Buffer is full */
				if (idx == MESSAGE_LEN) {
					buffer_full = true;
					printf("Buffer is now full\n");

					/* Buffer full, on DE-PRESS */
					/* Send buffer to morse_chat app */
					printf("%s\n", message);
					
					main_done = false;
					memcpy(main_buff, message, 11);
					ipc_notify_service(main_service);
					yield_for(&main_done);

					/* Reset state machine */
					reset_state();
				}
			}
		} else {
			printf("Buffer is full???\n");
		}
	}
}

static void main_ipc_callback(int pid, int len, int arg2, void *ud)
{
	main_done = true;
}

/* IPC Discover Main Service */
static int setup_main_ipc()
{
	int ret;
	ret = ipc_discover("org.tockos.applications.morse_chat",
				&main_service);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to discover Main Service\n");
		return ret;
	}

	ret = ipc_register_client_callback(main_service, main_ipc_callback, NULL);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to register Main Service callback\n");
		return ret;
	}

	ret = ipc_share(main_service, main_buff, 64);
	if (ret != RETURNCODE_SUCCESS) {
		printf("Unable to share buffer with Main Service\n");
		return ret;
	}

	return 0;
}


int main(void)
{
	int btn_count, ret;
	memset(message, 0, MESSAGE_LEN + 1);

	button_subscribe(button_callback, NULL);
	button_count(&btn_count);
	if (btn_count < 1) {
		printf("Cannot get button count\n");
		return -1;
	}

	/* Discover Main service */
	ret = setup_main_ipc();
	if (ret) {
		printf("Couldn't discover Main Service\n");
		return ret;
	}

	button_enable_interrupt(0);
	button_enable_interrupt(1);

	return 0;
}