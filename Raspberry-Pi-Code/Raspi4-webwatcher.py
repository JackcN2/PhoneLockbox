import socketio
import os

# Create a new Socket.IO client instance
sio = socketio.Client()

# Check if the 'webauth' file exists and create it if it doesn't
webauth_file = "webauth"
if not os.path.exists(webauth_file):
    with open(webauth_file, "w") as file:
        file.write("0")  # Default content when the file is created

# Define the event handler for the "password_response" event
@sio.event
def password_response(data):
    if data['result'] == 'correct':
        print("Password is correct!")
        # Write '1' as the first character of the 'webauth' file
        with open(webauth_file, "r+") as file:
            file.seek(0)  # Go to the beginning of the file
            file.write("1")
            file.truncate()  # Ensure only "1" remains in the file
    else:
        print("Incorrect password!")

# Connect to the Flask-SocketIO server on port 5000
sio.connect('http://192.168.2.252:5000')

# Keep the client running to listen for events
sio.wait()
