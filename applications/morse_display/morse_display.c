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

static void use_leds(int sender_id)
{
    int num_leds;

    int err = led_count(&num_leds);
	if (err < 0) {
        printf("Couldn't get LED number\n");
        return;
    }

    for (int i = 0; i < num_leds; i++) {
        if (digits[sender_id][i])
            led_on(i);
        else
            led_off(i);
    }
  
}

static void ipc_callback(int pid, int len, int buf, __attribute__ ((unused)) void* ud) {
    char *buffer = (char *) buf;
    struct message_t msg;
    msg.morse = calloc(MORSE_MAX_LEN, sizeof(char));
    
    if (!len)
        return;
    
    /* Get shared data */
    sscanf(buffer, "%d %s", &msg.uid, msg.morse);
    printf("[MORSE_DISPLAY]: %d %s", msg.uid, msg.morse);
    
    /* Display sender ID using LEDs */
    use_leds(msg.uid);

    /* Play the buzzer */
    use_buzzer((char *) msg.morse);

    /* Clear leds */
    use_leds(0);

    ipc_notify_client(pid);
}

int main(void)
{
    ipc_register_service_callback(ipc_callback, NULL);
    return 0;
}