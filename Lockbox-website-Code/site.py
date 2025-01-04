from flask import Flask, request, render_template, redirect, url_for
from flask_socketio import SocketIO, emit

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

# Predefined password
CORRECT_PASSWORD = "ABCD1234"

@app.route("/", methods=["GET", "POST"])
def home():
    if request.method == "POST":
        password = request.form.get("password")
        if password == CORRECT_PASSWORD:
            # Emit to WebSocket clients that the password is correct
            socketio.emit('password_response', {'result': 'correct'})
            return redirect(url_for('result', result='correct'))
        else:
            # Emit to WebSocket clients that the password is incorrect
            socketio.emit('password_response', {'result': 'incorrect'})
            return redirect(url_for('result', result='incorrect'))
    return render_template("index.html")

@app.route("/result/<result>")
def result(result):
    if result == "correct":
        message = "Password is correct!"
        message_class = "correct"
    else:
        message = "Incorrect password, try again."
        message_class = "incorrect"
    
    return render_template("result.html", message=message, message_class=message_class)

# Run Flask and SocketIO on the same port
if __name__ == "__main__":
    socketio.run(app, debug=True, host='0.0.0.0', port=5000)
