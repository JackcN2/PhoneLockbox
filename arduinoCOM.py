import smbus2
import threading
import time

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

# Function to handle communication with all slaves
def communicate_with_slaves():
    global auth, keypad_auth, rf_auth, rsend, beep

    while True:
        for name, address in slave_addresses.items():
            received_data = read_from_slave(address)

            if received_data is not None:
                with auth_lock:
                    if received_data == ord('E'):  # 'E' to reset auth
                        auth = 0
                        keypad_auth = 0
                        rf_auth = 0
                        print(f"Received 'E' from {name} (address {hex(address)}), auth reset to 0.")
                        for _, addr in slave_addresses.items():
                            send_to_slave(addr, ord('1'))  # Send '1' to all slaves

                    elif received_data == ord('K') and keypad_auth == 0:  # 'K' for keypad authentication
                        auth += 1
                        keypad_auth = 1
                        print(f"Received 'K' from {name} (address {hex(address)}), auth incremented to {auth}")
                        for _, addr in slave_addresses.items():
                            send_to_slave(addr, ord('V'))  # Send 'V' to all slaves

                    elif received_data == ord('M') and rf_auth == 0:  # 'M' for RF authentication
                        auth += 1
                        rf_auth = 1
                        print(f"Received 'M' from {name} (address {hex(address)}), auth incremented to {auth}")

                    elif received_data == ord('X'):  # 'X' to reset everything
                        print(f"Received 'X' from {name} (address {hex(address)})")
                        rsend = 0
                        auth = 0
                        rf_auth = 0
                        keypad_auth = 0
                        beep = 0
                        for _, addr in slave_addresses.items():
                            send_to_slave(addr, ord('R'))  # Send 'R' to all slaves

                    if auth == 1:
                        for _, addr in slave_addresses.items():
                            send_to_slave(addr, ord('1'))  # Send '1' to all slaves
                        print(f"Auth is 1, sent '1' to all slaves.")

                    if auth == 2 and beep == 0:
                        beep = 1
                        print("Auth reached 2, sending 'E' to all slaves.")
                        for _, addr in slave_addresses.items():
                            send_to_slave(addr, ord('E'))  # Send 'E' to all slaves

        time.sleep(0.1)  # Short delay to prevent overloading the I2C bus

# Main function
def main():
    try:
        communication_thread = threading.Thread(target=communicate_with_slaves)
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
        bus.close()
        print("I2C bus closed. Program ended.")

if __name__ == "__main__":
    main()