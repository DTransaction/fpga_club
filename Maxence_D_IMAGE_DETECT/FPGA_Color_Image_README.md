FPGA Color Image Generator

Overview



This repository contains a simple FPGA-based color image generator.

The project demonstrates UART communication, image data handling, and pixel-level color generation in C, making it a beginner-friendly FPGA/embedded systems exercise.



Components Breakdown



**1️⃣ UART Interface**



Configured with 115200 baud rate, 8 data bits, no parity, 1 stop bit (8N1)



Handles serial communication between the FPGA and a host computer



Functions:



uart\_send\_image() → sends generated image data to PC



uart\_recv\_command() → receives user input or commands



**2️⃣ Image Buffer**



Stores color pixel data in RGB format



Supports configurable resolution (e.g., 640×480)



Allows dynamic updates to pixel values before sending



**3️⃣ Color Generation Engine**



Generates simple patterns or gradients in RGB



Operates entirely on the FPGA using C logic and loops



**4️⃣ Interactive Shell**



Displays a startup message and command prompt (>)



Supports user commands to select patterns, colors, or image modes



Sends generated image data over UART to a connected PC



Execution Flow



Initialize UART: Configure FPGA UART peripheral and set baud rate



Welcome Message: Display instructions on the terminal



Command Prompt: Wait for user input (>)



Parse \& Execute:



Determine requested image pattern or color



Fill image buffer with pixel data



Send Image: Transmit image data over UART



Repeat Until exit: Program ends when user types exit



How to Use



Open the project in Xilinx SDK / Vitis



Compile and load onto the FPGA board



Connect FPGA UART to PC using a USB cable



Open a serial terminal (e.g., PuTTY, Tera Term, minicom)



Baud Rate: 115200



Data Bits: 8



Parity: None



Stop Bits: 1



Run the program and enter commands to generate or send images



🔗 Contributors



Maxence Deschenes



For questions, feel free to reach out!

