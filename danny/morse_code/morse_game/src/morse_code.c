/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#include <stdio.h>
#include "morse_code.h"
#include "system.h"

/****************		MACROS			****************/
// Number of characters available (a-z, 0-9)
#define MAX_NUM_CHARS 36

// UTF-8 codes for characters
#define UTF8_UPPER_A 65
#define UTF8_UPPER_Z 90
#define UTF8_LOWER_A 97
#define UTF8_LOWER_Z 122
#define UTF8_0 48
#define UTF8_9 57
#define UTF8_SPACE 32

typedef enum {
    DOT = 1,
    DASH = 2,
    END_OF_SYMBOL,
    END_OF_CHAR,
    END_OF_WORD
} MorseSymbol;



/****************		VARIABLES		****************/
static double morse_unit;
// a-z, 0-9 mapping of Morse code symbols
static u8 all_chars[MAX_NUM_CHARS][MAX_SYMBOLS_PER_CHAR] = {
	{1, 2},          {2, 1, 1, 1},    {2, 1, 2, 1},    {2, 1, 1},       {1},
	{1, 1, 2, 1},    {2, 2, 1},       {1, 1, 1, 1},    {1, 1},          {1, 2, 2, 2},
	{2, 1, 2},       {1, 2, 1, 1},    {2, 2},          {2, 1},          {2, 2, 2},
	{1, 2, 2, 1},    {2, 2, 1, 2},    {1, 2, 1},       {1, 1, 1},       {2},
	{1, 1, 2},       {1, 1, 1, 2},    {1, 2, 2},       {2, 1, 1, 2},    {2, 1, 2, 2},
	{2, 2, 1, 1},    {2, 2, 2, 2, 2}, {1, 2, 2, 2, 2}, {1, 1, 2, 2, 2}, {1, 1, 1, 2, 2},
	{1, 1, 1, 1, 2}, {1, 1, 1, 1, 1}, {2, 1, 1, 1, 1}, {2, 2, 1, 1, 1}, {2, 2, 2, 1, 1},
	{2, 2, 2, 2, 1}
};



/****************		FUNCTIONS		****************/
/*
 * @brief Checks equality of two same sized u8 arrays
 * @param arr_1[] Array 1
 * @param arr_2[] Array 2
 * @param length Length of both arrays
 */
static u8 arr_eq(u8 arr_1[], u8 arr_2[], size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (arr_1[i] != arr_2[i]) return 0;
    }
    return 1;
}

/*
 * @brief Generates a random string
 * @param buffer Writes random string to buffer
 * @param length Length of string to be generated
 *
 * Uses timer0 as a RNG
 */
void generate_random_string(char* buffer, size_t length) {
    if (length < 1) return;

    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    size_t charset_size = sizeof(charset) - 1;

    for (size_t i = 0; i < length; i++) {
        u32 timer0_value = *(timer0_ptr+2);
        int key = timer0_value % charset_size;
        buffer[i] = charset[key];
    }

    buffer[length] = '\0';  // Null-terminate the string
}

/**
 * @brief Calibrates the Morse timing unit based on user input.
 *
 * The user must press the button three times to represent three Morse code dots.
 * The average press duration is stored as the base timing unit for dots and other symbols.
 */
void calibrate() {
    double press_time;
    u8 calibration_count = 0;
    u32 timer_value;

    xil_printf("To calibrate, do 3 dots.\n");
    while (calibration_count < 3) {
    	if (g_button_flag) {
    		usleep(40000);
    		g_button_flag = 0;
    		g_button_pressed = XGpio_DiscreteRead(&btn_gpio, CHANNEL);
    	} else continue;

        // Button pressed, begin press timer
        if (g_button_pressed) {
            XTmrCtr_Start(&timer0, 0);
        }

        // Button released
        else if (!g_button_pressed) {
            XTmrCtr_Stop(&timer0, 0);
            timer_value = XTmrCtr_GetValue(&timer0, 0);
            press_time = (double)timer_value / (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;

            printf("Press time: %.2f\n", press_time);
            morse_unit += press_time;
            ++calibration_count;
        }
    }
    morse_unit /= calibration_count;
    // Set release time for character gaps
    *(timer1_ptr + 1) = morse_unit * 6 * XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
    printf("Morse dot average: %.2f\n", morse_unit);
}

/*
 * @brief Blinks a given word in Morse
 * @param str[] Word to be blinked
 * @param display_chars If 1, characters are printed as they are blinked
 *
 * Switches control how fast blinks are.
 * 	SW3 - 8x faster
 * 	SW2 - 4x faster
 * 	SW1 - 2x faster
 * 	SW0 - 1x faster
 */
void word_to_morse(const char str[], u8 display_chars) {
    // Used to convert UTF-8 character hex code to an index value
    // to find Morse code equivalent in 2-D array `all_chars`
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
        for (u8 symbol_i = 0; symbol_i < MAX_SYMBOLS_PER_CHAR; ++symbol_i) {
        	if (g_button_flag) return;
            // Skip filler symbols
            if (all_chars[str[char_i] - decrement][symbol_i] == 0) continue;

            XGpio_DiscreteWrite(&led_gpio, CHANNEL, 0x0F);

            // Determines if dot or dash
            if (all_chars[str[char_i] - decrement][symbol_i] == DOT) {
                usleep(1000000 / speed_factor);  // Dot length
            } else if (all_chars[str[char_i] - decrement][symbol_i] == DASH) {
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

/*
 * @brief Decodes Morse message to string
 * @param translated_word Decoded message written here
 * @param morse_input User's Morse input
 * @param word_size Character length of user's Morse input
 */
void morse_to_word(char* translated_word, u8 morse_input[100][MAX_SYMBOLS_PER_CHAR], u8 word_size) {
	u8 char_found = 0;
    // Loop through "Morsed" word
    for (u8 char_i = 0; char_i <= word_size; ++char_i) {
        // Check Morsed char against all characters
        for (u8 all_char_i = 0; all_char_i < MAX_NUM_CHARS; ++all_char_i) {
            // Compare current pattern of symbols against all valid Morse characters
            if (!arr_eq(morse_input[char_i], all_chars[all_char_i], MAX_SYMBOLS_PER_CHAR)) continue;
            // If letter
            if (all_char_i < 26) translated_word[char_i] = all_char_i + UTF8_LOWER_A;
            // If number
            else if (all_char_i < MAX_NUM_CHARS) translated_word[char_i] = all_char_i + 0x16;
            char_found = 1;
            break;
        }
        // If pattern doesn't exist
		if (!char_found) translated_word[char_i] = '?';
        char_found = 0;
    }
    translated_word[word_size + 1] = '\0';
}

/*
 * @brief Configures timers upon button press
 */
void morse_press(void) {
    XTmrCtr_Start(&timer0, 0);                       // Start press timer
    *timer1_ptr |= BIT5;                             // Stop release timer
    // Set reset value of timer1 to time length of character gap
    *(timer1_ptr + 1) = morse_unit * 6 * XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
}

/*
 * @brief Configures timers upon button release and determines if dot or dash
 */
u8 morse_release() {
    XTmrCtr_Stop(&timer0, 0);                        // Stop press timer
    *timer1_ptr &= ~(BIT5);                          // Start release timer
    u32 timer0_value = *(timer0_ptr+2);  			 // Get press time
    double press_time = (double)timer0_value / (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;

    if (press_time < morse_unit * 2.5) {
        xil_printf(".");
        return DOT;
    } else {
        xil_printf("-");
        return DASH;
    }
}

/*
 * @brief Configures timers when button has been released for a character gap time length
 */
void morse_char_gap() {
    *timer1_ptr |= BIT5; // Reload/stop release timer
    *(timer1_ptr + 1) = morse_unit * 9 * XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
    *timer1_ptr &= ~(BIT5); // Start release timer
    xil_printf(" | ");
}
