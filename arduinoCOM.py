import serial
import threading
import time
print("program started")
# Global variables to track auth and other authentication states
auth = 0
keypad_auth = 0  # To track keypad authentication
rf_auth = 0  # To track RF authentication
auth_lock = threading.Lock()  # Lock to safely update shared variables across threads
connected_ports = []  # List to keep track of connected serial ports
rsend = 0
beep = 0
# Function to read from a connected serial port
def read_from_port(serial_port, name):
    global auth, keypad_auth, rf_auth, rsend, beep
    while True:
        time.sleep(0.1)
        if serial_port.in_waiting > 0:
            data = serial_port.readline().decode('utf-8').strip()  # Read a full line
            print(f"Received from {name}: {data}")

            with auth_lock:
                if data == 'Box Open' or  data == 'E':  # If 'BoxOpen' is received, set auth to 2
                    auth = 2
                    print(f"Received 'BoxOpen' from {name}, auth set to 2.")

                elif data == 'E':  # If 'V' is received, reset auth and send '1'
                    auth = 0
                    keypad_auth = 0
                    rf_auth = 0
                    print(f"Received 'V' from {name}, auth reset to 0.")
                    for port in connected_ports:
                        port.write(b'1')  # Send '1' to all connected serial ports
                        print(f"Sent '1' to {port.port}")

                elif data == 'K' and keypad_auth == 0:  # If 'K' is received and keypad_auth is 0
                    auth += 1
                    keypad_auth = 1
                    for port in connected_ports: 
                        port.write(b'V')
                    print(f"Received 'K' from {name}, auth incremented to {auth}, keypad_auth set to 1.")

                elif data == 'M' and rf_auth == 0:  # If 'B' is received and rf_auth is 0
                    auth += 1
                    rf_auth = 1
                    print(f"Received 'M' from {name}, auth incremented to {auth}, rf_auth set to 1.")
                elif data == 'X':
                    print(f"Got R")
                    if rsend == 1:
                        rsend = 0
                        beep = 0
                        auth = 0
                    else:
                        for port in connected_ports:
                            port.write(b'R')
                            rsend = 0
                            auth = 0
                            rf_auth = 0
                            keypad_auth = 0
                            beep = 0
                if auth == 1:
                    for port in connected_ports:
                        port.write(b'1')  # Send '1' to all connected serial ports
                        print(f"Sent '1' to {port.port}")
                if auth == 2 and beep == 0:
                    beep = 1
                    print("beepin")
                    for port in connected_ports:
                        port.write(b'2')
                        port.write(b'3')
                        print("Auth reached 2, sending 'E' to all connected ports.")
                        for port in connected_ports:
                            port.write(b'E')  # Send 'E' to all connected serial ports
                    
                  
# Function to establish a connection to a serial port
def connect_to_port(port_name, baudrate):
    while True:
        try:
            # Attempt to connect to the serial port
            serial_port = serial.Serial(port_name, baudrate=baudrate, timeout=1)
            print(f"Connected to {port_name}")
            return serial_port
        except serial.SerialException:
            # Suppress failure messages, wait and retry
            time.sleep(5)

# Function to manage connection attempts for each COM port
def handle_port(port_name, baudrate):
    serial_port = connect_to_port(port_name, baudrate)
    with auth_lock:
        connected_ports.append(serial_port)  # Add to list of connected ports
    read_from_port(serial_port, port_name)

# Function to monitor the auth variable and send 'E' when auth reaches 2
def global_auth_monitor():
    global auth
    while True:
        with auth_lock:
            if auth == 2:
                print("Auth reached 2, sending 'E' to all connected ports.")
                for port in connected_ports:
                    port.write(b'E')  # Send 'E' to all connected serial ports
                    print(f"Sent 'E' to {port.port}")
                     
        time.sleep(1)  # Small delay to prevent CPU overuse

def main():
    baudrate = 9600  # Set your baudrate here
    potential_ports = ['COM10', 'COM12', 'COM13', 'COM12', 'COM13','COM9']  # List of potential COM ports

    threads = []

    # Create and start the global auth monitor thread
    #auth_monitor_thread = threading.Thread(target=global_auth_monitor)
  #  auth_monitor_thread.start()
    #threads.append(auth_monitor_thread)

    # Loop through potential COM ports
    for port in potential_ports:
        # Create a thread to manage each port connection
        port_thread = threading.Thread(target=handle_port, args=(port, baudrate))
        port_thread.start()
        threads.append(port_thread)

    # Keep the main thread alive
    for thread in threads:
        thread.join()

if __name__ == "__main__":
    main()
