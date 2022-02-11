#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buzzer.h"
#include "button.h"
#include "ipc.h"
#include "led.h"
#include "timer.h"
#include "tock.h"

#include "morse_display.h"


static void use_buzzer(char *data)
{
	int notes = strlen(data);
	int *melody = calloc(2 * notes, sizeof(int));

	if (!buzzer_exists()) {
		printf ("There is no available buzzer\n");
		return;
  	}

    /* Generate melody array: (note_freq, note_duration) */
	for (int i = 0 ; i < notes; i += 1) {
		if (data[i] == '.') {
			melody[2 * i] = point[0];
			melody[2 * i + 1] = point[1];
		} else {
			melody[2 * i] = dash[0];
			melody[2 * i + 1] = dash[1];
		}
	}

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

static int *decode_morse(char *data)
{
	if (data == NULL) {
		return clear;
	}

	if (strcmp(data, ".-") == 0) {
		return letter_A;
	} else if (strcmp(data, "-...") == 0) {
		return letter_B;
	} else if (strcmp(data, "-.-.") == 0) {
		return letter_C;
	} else if (strcmp(data, "-..") == 0) {
		return letter_D;
	} else if (strcmp(data, ".") == 0) {
		return letter_E;
	} else if (strcmp(data, "..-.") == 0) {
		return letter_F;
	} else if (strcmp(data, "--.") == 0) {
		return letter_G;
	} else if (strcmp(data, "....") == 0) {
		return letter_H;
	} else if (strcmp(data, "..") == 0) {
		return letter_I;
	} else if (strcmp(data, ".---") == 0) {
		return letter_J;
	} else if (strcmp(data, "-.-") == 0) {
		return letter_K;
	} else if (strcmp(data, ".-..") == 0) {
		return letter_L;
	} else if (strcmp(data, "--") == 0) {
		return letter_M;
	} else if (strcmp(data, "-.") == 0) {
		return letter_N;
	} else if (strcmp(data, "---") == 0) {
		return letter_O;
	} else if (strcmp(data, ".--.") == 0) {
		return letter_P;
	} else if (strcmp(data, "--.-") == 0) {
		return letter_Q;
	} else if (strcmp(data, ".-.") == 0) {
		return letter_R;
	} else if (strcmp(data, "...") == 0) {
		return letter_S;
	} else if (strcmp(data, "-") == 0) {
		return letter_T;
	} else if (strcmp(data, "..-") == 0) {
		return letter_U;
	} else if (strcmp(data, "...-") == 0) {
		return letter_V;
	} else if (strcmp(data, ".--") == 0) {
		return letter_W;
	} else if (strcmp(data, "-..-") == 0) {
		return letter_X;
	} else if (strcmp(data, "-.--") == 0) {
		return letter_Y;
	} else if (strcmp(data, "--..") == 0) {
		return letter_Z;
	}

	return letter_unknown;
}

static void use_leds(char *data, int *recording)
{
    int num_leds;

    int err = led_count(&num_leds);
	if (err < 0) {
        printf("Couldn't get LED number\n");
        return;
    }

    for (int i = 0; i < num_leds; i++) {
      if (*recording == 1) {
        if (recording_mode[i])
          led_on(i);
        else
          led_off(i);
      } else {
        if (decode_morse(data)[i])
                led_on(i);
        else
          led_off(i);
      }
    }

}

static void ipc_callback(int pid, int len, int buf, __attribute__ ((unused)) void* ud) {
    char *buffer = (char *) buf;
    struct message_t msg;
    msg.morse = calloc(MORSE_MAX_LEN + 1, sizeof(char));

    if (!len)
        return;

    /* Get shared data */
	int recording;

    sscanf(buffer, "%d %d %s", &recording, &msg.uid, msg.morse);

	printf("[MORSE_DISPLAY]: %d %s\n", msg.uid, msg.morse);

	if (recording == 1) {
		use_leds(NULL, &recording);
	    ipc_notify_client(pid);
		return;
	}

	/* Display sender ID using LEDs */
	use_leds((char *) msg.morse, &recording);

	/* Play the buzzer */
	use_buzzer((char *) msg.morse);

	/* Clear leds */
	use_leds(NULL, &recording);


    ipc_notify_client(pid);
}

int main(void)
{
    ipc_register_service_callback(ipc_callback, NULL);
    return 0;
}