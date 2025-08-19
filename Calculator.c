#include "xparameters.h"
#include "xuartps.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UART_DEVICE_ID  XPAR_XUARTPS_0_DEVICE_ID
#define BAUD_RATE       115200
#define RX_BUF_SIZE     64

static XUartPs Uart_Ps;
static uint8_t rx_buf[RX_BUF_SIZE];

static void uart_send_string(const char *str) {
    XUartPs_Send(&Uart_Ps, (uint8_t*)str, strlen(str));
}

static int uart_recv_line(char *buf, int max_len) {
    int len = 0;
    while (len < max_len - 1) {
        uint8_t c;
        int r = XUartPs_Recv(&Uart_Ps, &c, 1);
        if (r > 0) {
            if (c == '\n' || c == '\r') break;
            buf[len++] = (char)c;
        }
    }
    buf[len] = '\0';
    return len;
}

int main(void) {
    XUartPs_Config *Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (!Config) return -1;
    if (XUartPs_CfgInitialize(&Uart_Ps, Config, Config->BaseAddress) != XST_SUCCESS) return -1;
    XUartPs_SetBaudRate(&Uart_Ps, BAUD_RATE);

    uart_send_string("Connected to FPGA UART Calculator.\r\n");
    uart_send_string("Type commands: ADD 4 7, SUB 10 3, MUL 6 5, DIV 20 4\r\n");
    uart_send_string("Type 'exit' to quit.\r\n\r\n");

    while (1) {
        uart_send_string("> ")
        if (uart_recv_line((char*)rx_buf, RX_BUF_SIZE) == 0) continue;

        if (strcmp((char*)rx_buf, "exit") == 0) {
            uart_send_string("Exiting...\r\n");
            break;
        }

        char op[8];
        int a, b;
        int parsed = sscanf((char*)rx_buf, "%7s %d %d", op, &a, &b);
        if (parsed != 3) {
            uart_send_string("Error: Invalid input\r\n");
            continue;
        }

        int result;
        // Operations
        if (strcmp(op, "ADD") == 0) result = a + b;
        else if (strcmp(op, "SUB") == 0) result = a - b;
        else if (strcmp(op, "MUL") == 0) result = a * b;
        else if (strcmp(op, "DIV") == 0) {
            if (b == 0) {
                uart_send_string("Error: Division by zero\r\n");
                continue;
            }
            result = a / b;
        } else {
            uart_send_string("Error: Unknown operation\r\n");
            continue;
        }

        char out_buf[32];
        snprintf(out_buf, sizeof(out_buf), "Result: %d\r\n", result);
        uart_send_string(out_buf);
    }

    return 0;
}
