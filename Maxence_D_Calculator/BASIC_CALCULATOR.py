import serial
import time

# --- CONFIG ---
PORT = "COM8"         # Update with your ZYBO port
BAUDRATE = 115200
TIMEOUT = 1

# --- Open Serial Port ---
ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
time.sleep(2)  # Wait for UART to initialize

print("Connected to FPGA UART Calculator.")
print("Type commands: ADD 4 7, SUB 10 3, MUL 6 5, DIV 20 4")
print("Type 'exit' to quit.\n")

try:
    while True:
        cmd = input("Command> ")
        if cmd.lower() == "exit":
            ser.write(b"exit\r\n")
            break

        # Send command with CRLF
        ser.write((cmd + "\r\n").encode())

        # Read response(s) from FPGA
        time.sleep(0.1)  # small delay
        while True:
            response = ser.readline().decode(errors="ignore").strip()
            if not response:
                break
            print(response)

finally:
    ser.close()
    print("Serial port closed.")
