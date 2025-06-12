/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include "system.h"

/****************		MACROS			****************/
#define MAX_SYMBOLS_PER_CHAR 5

/****************		FUNCTIONS		****************/
static u8 arr_eq(u8 arr_1[], u8 arr_2[], size_t length);
void generate_random_string(char* buffer, size_t length);
void calibrate();
void word_to_morse(const char str[], u8 display_chars);
void morse_to_word(char * translated_word, u8 word_chars [100][5], u8 word_size);
void morse_press(void);
u8 morse_release();
void morse_char_gap();

#endif // MORSE_UTILS_H
