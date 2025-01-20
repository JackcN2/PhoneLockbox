import smbus2
import threading
import time
import RPi.GPIO as GPIO
import os
print("Program started")

# Global variables to track authentication and states
auth = 0
keypad_auth = 0  # To track keypad authentication
rf_auth = 0  # To track RF authentication
finger_auth = 0 # For fingerprint auth
auth_lock = threading.Lock()  # Lock to safely update shared variables across threads
beep1 = 0
beep2 = 0
rsend = 0
web_auth = 0

# I2C setup
I2C_BUS = 1  # I2C bus number (typically 1 on Raspberry Pi)
slave_addresses = {
    "LCD": 0x11,         # Address of the LCD module
    "Keypad": 0x12,
    "RFID": 0x13,
    "Panel": 0x14,
    
    # Address of the Keypad module
}  # Add additional devices if needed
bus = smbus2.SMBus(I2C_BUS)

# GPIO setup
GPIO.setmode(GPIO.BCM)
GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)  # GPIO 13 as input with pull-down resistor (for slave siginaling)
GPIO.setup(23, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)   #For the fingerprint 
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
    global auth, keypad_auth, rf_auth, rsend, beep1, beep2

    while True:
        # Check auth states and execute any needed actions
        if auth == 1 and beep1 == 0:
            beep1 = 1
            send_to_slave(slave_addresses["Panel"], ord('1'))  # Beeps the panel
            print(f"Auth is 1, sent '1' to Control panel.")

        if auth == 2 and beep2 == 0:
            beep2 = 1
            print("Auth reached 2, Opening Box and sending '2' to control panel")
            send_to_slave(slave_addresses["Panel"], ord('2'))  # Beeps the panel
            send_to_slave(slave_addresses["Keypad"], ord('E'))  # Opens the box

        with auth_lock:
            with open('webauth', 'r') as file:
                first_char = file.read(1)
                if first_char == "1":
                    if web_auth == 0:
                        auth += 1
                        web_auth = 1
                    with open('webauth', "r+") as file:
                        file.seek(0)  # Go to the beginning of the file
                        file.write("0")
                        file.truncate()  # Ensure only "1" remains in the file
                    
            if GPIO.input(23) == GPIO.HIGH:
                if finger_auth == 0:
                    finger_auth = 1
                    auth +=1
                
            
        # Check if GPIO 13 (Polling line) is HIGH
        if GPIO.input(13) == GPIO.HIGH:
            print("GPIO 13 is HIGH. Polling all modules.")
            for name, slave_address in slave_addresses.items():
                received_data = read_from_slave(slave_address)

                if received_data is not None:
                    with auth_lock:
                        # Screen function
                        if received_data == ord('V'):  # V from screen to auto open (also from the panel in emergencies)
                            auth = 2

                        # Keypad function
                        elif received_data == ord('K') and keypad_auth == 0:  # 'K' for keypad authentication
                            auth += 1
                            keypad_auth = 1
                            print(f"Received 'K' from {name} (address {hex(slave_address)}), auth incremented to {auth}")

                        # RFID function
                        elif received_data == ord('M') and rf_auth == 0:  # 'M' for RF authentication
                            auth += 1
                            rf_auth = 1
                            print(f"Received 'M' from {name} (address {hex(slave_address)}), auth incremented to {auth}")

                        # Reset button function
                        elif received_data == ord('R'):
                            print(f"Received 'R' from {name} (address {hex(slave_address)})")
                            print("Resetting box")
                            rsend = 0
                            auth = 0
                            rf_auth = 0
                            keypad_auth = 0
                            beep1 = 0
                            beep2 = 0
                            finger_auth = 0
                            web_auth = 0
                            # Send 'R' to all devices
                            for _, addr in slave_addresses.items():
                                send_to_slave(addr, ord('R'))

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
