#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include "system.h"

// Constants
#define DOT 1
#define DASH 2
#define END_OF_SYMBOL 3
#define END_OF_CHAR 4
#define END_OF_WORD 5

// UTF-8 codes for characters
#define UTF8_UPPER_A 65
#define UTF8_UPPER_Z 90
#define UTF8_LOWER_A 97
#define UTF8_LOWER_Z 122
#define UTF8_0 48
#define UTF8_9 57
#define UTF8_SPACE 32




// Variables
// Morse code character map
extern u8 characters[36][5];




// Function declarations
void calibrate();
u8 arr_eq(u8 arr_1 [], u8 arr_2 [], size_t length);
void word_to_morse(const char str[], u8 display_chars);
void morse_to_word(char * translated_word, u8 word_chars [100][5], u8 word_size);
u8 morse_press();
u8 morse_release();
void generate_random_string(char* buffer, size_t length);

#endif // MORSE_UTILS_H
