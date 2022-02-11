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
	0, 1, 0, 0, 0,
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

int letter_A[] = {
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1    
};

int letter_B[] = {
    0, 1, 1, 1, 1,
    0, 1, 0, 1, 0,
    0, 1, 1, 0, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 1    
};

int letter_C[] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 0, 0, 0, 0,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1
};

int letter_D[] = {
    1, 1, 1, 0, 0,
    1, 0, 0, 1, 0,
    1, 0, 0, 1, 0,
    1, 0, 0, 1, 0,
    1, 1, 1, 0, 0
};

int letter_E[] = {
    1, 1, 1, 1, 0,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 0,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 0
};

int letter_F[] = {
    1, 1, 1, 1, 0,
    1, 0, 0, 0, 0,
    1, 1, 1, 0, 0,
    1, 0, 0, 0, 0,
    1, 0, 0, 0, 0
};

int letter_G[] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 0, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1
};

int letter_H[] = {
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1
};

int letter_I[] = {
    0, 0, 1, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0
};

int letter_J[] = {
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

int letter_K[] = {
    0, 1, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 1, 1, 0, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 0, 1
};

int letter_L[] = {
    0, 1, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 1
};

int letter_M[] = {
    1, 1, 0, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1
};

int letter_N[] = {
    0, 1, 0, 0, 1,
    0, 1, 1, 0, 1,
    0, 1, 0, 1, 1,
    0, 1, 0, 0, 1,
    0, 1, 0, 0, 1
};

int letter_O[] = {
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    1, 1, 0, 1, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0
};

int letter_P[] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 0, 0, 0
};

int letter_Q[] = {
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 0, 1, 1, 1
};

int letter_R[] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 0, 1, 0
};

int letter_S[] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

int letter_T[] = {
    1, 1, 1, 1, 1,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0
};

int letter_U[] = {
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

int letter_V[] = {
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0
};

int letter_W[] = {
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 0, 1, 0, 1,
    0, 0, 1, 0, 1,
    0, 1, 0, 1, 0
};

int letter_X[] = {
    1, 0, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    1, 0, 0, 0, 1
};

int letter_Y[] = {
    1, 0, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0
};

int letter_Z[] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 0, 0,
    1, 1, 1, 1, 1
};

int letter_unknown[] = {
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0
};

int *digits[] = {clear, digit_1, digit_2, digit_3, digit_4, digit_5};
int num_digits = 5;

#endif /* _MORSE_DISPLAY_APP_H_ */
