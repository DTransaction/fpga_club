/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#include "game_modes.h"
#include "morse_code.h"



/****************		MACROS		****************/
#define MAX_WORD_LEN 100



/****************		VARIABLES		****************/



/****************		FUNCTIONS		****************/
/**
 * @brief User tries to transmit randomly generated word
 *
 * Generates random 5 character word, user replicates that
 * word through Morse code and is verified.
 */
void transmitter(void) {
    char random_word[MAX_WORD_LEN] = {0};
    char translated_word[MAX_WORD_LEN] = {0};
    u8 morse_input[MAX_WORD_LEN][MAX_SYMBOLS_PER_CHAR] = {0};
    u8 char_count = 0;
    u8 symbol_count = 0;
	u8 character_gap = 0;

	g_timer1_flag = 0;

    xil_printf(
    	"Transmitter mode\n"
    	"    Given a random word, try to signal it in Morse code. \n"
    );

    generate_random_string(random_word, MAX_SYMBOLS_PER_CHAR);
    xil_printf("%s\n", random_word);

	while (1) {
		// Check for button action
		if (g_button_flag) {
			g_button_flag = 0;
			// Button pressed
			if (g_button_pressed) {
				// Return to main menu if BTN3 pressed
				if (g_button_pressed == BTN3) return;
				morse_press();
				character_gap = FALSE;
			}
			// Button released
			else {
				morse_input[char_count][symbol_count] = morse_release();
				++symbol_count;
			}
		}

		// Timer1 flag
		else if (g_timer1_flag) {
			g_timer1_flag = 0;
			// Timer1 stage 1: Released for morse_unit*6, end of character
			if (!character_gap) {
				morse_char_gap();
				++char_count;
				symbol_count = 0;
				character_gap = 1;
			}
			// Timer1 stage 2: Released for morse_unit*15, end of word
			else {
            	--char_count; // Past pause was not character gap, actually word gap
				character_gap = 0;
				// Print Morse code input as characters
				morse_to_word(translated_word, morse_input, char_count);
				xil_printf("   %s\n", translated_word);

				XTmrCtr_Start(&timer0, 0); // Start timer0 for RNG
				// Check if correct
				if (strcmp(translated_word, random_word))
					xil_printf("Incorrect!\n\n");
				else
					xil_printf("Correct!\n\n");

				// Reset transmitter game
				memset(random_word, 0, sizeof(random_word));
				memset(translated_word, 0, sizeof(translated_word));
				memset(morse_input, 0, sizeof(morse_input));
				char_count = 0;
				generate_random_string(random_word, MAX_SYMBOLS_PER_CHAR);
				xil_printf("%s\n", random_word);
			}
		}
	}
}

/**
 * @brief User tries to decode Morse message
 *
 * Generates random 5 character word, FPGA board blinks that
 * word in Morse code, user guesses word and is verified.
 *
 * Switches control how fast blinks are.
 * 	SW3 - 8x faster
 * 	SW2 - 4x faster
 * 	SW1 - 2x faster
 * 	SW0 - 1x faster
 */
void receiver(void) {
    char random_word[MAX_WORD_LEN] = {0};
    char buffer[MAX_WORD_LEN] = {0};

	xil_printf(
		"Receiver mode\n"
		"    Board will blink a random word. When complete,\n"
		"    type what it translates to in the serial terminal.\n"
		"    Use switches to change blink speed.\n\n"
	);

	sleep(3); // Give user time to read

	// Return to main menu if BTN3 pressed
	while (g_button_pressed != BTN3) {
		for (u8 i = 3; i>0; --i) {
			xil_printf("Beginning in %d...\n", i);
			sleep(1);
		}

		// Blink random word
	    generate_random_string(random_word, MAX_SYMBOLS_PER_CHAR);
	    word_to_morse(random_word, 0);



	    xil_printf("Done. Guess: ");
        while (!XUartPs_IsReceiveData(uart_ps.Config.BaseAddress)) {
        	if (g_button_pressed == BTN3) return;
        }

        // Read and compare strings
        read_uart_string(buffer);
        xil_printf("%s\n", buffer);
        if (strcmp(random_word, buffer)) {
        	xil_printf("Incorrect! Actual word was: %s\n\n", random_word);
        }
        else xil_printf("Correct!\n\n");

        // Reset game
        memset(random_word, 0, sizeof(random_word));
        memset(buffer, 0, sizeof(buffer));
	}
}

/*
 * @brief User transmits Morse message to be decoded or UART string to be blinked
 *
 * Mode 1 (Morse code to word): User presses button to generate
 *  whatever characters and the translated result will be
 *  printed into serial terminal.
 *
 * Mode 2 (Word to Morse code): User sends a word through serial
 *  terminal and the FPGA board blinks that word in Morse code.
 *
 *  Switches control how fast blinks are.
 * 	 SW3 - 8x faster
 * 	 SW2 - 4x faster
 * 	 SW1 - 2x faster
 * 	 SW0 - 1x faster
 */
void free_play(void) {
	u8 character_gap = 0;
    u8 symbol_count = 0;
    u8 char_count = 0;
    u8 morse_input[MAX_WORD_LEN][MAX_SYMBOLS_PER_CHAR] = {0};
    char buffer[MAX_WORD_LEN] = {0};
    g_timer1_flag = 0;

    xil_printf("Free play mode\n");
    xil_printf("Morse to word: Enter Morse code with the buttons\n");
    xil_printf("Word to Morse: Send UART message to blink in Morse code.\n");
    xil_printf("    Switches control blinking speed\n");

    while (1) {
        // Check for button activity
    	if (g_button_flag) {
    		g_button_flag = 0;
			// Button pressed
			if (g_button_pressed) {
				// Return to main menu if BTN3 pressed
				if (g_button_pressed == BTN3) return;
				morse_press();
				character_gap = 0;
			}
			// Button released
			else {
				// User makes mistake and inputs >5 symbols
				if (symbol_count >= MAX_SYMBOLS_PER_CHAR) {
					++char_count;
					symbol_count = 0;
				}
				morse_input[char_count][symbol_count] = morse_release();
				++symbol_count;
			}
    	}

    	// Timer1 flag
        else if (g_timer1_flag) {
            g_timer1_flag = 0;
            // Released for morse_unit*6, end of character
            if (!character_gap) {
            	morse_char_gap();
				++char_count;
				symbol_count = 0;
            	character_gap = TRUE;
            }

            // Released for morse_unit*15, end of word
            else {
            	--char_count; // Past pause was not character gap, actually word gap
            	// Print Morse code input as characters
				morse_to_word(buffer, morse_input, char_count);
				xil_printf("   %s\n", buffer);

				// Reset Morse code input and buffer
				memset(morse_input, 0, sizeof(morse_input));
				memset(buffer, 0, sizeof(buffer));

				// Reset counts
				char_count = 0;
				symbol_count = 0;
            	character_gap = FALSE;
            }
        }

        // Check for UART input
        else if (XUartPs_IsReceiveData(uart_ps.Config.BaseAddress)) {
            read_uart_string(buffer);
            word_to_morse(buffer, 1);
            memset(buffer, 0, sizeof(buffer));
        }
    }
}
