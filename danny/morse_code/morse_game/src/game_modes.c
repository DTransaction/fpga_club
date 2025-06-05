#include "game_modes.h"
#include "morse_code.h"

void transmitter() {
    char random_word[100] = {0};
    u8 word_arr[100][5] = {0};
    char translated_word[100] = {0};

    xil_printf("Transmitter mode\n");
    xil_printf("Given a random word, try to type it out in morse code. \n");

    while (btn_pressed != BTN3) {
        memset(random_word, 0, sizeof(random_word));
        memset(word_arr, 0, sizeof(word_arr));
        generate_random_string(random_word, 5);
        u8 char_count = 0;
        u8 symbol_count = 0;
        xil_printf("%s\n", random_word);

        while (1) {
            // Check for button presses
        	if (btn_flag) {
        		btn_flag = 0;
				// Button pressed
				if (btn_pressed && morse_press() == END_OF_CHAR) {
					++char_count;
					symbol_count = 0;
				}
				// Button released
				else if (!btn_pressed) {
					word_arr[char_count][symbol_count] = morse_release();
					++symbol_count;
				}
				// Go to main menu
				else if (btn_pressed == BTN3) break;
        	}

            // Released too long, end of word
            else if (timer1_flag) {
                timer1_flag = 0;
                morse_to_word(translated_word, word_arr, char_count);
                xil_printf("   %s\n", translated_word);
                if (strcmp(translated_word, random_word))
                    xil_printf("Incorrect!\n\n");
                else
                    xil_printf("Correct!\n\n");
                break;
            }
        }
    }
}

void receiver() {
    char random_word[100] = {0};
    char buffer[100] = {0};

	xil_printf("Receiver mode\n");
	xil_printf("    Board will blink a random word. When complete,\n");
	xil_printf("    type what it translates to in the serial terminal.\n");
	xil_printf("    Use switches to change blink speed.\n\n");

	sleep(3);

	while (btn_pressed != BTN3) {
		for (u8 i = 3; i>0; --i) {
			xil_printf("Beginning in %d...\n", i);
			sleep(1);
		}
	    generate_random_string(random_word, 5);
	    word_to_morse(random_word, 0);
	    xil_printf("Done. Guess: ");
        while (!XUartPs_IsReceiveData(uart_ps.Config.BaseAddress)) {
        	if (btn_pressed == BTN3) return;
        }
        read_uart_string(buffer);
        xil_printf("%s\n", buffer);
        if (strcmp(random_word, buffer)) {
        	xil_printf("Incorrect! Actual word was: %s\n\n", random_word);
        }
        else xil_printf("Correct!\n\n");

        memset(random_word, 0, sizeof(random_word));
        memset(buffer, 0, sizeof(buffer));
	}
}

void free_play() {
    u8 symbol_count = 0;
    u8 char_count = 0;
    u8 word_arr[100][5] = {0};
    char buffer[100] = {0};
    timer1_flag = 0;

    xil_printf("Free play mode\n");
    xil_printf("Morse to word: Enter morse code with the buttons\n");
    xil_printf("Word to morse: Send UART message to blink in morse code.\n");
    xil_printf("    Switches control blinking speed\n");

    while (1) {
        // Check for button activity
    	if (btn_flag) {
    		btn_flag = 0;
			// Go to main menu
			if (btn_pressed == BTN3) return;
			// Button pressed
			else if (btn_pressed && morse_press() == END_OF_CHAR) {
				++char_count;
				symbol_count = 0;
			}
			// Button released
			else if (!btn_pressed) {
				word_arr[char_count][symbol_count] = morse_release();
				// User makes mistake and inputs >5 symbols
				if (symbol_count >= 5) {
					++char_count;
					symbol_count = 0;
				} else ++symbol_count;
			}
    	}
        // Released too long, end of word
        else if (timer1_flag) {
            timer1_flag = 0;
            morse_to_word(buffer, word_arr, char_count);
            xil_printf("   %s\n", buffer);
            // Reset word array and buffer
            memset(word_arr, 0, sizeof(word_arr));
            memset(buffer, 0, sizeof(buffer));
            char_count = 0;
            symbol_count = 0;
        }
        // Check for UART input
        else if (XUartPs_IsReceiveData(uart_ps.Config.BaseAddress)) {
            read_uart_string(buffer);
            word_to_morse(buffer, 1);
            memset(buffer, 0, sizeof(buffer));
        }
    }
}
