/*
 * Written by Danny Tran (101236303)
 * June 11, 2025
 */

#include "system.h"

// Global peripherals
XGpio btn_gpio;
XGpio sw_gpio;
XGpio led_gpio;
XGpio rgb_gpio;
XTmrCtr timer0;
XTmrCtr timer1;
u32 *const timer0_ptr = XPAR_AXI_TIMER_0_BASEADDR;
u32 *const timer1_ptr = XPAR_AXI_TIMER_1_BASEADDR;
XScuGic gic;
XUartPs uart_ps;
u32 g_button_pressed;

// Flags
u8 g_button_flag;
u8 g_timer1_flag;

// Initialize GPIO
void initialize_gpio(XGpio *gpio, u16 id) {
    u8 status = XGpio_Initialize(gpio, id);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO %d initialization failure\n", id);
    }
}

// Initialize timer
void initialize_timer(XTmrCtr *timer, u16 id) {
    u8 status;
    status = XTmrCtr_Initialize(timer, id);
    if (status != XST_SUCCESS) {
        xil_printf("Timer initialization failed\n");
    }
}

// Button ISR
void Button_Intr_Handler(void *CallbackRef) {
    g_button_flag = 1;
	usleep(30000);
	g_button_pressed = XGpio_DiscreteRead(&btn_gpio, CHANNEL);
    XGpio_DiscreteWrite(&led_gpio, CHANNEL, g_button_pressed);
    XGpio_InterruptClear(&btn_gpio, XGPIO_IR_CH1_MASK);
}

// Timer 1 Interrupt Service Routine
void Timer1_Intr_Handler(void *CallbackRef) {
    g_timer1_flag = 1;
    *timer1_ptr |= BIT8;  // Clear interrupt
//	xil_printf("Timer1 interrupt occurred\n");
}

// Initialize Interrupt System
void initialize_interrupt() {
    u8 status;
    XScuGic_Config *gic_config;

    // Lookup config
    gic_config = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
    if (gic_config == NULL) xil_printf("Lookup config failed\n");

    // Apply config to GIC instance
    status = XScuGic_CfgInitialize(&gic, gic_config, gic_config->CpuBaseAddress);
    if (status != XST_SUCCESS) xil_printf("Config initialization failed\n");

    // Connect buttons to GIC
    status = XScuGic_Connect(&gic, XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR, (Xil_ExceptionHandler)Button_Intr_Handler,
                             (void *)&btn_gpio);
    if (status != XST_SUCCESS) xil_printf("Button interrupt connect failed\n");

    // Connect timer to GIC
    status = XScuGic_Connect(&gic, XPAR_FABRIC_AXI_TIMER_1_INTERRUPT_INTR, (Xil_ExceptionHandler)Timer1_Intr_Handler,
                             (void *)&timer1);
    if (status != XST_SUCCESS) xil_printf("Timer1 interrupt connect failed\n");

    // Enable interrupts
    XScuGic_Enable(&gic, XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR);
    XScuGic_Enable(&gic, XPAR_FABRIC_AXI_TIMER_1_INTERRUPT_INTR);
    XGpio_InterruptEnable(&btn_gpio, XGPIO_IR_CH1_MASK);
    XGpio_InterruptGlobalEnable(&btn_gpio);

    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &gic);
    Xil_ExceptionEnable();
}

void initialize_board() {
    // Initialize GPIO
    initialize_gpio(&btn_gpio, BTN_ID);
    initialize_gpio(&sw_gpio, SW_ID);
    initialize_gpio(&led_gpio, LED_ID);
    initialize_gpio(&rgb_gpio, RGB_ID);

    // Initialize timers
    initialize_timer(&timer0, XPAR_TMRCTR_0_DEVICE_ID);
    initialize_timer(&timer1, XPAR_TMRCTR_1_DEVICE_ID);

    // Set GPIO directions
    XGpio_SetDataDirection(&btn_gpio, CHANNEL, 0xF);
    XGpio_SetDataDirection(&sw_gpio, CHANNEL, 0xF);
    XGpio_SetDataDirection(&led_gpio, CHANNEL, 0x0);
    XGpio_SetDataDirection(&rgb_gpio, CHANNEL, 0x0);

    // Initialize interrupt system
    initialize_interrupt();

    // Configure timers
    *(timer0_ptr) = 0x041; // 0000 1000 0001
    *(timer1_ptr) = 0x0E2; // 0000 1110 0010

    XUartPs_Config *Config = XUartPs_LookupConfig(UART_ID);
    XUartPs_CfgInitialize(&uart_ps, Config, Config->BaseAddress);
    XUartPs_SetOptions(&uart_ps, XUARTPS_OPTION_RESET_TX);
    XUartPs_SetOptions(&uart_ps, XUARTPS_OPTION_RESET_RX);
}

void read_uart_string(char *uart_string) {
    u8 received_byte;

    for (int char_i = 0; char_i < 100; ++char_i) {
        received_byte = XUartPs_RecvByte(uart_ps.Config.BaseAddress);
        if (received_byte == '\n' || received_byte == '\r') {
            uart_string[char_i] = '\0';
        	break;
    	}
        else uart_string[char_i] = received_byte;
    }
}

