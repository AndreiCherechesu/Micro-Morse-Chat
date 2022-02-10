/* Morse Chat Application Header File */

#ifndef _MORSE_CHAT_APP_H_
#define _MORSE_CHAT_APP_H_

#define MORSE_MAX_LEN 128
#define MAX_MESSAGES_NUM 

struct message_t
{
    int uid;
	char *morse;
};

struct message_t *parse_data(char *data, int *num_messages);

#endif /* _MORSE_CHAT_APP_H_ */