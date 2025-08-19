import serial
import time
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def make_red_image(path="red_image.jpg", width=160, height=120):
    """make a pure red RGB image."""
    red_pixels = np.full((height, width, 3), (255, 0, 0), dtype=np.uint8)
    img = Image.fromarray(red_pixels, 'RGB')
    img.save(path)
    print(f"created {width}x{height} red image: {path}")
    return path

def make_green_image(path="green_image.jpg", width=160, height=120):
    """make a pure green RGB image."""
    green_pixels = np.full((height, width, 3), (0, 255, 0), dtype=np.uint8)
    img = Image.fromarray(green_pixels, 'RGB')
    img.save(path)
    print(f"created {width}x{height} green image: {path}")
    return path

def make_blue_image(path="blue_image.jpg", width=160, height=120):
    """make a pure blue RGB image."""
    blue_pixels = np.full((height, width, 3), (0, 0, 255), dtype=np.uint8)
    img = Image.fromarray(blue_pixels, 'RGB')
    img.save(path)
    print(f"created {width}x{height} blue image: {path}")
    return path

def transfer_image_and_get_colors(image_path, port='COM8', baudrate=115200, timeout=10):
    """Send image over UART and receive dominant color per pixel."""
    try:
        with serial.Serial(port, baudrate, timeout=1) as ser:
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            time.sleep(2)

            # Load image
            img = Image.open(image_path).convert('RGB')
            width, height = img.size
            total_pixels = width * height
            img_bytes = img.tobytes()
            image_size = len(img_bytes)
            print(f"Image size: {width}x{height} ({image_size} bytes, {total_pixels} pixels)")

            # Handshake
            print("Initiating handshake...")
            ser.write(b'S')
            start_time = time.time()
            while True:
                if ser.in_waiting > 0:
                    response = ser.read(1)
                    if response == b'A':
                        print("Handshake passed!")
                        break
                if time.time() - start_time > timeout:
                    print("Handshake failed")
                    return None

            # Send image size
            print("Sending image size...")
            ser.write(image_size.to_bytes(4, 'little'))

            # Send image data
            print("Sending image data...")
            chunk_size = 1024
            sent = 0
            while sent < image_size:
                chunk_end = min(sent + chunk_size, image_size)
                chunk = img_bytes[sent:chunk_end]
                bytes_written = ser.write(chunk)
                sent += bytes_written
                if sent % (chunk_size * 10) == 0:
                    print(f"Sent {sent}/{image_size} bytes")
            
            print(f"Sent {sent} bytes total")

            # Wait for 'D' from FPGA
            print("Waiting for processing completion...")
            start_time = time.time()
            while True:
                if ser.in_waiting > 0:
                    response = ser.read(1)
                    if response == b'D':
                        print("FPGA processing complete")
                        break
                if time.time() - start_time > timeout:
                    print("Processing timeout")
                    return None

            # Receive dominant colors - expect total_pixels bytes
            print(f"Receiving dominant colors ({total_pixels} bytes expected)...")
            color_data = []
            bytes_received = 0
            start_time = time.time()
            
            while bytes_received < total_pixels:
                if ser.in_waiting > 0:
                    chunk = ser.read(min(ser.in_waiting, total_pixels - bytes_received))
                    for byte in chunk:
                        if byte == 0:      # FPGA encoding for Red received
                            color_data.append('R')
                        elif byte == 1:    # FPGA encoding for Green received
                            color_data.append('G')
                        elif byte == 2:    # FPGA encoding for Blue received
                            color_data.append('B')
                        else:
                            print(f"Unexpected byte: {byte}")
                        bytes_received += 1
                    
                    if len(color_data) % 1000 == 0 and len(color_data) > 0:
                        progress = len(color_data) / total_pixels * 100
                        print(f"Received {len(color_data)}/{total_pixels} colors ({progress:.0f}%)")
                
                if time.time() - start_time > timeout * 2:
                    print(f"Timeout: received {len(color_data)}/{total_pixels} colors")
                    break
                    
                time.sleep(0.001)
            
            if len(color_data) != total_pixels:
                print(f"Incomplete data: received {len(color_data)}/{total_pixels} colors")
                if len(color_data) == 0:
                    return None
            
            # Count colors and create histogram
            r_count = color_data.count('R')
            g_count = color_data.count('G')
            b_count = color_data.count('B')
            
            print(f"Color counts - R: {r_count}, G: {g_count}, B: {b_count}")
            print(f"Total: {r_count + g_count + b_count} (expected {total_pixels})")
            
            return r_count, g_count, b_count, color_data

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None
    

# Basic Historgram Generator based off count.

def plot_color_histogram(r_count, g_count, b_count):
    """Plot simple color histogram."""
    if r_count is None:
        print("No data to plot")
        return
    
    colors = ['Red', 'Green', 'Blue']
    counts = [r_count, g_count, b_count]
    
    plt.figure(figsize=(8, 6))
    bars = plt.bar(colors, counts, color=['red', 'green', 'blue'], alpha=0.7)
    plt.title('Dominant Color Distribution')
    plt.ylabel('Pixel Count')
    
    # Add count labels on bars
    for bar, count in zip(bars, counts):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + max(counts)*0.01, 
                str(count), ha='center', va='bottom')
    
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    PORT = 'COM8'
    BAUD = 115200

    # Choose which image to test
    # img_path = make_red_image(width=160, height=120)
    img_path = make_green_image(width=160, height=120)
    # img_path = make_blue_image(width=160, height=120)

    result = transfer_image_and_get_colors(img_path, PORT, BAUD)
    
    if result:
        r_count, g_count, b_count, color_data = result
        print(f"\nResults:")
        print(f"Red pixels: {r_count}")
        print(f"Green pixels: {g_count}")
        print(f"Blue pixels: {b_count}")
        
        plot_color_histogram(r_count, g_count, b_count)
        
        # Show first 20 colors received for verification
        if len(color_data) >= 20:
            print(f"First 20 colors: {color_data[:20]}")
    else:
        print("Failed to receive color data")
