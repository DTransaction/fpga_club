#include "game_modes.h"
#include "morse_code.h"
#include "sleep.h"
#include "system.h"
#include "xil_printf.h"
#include "xuartps.h"
#include "xsysmon.h"
#define MAIN_MENU 8
#define TRANSMITTER 4
#define RECEIVER 2
#define FREE_PLAY 1

void print_main_menu_dialogue() {
    xil_printf(
    	"\nPress BTN3 at any point to return to main menu. \n"
        "Main menu: press BTN to select game mode:\n"
        "3 - Main menu\n"
        "2 - Transmitter\n"
        "1 - Receiver\n"
        "0 - Free play\n\n");
}

int main() {
	u8 game_mode = MAIN_MENU;
    xil_printf("\nProgram started\n");  // Might need to sleep for a bit after this
    usleep(2000);
    initialize_board();
    calibrate();
    print_main_menu_dialogue();

    // Main forever loop
    while (1) {
    	if (btn_flag) {
    		btn_flag = 0;
    	} else continue;

    	game_mode = btn_pressed;

    	while (btn_pressed);
    	btn_flag = 0;

        switch (game_mode) {
			case MAIN_MENU:
				break;
            case TRANSMITTER:
                transmitter();
                print_main_menu_dialogue();
                break;
            case RECEIVER:
                receiver();
                print_main_menu_dialogue();
                break;
            case FREE_PLAY:
                free_play();
                print_main_menu_dialogue();
                break;
        }
    }

    return 0;
}
