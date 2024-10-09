import os
from PIL import Image
from flask import Flask, request, jsonify
import numpy as np
import cv2
import threading
import time
from keras_facenet import FaceNet

app = Flask(__name__)

# Configuration
UPLOAD_FOLDER = 'uploads'
KNOWN_IMAGE_PATH = 'example_face.jpg'  # Path to the known image for comparison
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

TRANSMIT_FILE_PATH = r'C:\Users\glueo\Documents\templates\transmit.txt'
file_change_detected = False  # Shared state variable

# Load the FaceNet model
model = FaceNet()

def convert_to_jpeg(image_path):
    """Convert an image to JPEG format."""
    with Image.open(image_path) as img:
        img = img.convert('RGB')
        img.save(image_path, 'JPEG')

def extract_face(image_path):
    img = cv2.imread(image_path)
    if img is None:
        return None

    # Load DNN face detector
    net = cv2.dnn.readNetFromCaffe(
        'deploy.prototxt.txt',  # Path to the Caffe prototxt file
        'res10_300x300_ssd_iter_140000.caffemodel'  # Path to the model file
    )

    # Prepare the image for detection
    blob = cv2.dnn.blobFromImage(cv2.resize(img, (300, 300)), 1.0, (300, 300), (104.0, 177.0, 123.0))
    net.setInput(blob)
    detections = net.forward()

    # Check for face detections
    if detections.shape[2] > 0:
        confidence = detections[0, 0, 0, 2]
        if confidence > 0.5:  # Confidence threshold
            box = detections[0, 0, 0, 3:7] * np.array([img.shape[1], img.shape[0], img.shape[1], img.shape[0]])
            (x, y, x1, y1) = box.astype("int")
            face = img[y:y1, x:x1]  # Extract face
            return cv2.resize(face, (160, 160))

    return None

def get_embedding(face):
    if face is not None:
        face = np.expand_dims(face, axis=0)  # Add batch dimension
        return model.embeddings(face)
    return None

def compare_faces(embedding1, embedding2):
    distance = np.linalg.norm(embedding1 - embedding2)
    return distance

@app.route('/upload', methods=['POST'])
def upload_file():
    if 'file' not in request.files:
        return jsonify({'error': 'No file part'}), 400

    file = request.files['file']
    if file.filename == '':
        return jsonify({'error': 'No selected file'}), 400
    
    global file_change_detected
    if file_change_detected:
        file_change_detected = False
        return jsonify({'message': 'webauth'}), 220
    try:
        # Save the uploaded file
        file_path = os.path.join(UPLOAD_FOLDER, 'received_image.jpg')
        file.save(file_path)

        # Convert images to JPEG
        convert_to_jpeg(file_path)
        convert_to_jpeg(KNOWN_IMAGE_PATH)

        # Perform face recognition
        face1 = extract_face(file_path)
        face2 = extract_face(KNOWN_IMAGE_PATH)

        if face1 is None or face2 is None:
            return jsonify({'error': 'Face not detected in one or both images.'}), 400

        # Generate embeddings
        embedding1 = get_embedding(face1)
        embedding2 = get_embedding(face2)

        if embedding1 is None or embedding2 is None:
            return jsonify({'error': 'Failed to generate embeddings.'}), 400

        # Compare faces
        distance = compare_faces(embedding1, embedding2)

        # Define a threshold for matching
        threshold = 0.9  # Adjust based on your needs
        
        if distance < threshold:
            print("match")
            return jsonify({'message': 'Face match found!', 'distance': float(distance)}), 200
            
        else:
            return jsonify({'message': 'No face match found.', 'distance': float(distance)}), 200

    except Exception as e:
        return jsonify({'error': f'Error during file upload: {str(e)}'}), 500

@app.route('/check_transmit', methods=['GET'])
def check_transmit():
    global file_change_detected
    if file_change_detected:
        file_change_detected = False  # Reset the state
        print("change")
        return jsonify({'message': 'webauth'}), 220
    return jsonify({'message': 'no change'}), 200

def monitor_file():
    global file_change_detected
    while True:
        try:
            if os.path.exists(TRANSMIT_FILE_PATH):
                with open(TRANSMIT_FILE_PATH, 'r+') as file:
                    content = file.read()
                    if content and content[0] == '1':
                        # Change the first character to '0'
                        file.seek(0)
                        file.write('0' + content[1:])
                        file.truncate()
                        file_change_detected = True  # Set the change detected flag
                        print("File updated.")  # Keep this print for file update notification
                        return("change")
        except Exception as e:
            print(f"Error monitoring file: {e}")

        time.sleep(1)  # Check every second

# Run the file monitor in a separate thread
monitor_thread = threading.Thread(target=monitor_file, daemon=True)
monitor_thread.start()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
