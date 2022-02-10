/* Morse Display Application Header File */

#ifndef _MORSE_DISPLAY_APP_H_
#define _MORSE_DISPLAY_APP_H_

#define MORSE_MAX_LEN 128

struct message_t
{
    int uid;
	char *morse;
};

int point[] = {NOTE_G4, 4};
int dash[] = {NOTE_C4, -4};

#define TEMPO 126

int clear[] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0    
};

int digit_1[] = {
	0, 0, 0, 1, 0,
	0, 0, 1, 1, 0,
	0, 1, 0, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0
};

int digit_2[] = {
	0, 0, 1, 0, 0,
	0, 1, 0, 1, 0,
	0, 0, 1, 0, 0,
	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0
};

int digit_3[] = {
	0, 1, 1, 1, 0,
	0, 0, 0, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 1, 0,
	0, 1, 1, 1, 0
};

int digit_4[] = {
	0, 1, 0, 1, 0,
	0, 1, 0, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 1, 0,
	0, 0, 0, 1, 0
};

int digit_5[] = {
	0, 1, 1, 1, 0,
	0, 1, 0, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 1, 0,
	0, 1, 1, 1, 0
};

int *digits[] = {clear, digit_1, digit_2, digit_3, digit_4, digit_5};
int num_digits = 5;

#endif /* _MORSE_DISPLAY_APP_H_ */
