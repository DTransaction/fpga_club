#include <stdio.h>

#include "morse_code.h"
#include "system.h"

u8 characters[36][5] = {{1, 2},          {2, 1, 1, 1},    {2, 1, 2, 1},    {2, 1, 1},       {1},
                        {1, 1, 2, 1},    {2, 2, 1},       {1, 1, 1, 1},    {1, 1},          {1, 2, 2, 2},
                        {2, 1, 2},       {1, 2, 1, 1},    {2, 2},          {2, 1},          {2, 2, 2},
                        {1, 2, 2, 1},    {2, 2, 1, 2},    {1, 2, 1},       {1, 1, 1},       {2},
                        {1, 1, 2},       {1, 1, 1, 2},    {1, 2, 2},       {2, 1, 1, 2},    {2, 1, 2, 2},
                        {2, 2, 1, 1},    {2, 2, 2, 2, 2}, {1, 2, 2, 2, 2}, {1, 1, 2, 2, 2}, {1, 1, 1, 2, 2},
                        {1, 1, 1, 1, 2}, {1, 1, 1, 1, 1}, {2, 1, 1, 1, 1}, {2, 2, 1, 1, 1}, {2, 2, 2, 1, 1},
                        {2, 2, 2, 2, 1}};
double dot_avg;

// Calibration
void calibrate() {
    double press_time;
    u8 calibration_count = 0;
    u32 timer_value;

    xil_printf("To calibrate, do 3 dots.\n");
    while (calibration_count < 3) {
    	if (btn_flag) {
    		usleep(40000);
    		btn_flag = 0;
    		btn_pressed = XGpio_DiscreteRead(&btn_gpio, CHANNEL);
    	} else continue;

        // Button pressed, begin press timer
        if (btn_pressed) {
            XTmrCtr_Start(&timer0, 0);
        }

        // Button released
        else if (!btn_pressed) {
            XTmrCtr_Stop(&timer0, 0);
            timer_value = XTmrCtr_GetValue(&timer0, 0);
            press_time = (double)timer_value / (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;

            printf("Press time: %.2f\n", press_time);
            dot_avg += press_time;
            ++calibration_count;
        }
    }
    dot_avg /= calibration_count;
    // Set release time for word gaps
    *(timer1_ptr + 1) = dot_avg * 15 * XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
    printf("Dot average: %.2f\n", dot_avg);
}

u8 arr_eq(u8 arr_1[], u8 arr_2[], size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (arr_1[i] != arr_2[i]) return 0;
    }
    return 1;
}

// Given a string, blinks it in Morse code
void word_to_morse(const char str[], u8 display_chars) {
    // Used to convert UTF-8 character hex code to an index value
    // to find Morse code equivalent in 2-D array `characters`
    u8 decrement;
    // Speeds up the transmission based off of the switch
    // configuration in binary
    u8 speed_factor = XGpio_DiscreteRead(&sw_gpio, CHANNEL);

    // Ensure division by 0 is not possible
    if (speed_factor == 0) speed_factor = 1;

    // Iterate through string
    for (u8 char_i = 0; char_i < strlen(str); ++char_i) {
        // Check if uppercase letter
        if (str[char_i] >= UTF8_UPPER_A && str[char_i] <= UTF8_UPPER_Z) {
            decrement = UTF8_UPPER_A;
            // Check if lowercase letter
        } else if (str[char_i] >= UTF8_LOWER_A && str[char_i] <= UTF8_LOWER_Z) {
            decrement = UTF8_LOWER_A;
            // Check if number
        } else if (str[char_i] >= UTF8_0 && str[char_i] <= UTF8_9) {
            decrement = 0x16;
            // Check if space
        } else if (str[char_i] == UTF8_SPACE) {
            usleep(8000000 / speed_factor);
            xil_printf(" ", str[char_i]);
        } else {
            xil_printf("?");
            continue;
        }
        if (display_chars) xil_printf("%c", str[char_i]);

        // Iterate through Morse code symbols of a character
        for (u8 symbol_i = 0; symbol_i < 5; ++symbol_i) {
            // Skip filler symbols
            if (characters[str[char_i] - decrement][symbol_i] == 0) continue;

            XGpio_DiscreteWrite(&led_gpio, CHANNEL, 0x0F);

            // Determines if dot or dash
            if (characters[str[char_i] - decrement][symbol_i] == DOT) {
                usleep(1000000 / speed_factor);  // Dot length
            } else if (characters[str[char_i] - decrement][symbol_i] == DASH) {
                usleep(3000000 / speed_factor);  // Dash length
            }

            // Pause between Morse symbols
            XGpio_DiscreteWrite(&led_gpio, CHANNEL, 0x00);
            usleep(1000000 / speed_factor);
        }
        usleep(2000000 / speed_factor);  // Pause between characters
    }
    xil_printf("\n");
}

void morse_to_word(char* translated_word, u8 word_chars[100][5], u8 word_size) {
    // Loop through "morsed" word
    for (u8 char_i = 0; char_i <= word_size; ++char_i) {
        // Check morsed char against all characters
        for (u8 all_char_i = 0; all_char_i < 36; ++all_char_i) {
            // Compare current pattern of symbols against all valid morse characters
            if (!arr_eq(word_chars[char_i], characters[all_char_i], 5)) continue;

            // If letter
            if (all_char_i < 26) {
                translated_word[char_i] = all_char_i + UTF8_LOWER_A;
            }
            // If number
            else if (all_char_i < 36) {
                translated_word[char_i] = all_char_i + 0x16;
            }
            // If does not exist
            else
                translated_word[char_i] = '?';
            break;
        }
    }
    translated_word[word_size + 1] = '\0';
}

u8 morse_press() {
    u32 timer_value = *(timer0_ptr+2);
//    u32 timer_value = XTmrCtr_GetValue(&timer0, 0);  // Get released time
    XTmrCtr_Start(&timer0, 0);                       // Start press timer
    *timer1_ptr |= BIT5;                             // Stop release timer
                                                     //	XTmrCtr_Stop(&timer1, 1); // Stop release timer
    double release_time = (double)timer_value / (double)XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;

    // End of symbol
    if (release_time < dot_avg * 6) {
        return END_OF_SYMBOL;
        // End of character
    } else if (release_time <= dot_avg * 15) {
        xil_printf(" | ");
        return END_OF_CHAR;
        // End of word
    } else {
        return END_OF_WORD;
    }
}

u8 morse_release() {
    u32 timer_value = *(timer0_ptr+2);  			 // Get pressed time
    XTmrCtr_Start(&timer0, 0);                       // Start release timer
    *timer1_ptr &= ~(BIT5);                          // Start release timer
    double press_time = (double)timer_value / (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;

    if (press_time < dot_avg * 2.5) {
        xil_printf(".");
        return DOT;
    } else {
        xil_printf("-");
        return DASH;
    }
}

void generate_random_string(char* buffer, size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    size_t charset_size = sizeof(charset) - 1;

    if (length < 1) return;

    for (size_t i = 0; i < length; i++) {
        int key = rand() % charset_size;
        buffer[i] = charset[key];
    }

    buffer[length] = '\0';  // Null-terminate the string
}
