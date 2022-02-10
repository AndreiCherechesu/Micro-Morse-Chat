/* vim: set sw=2 expandtab tw=80: */

#include <stdio.h>
#include <stdlib.h>
#include "tock.h"
#include "network.h"
#include "buzzer.h"
#include <string.h>
#include "timer.h"

int point[] = {NOTE_G4, 4};
int dash[] = {NOTE_C4, -4};

#define TEMPO 85

struct message_t
{
	int uid;
	char *morse;
};

struct message_t *parse_data(char *data)
{
	struct message_t *msg = calloc(10, sizeof(struct message_t));
	for (int i = 0; i < 10; i++)
	{
		msg[i].morse = calloc(10, sizeof(char));
	}

	int num_messages;
	char *messages = calloc(128, sizeof(char));
	int bytes_read;

	sscanf(data, "%d", &num_messages);

	messages = data + 2;
	printf("Avem de citit %d mesaje!\n", num_messages);
	printf("Mesajele de citit: %s\n", messages);

	for (int i = 0; i < num_messages; i++)
	{
		sscanf(messages, "%d,%s", &(msg[i].uid), msg[i].morse);
		printf("%d,%s\n", msg[i].uid, msg[i].morse);
		messages += 12;
	}

	// char *ptr = strtok(data, ";");
	// while (ptr) {
	//     sscanf(ptr, "%d,%s", &(msg[idx].uid), msg[idx].morse);
	//     // char *internal_ptr = strtok(ptr, ",");
	//     // msg[idx].uid = atoi(internal_ptr);

	//     // internal_ptr = strtok(NULL, ",");
	//     // msg[idx].morse = internal_ptr;

	//     ptr = strtok(NULL, ";");
	//     idx++;
	// }

	return msg;
}

void use_buzzer(char *data)
{
	int *melody = calloc(2 * strlen(data), sizeof(int));
	for (int i = 0 ; i < strlen(data); i += 1) {
		if (data[i] == '.') {
			melody[2 * i] = point[0];
			melody[2 * i + 1] = point[1];
		} else {
			melody[2 * i] = dash[0];
			melody[2 * i + 1] = dash[1];
		}
	}

	if (!buzzer_exists()) {
		printf ("There is no available buzzer\n");
		// return -1;
  	}

	int notes = strlen(data);
	// int notes = sizeof(melody) / sizeof(melody[0]) / 2;
	int wholenote = (60000 * 4) / TEMPO;
	for (int note = 0; note < notes * 2; note = note + 2)
	{

		// calculates the duration of each note
		int divider = melody[note + 1];
		int note_duration = 0;
		if (divider > 0)
		{
			// regular note, just proceed
			note_duration = (wholenote) / divider;
		}
		else if (divider < 0)
		{
			// dotted notes are represented with negative durations!!
			note_duration = (wholenote) / abs(divider);
			note_duration *= 1.2; // increases the duration in half for dotted notes
		}

		// we only play the note for 90% of the duration, leaving 10% as a pause
		tone_sync(melody[note] * 3, note_duration * 0.8);
		delay_ms(200);
	}
}

#define NETWORK_URL "https://62044c2fc6d8b20017dc34c5.mockapi.io/api/v1/messages"

int main(void)
{
	struct message_t *msg;

	if (!driver_exists(DRIVER_NUM_NETWORK))
	{
		printf("No network driver\n");
		return -1;
	}

	char *data = network_get(NETWORK_URL, 1024);
	if (!data)
	{
		printf("Couldn't receive data\n");
		return -2;
	}

	// network_post(NETWORK_URL, "{}");

	printf("Got: %s\n", data);

	msg = parse_data(data);
	for (int i = 0; i < 10; i++)
	{
		if (msg[i].uid) {
			printf("%d,%s\n", msg[i].uid, msg[i].morse);
			use_buzzer(msg[i].morse);
		}

		delay_ms(500);
	}

	free(data);

	return 0;
}
