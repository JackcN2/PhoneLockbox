import smbus2
import threading
import time
import RPi.GPIO as GPIO

print("Program started")

# Global variables to track authentication and states
auth = 0
keypad_auth = 0  # To track keypad authentication
rf_auth = 0  # To track RF authentication
auth_lock = threading.Lock()  # Lock to safely update shared variables across threads
beep = 0
rsend = 0

# I2C setup
I2C_BUS = 1  # I2C bus number (typically 1 on Raspberry Pi)
slave_addresses = {
    "LCD": 0x11,         # Address of the LCD module
    "Keypad": 0x12,      # Address of the Keypad module
}  # Add additional devices if needed
bus = smbus2.SMBus(I2C_BUS)

# GPIO setup
GPIO.setmode(GPIO.BCM)
GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)  # GPIO 13 as input with pull-down resistor

# Function to send a byte of data to a slave
def send_to_slave(slave_address, data):
    try:
        bus.write_byte(slave_address, data)
        print(f"Sent {chr(data)} (byte: {data}) to slave at address {hex(slave_address)}")
    except Exception as e:
        print(f"Error sending data to slave at address {hex(slave_address)}: {e}")

# Function to read a byte of data from a slave
def read_from_slave(slave_address):
    try:
        data = bus.read_byte(slave_address)
        print(f"Received {chr(data)} (byte: {data}) from slave at address {hex(slave_address)}")
        return data
    except Exception as e:
        print(f"Error reading data from slave at address {hex(slave_address)}: {e}")
        return None

# Function to handle communication with the specific slave
def poll_slave_when_gpio_high():
    global auth, keypad_auth, rf_auth, rsend, beep

    slave_address = slave_addresses["Keypad"]

    while True:
        # Check if GPIO 13 is HIGH
        if GPIO.input(13) == GPIO.HIGH:
            print("GPIO 13 is HIGH. Polling the Keypad module.")
            received_data = read_from_slave(slave_address)

            if received_data is not None:
                with auth_lock:
                    if received_data == ord('E'):  # 'E' to reset auth
                        auth = 0
                        keypad_auth = 0
                        rf_auth = 0
                        print(f"Received 'E' from Keypad (address {hex(slave_address)}), auth reset to 0.")
                        send_to_slave(slave_address, ord('1'))  # Send '1' to Keypad

                    elif received_data == ord('K') and keypad_auth == 0:  # 'K' for keypad authentication
                        auth += 1
                        keypad_auth = 1
                        print(f"Received 'K' from Keypad (address {hex(slave_address)}), auth incremented to {auth}")
                        send_to_slave(slave_address, ord('V'))  # Send 'V' to Keypad

                    elif received_data == ord('M') and rf_auth == 0:  # 'M' for RF authentication
                        auth += 1
                        rf_auth = 1
                        print(f"Received 'M' from Keypad (address {hex(slave_address)}), auth incremented to {auth}")

                    elif received_data == ord('X'):  # 'X' to reset everything
                        print(f"Received 'X' from Keypad (address {hex(slave_address)})")
                        rsend = 0
                        auth = 0
                        rf_auth = 0
                        keypad_auth = 0
                        beep = 0
                        send_to_slave(slave_address, ord('R'))  # Send 'R' to Keypad

                    if auth == 1:
                        send_to_slave(slave_address, ord('1'))  # Send '1' to Keypad
                        print(f"Auth is 1, sent '1' to Keypad.")

                    if auth == 2 and beep == 0:
                        beep = 1
                        print("Auth reached 2, sending 'E' to Keypad.")
                        send_to_slave(slave_address, ord('E'))  # Send 'E' to Keypad

        time.sleep(0.1)  # Short delay to prevent rapid polling

# Main function
def main():
    try:
        communication_thread = threading.Thread(target=poll_slave_when_gpio_high)
        communication_thread.daemon = True  # Ensure thread exits when main program exits
        communication_thread.start()

        # Keep the main thread running to handle any future tasks
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nProgram terminated by user.")
    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        GPIO.cleanup()
        bus.close()
        print("I2C bus closed and GPIO cleaned up. Program ended.")

if __name__ == "__main__":
    main()
