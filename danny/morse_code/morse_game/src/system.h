/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include "xgpio.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "xparameters.h"

// Device IDs
#define UART_ID        XPAR_XUARTPS_0_DEVICE_ID
#define BTN_ID         XPAR_AXI_GPIO_0_DEVICE_ID
#define SW_ID          XPAR_AXI_GPIO_1_DEVICE_ID
#define LED_ID         XPAR_AXI_GPIO_2_DEVICE_ID
#define RGB_ID         XPAR_AXI_GPIO_3_DEVICE_ID
#define CHANNEL        1

// Buttons
#define BTN3 0x08
#define BTN2 0x04
#define BTN1 0x02
#define BTN0 0x01

// Bits
#define BIT8 1<<8
#define BIT7 1<<7
#define BIT6 1<<6
#define BIT5 1<<5
#define BIT4 1<<4
#define BIT3 1<<3
#define BIT2 1<<2
#define BIT1 1<<1
#define BIT0 1<<0

// Global peripherals
extern XGpio btn_gpio;
extern XGpio sw_gpio;
extern XGpio led_gpio;
extern XGpio rgb_gpio;
extern XTmrCtr timer0;
extern XTmrCtr timer1;
extern u32 *const timer0_ptr;
extern u32 *const timer1_ptr;
extern XScuGic gic;
extern XUartPs uart_ps;
extern u32 g_button_pressed;

// Flags
extern u8 g_button_flag;
extern u8 g_timer1_flag;

// Function declarations
void initialize_gpio(XGpio *gpio, u16 id);
void initialize_timer(XTmrCtr *timer, u16 id);
void Button_Intr_Handler(void *CallbackRef);
void Timer1_Intr_Handler(void *CallbackRef);
void initialize_interrupt();
void initialize_board(void);
void read_uart_string(char * uart_string);
u32 detect_btn();

#endif // SYSTEM_INIT_H
