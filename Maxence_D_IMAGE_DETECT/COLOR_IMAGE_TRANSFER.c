#include "xparameters.h"
#include "xuartps.h"
#include <stdint.h>

#define MY_UART_ID        XPAR_XUARTPS_0_DEVICE_ID
#define MY_BAUD_RATE      115200
#define MAX_IMAGE_SIZE    (640*480*3)
#define TIMEOUT_LIMIT     20000000

static XUartPs MyUart;
static uint8_t ImageBuffer[MAX_IMAGE_SIZE];

// Send multiple bytes through UART, blocking until done
static void send_bytes(uint8_t *data, unsigned length) {
    unsigned sent = 0;
    while (sent < length) {
        int s = XUartPs_Send(&MyUart, data + sent, length - sent);
        if (s > 0) sent += s;
    }
}

// Send a single byte through UART For handshake
static void send_byte(uint8_t b) {
    send_bytes(&b, 1);
}

// Receive exactly 'length' bytes from UART
static int receive_bytes(uint8_t *buffer, unsigned length) {
    unsigned received = 0, loops = 0;
    while (received < length) {
        int r = XUartPs_Recv(&MyUart, buffer + received, length - received);
        if (r > 0) {
            received += r;
            loops = 0;
        } else {
            loops++;
            if (loops > TIMEOUT_LIMIT) return -1;
        }
    }
    return received;
}

int main(void) {
    // Initialize UART
    XUartPs_Config *config = XUartPs_LookupConfig(MY_UART_ID);
    if (!config) return XST_FAILURE;
    if (XUartPs_CfgInitialize(&MyUart, config, config->BaseAddress) != XST_SUCCESS) {
        return XST_FAILURE;
    }
    XUartPs_SetBaudRate(&MyUart, MY_BAUD_RATE);

    while (1) {
        uint8_t startSignal;
        if (receive_bytes(&startSignal, 1) != 1) continue;
        if (startSignal != 'S') continue;

        send_byte('A');  //handshake

        uint8_t sizeBytes[4];
        if (receive_bytes(sizeBytes, 4) != 4) continue;

        uint32_t imageSize = sizeBytes[0] | (sizeBytes[1]<<8) | (sizeBytes[2]<<16) | (sizeBytes[3]<<24);
        if (imageSize == 0 || imageSize > MAX_IMAGE_SIZE) continue;
        if (receive_bytes(ImageBuffer, imageSize) != (int)imageSize) continue;

        uint32_t totalPixels = imageSize / 3;

        send_byte('D');  // Processing done

        // Send dominant color code for each pixel (0=Red, 1=Green, 2=Blue)
        for (uint32_t i = 0; i < totalPixels; i++) {
            uint32_t index = i * 3;
            uint8_t r = ImageBuffer[index];
            uint8_t g = ImageBuffer[index + 1];
            uint8_t b = ImageBuffer[index + 2];

            uint8_t colorCode;
            if (r >= g && r >= b) colorCode = 0;      // Red
            else if (g >= r && g >= b) colorCode = 1; // Green
            else colorCode = 2;                       // Blue

            send_byte(colorCode);
        }

        // Wait for UART to finish sending
        while(XUartPs_IsSending(&MyUart));
    }

    return 0;
}
