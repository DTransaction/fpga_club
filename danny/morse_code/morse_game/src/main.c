/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#include "game_modes.h"
#include "morse_code.h"
#include "sleep.h"
#include "system.h"
#include "xil_printf.h"
#include "xuartps.h"



/****************		MACROS			****************/
typedef enum {
	FREE_PLAY   = 1,
	RECEIVER    = 2,
	TRANSMITTER = 4,
	MAIN_MENU   = 8
} GameMode;



/****************		VARIABLES		****************/



/****************		FUNCTIONS		****************/
/**
 * @brief Print the main menu prompt
 */
static void print_main_menu(void) {
    xil_printf(
    	"\nPress BTN3 at any point to return to main menu. \n"
        "Main menu: press BTN to select game mode:\n"
        "3 - Main menu\n"
        "2 - Transmitter\n"
        "1 - Receiver\n"
        "0 - Free play\n\n"
    );
}


/**
 * @brief Determines which game mode function to call
 * @param mode The game mode selected
 */
static void handle_game_mode(GameMode mode) {
    switch (mode) {
        case TRANSMITTER:
            transmitter();
            break;
        case RECEIVER:
            receiver();
            break;
        case FREE_PLAY:
            free_play();
            break;
        default:
            return;
    }
    print_main_menu();
}

int main() {
	GameMode current_game_mode = MAIN_MENU;

    initialize_board();
    xil_printf("\nMorse Game Started\n");
    calibrate();
    print_main_menu();

    while (1) {
    	// Wait for button action
    	if (!g_button_flag) continue;

    	g_button_flag = 0;
    	current_game_mode = (GameMode)g_button_pressed;		// Capture button press
    	XTmrCtr_Start(&timer0, 0); 						  	// Timer used for RNG

    	while (g_button_pressed); 	// Don't continue unless button is released
    	g_button_flag = 0; 			// Button flag set by release, so reset flag

    	handle_game_mode(current_game_mode);
    }
    return 0;
}
