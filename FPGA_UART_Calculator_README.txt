# FPGA UART Calculator

## Overview
This repository contains a simple **UART-based calculator** implemented on an FPGA board.  
The project demonstrates **UART communication, string parsing, and arithmetic operations** in C, making it a beginner-friendly FPGA/embedded systems exercise.  

---

## Components Breakdown

1️⃣ **UART Interface**
- Configured with **115200 baud rate, 8 data bits, no parity, 1 stop bit (8N1)**
- Handles serial communication between the FPGA and a host computer
- Functions:
  - `uart_send_string()` → send data
  - `uart_recv_line()` → receive user input until newline/CR  

2️⃣ **Command Parser**
- Uses `sscanf` to split user input into:
  - **Operation** (`ADD`, `SUB`, `MUL`, `DIV`)
  - **Operands** (two integers)
- Provides **error messages** for invalid or incomplete commands  

3️⃣ **Arithmetic Engine**
- Performs integer-based calculations:
  - `ADD a b` → Addition
  - `SUB a b` → Subtraction
  - `MUL a b` → Multiplication
  - `DIV a b` → Division (**division-by-zero check included**)  

4️⃣ **Interactive Shell**
- Displays a **startup message** and **command prompt (>)**
- Supports `exit` command to terminate program
- Sends results or error messages back over UART  

---

## Execution Flow
1. **Initialize UART:** Configure FPGA UART peripheral and set baud rate  
2. **Welcome Message:** Display instructions on the terminal  
3. **Command Prompt:** Wait for user input (`>`)  
4. **Parse & Execute:**
   - Check command type
   - Perform arithmetic operation  
5. **Output Result:** Send result string back to terminal  
6. **Repeat Until `exit`:** Program ends when user types `exit`  

---

## How to Use
1. Open the project in **Xilinx SDK / Vitis**  
2. Compile and load onto the FPGA board  
3. Connect FPGA UART to PC using a USB cable  
4. Open a serial terminal (e.g., PuTTY, Tera Term, minicom)  
   - Baud Rate: `115200`  
   - Data Bits: `8`  
   - Parity: `None`  
   - Stop Bits: `1`  
5. Run the program and enter commands as shown above  

---

### 🔗 Contributors
- **Maxence Deschenes**

For questions, feel free to reach out!
