from pyfingerprint.pyfingerprint import PyFingerprint
import RPi.GPIO as GPIO
import time
while True:
    try:
        # Initialize the fingerprint sensor
        f = PyFingerprint('/dev/serial0', 57600, 0xFFFFFFFF, 0x00000000)

        if not f.verifyPassword():
            raise ValueError("The given fingerprint sensor password is wrong!")

    except Exception as e:
        print("The fingerprint sensor could not be initialized!")
        print("Exception message: " + str(e))
        exit(1)

    print("Currently used templates: " + str(f.getTemplateCount()) + "/" + str(f.getStorageCapacity()))

    try:
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(22, GPIO.OUT)
        GPIO.output(22, GPIO.LOW)
        
        print("Waiting for finger to authenticate...")

        # Wait until a finger is read
        while not f.readImage():
            pass

        # Convert the image to a binary template
        f.convertImage(0x01)

        # Search for a matching fingerprint in the sensor's memory
        result = f.searchTemplate()
        positionNumber = result[0]
        accuracyScore = result[1]

        if positionNumber == -1:
            print("No matching fingerprint found.")
        else:
            print("Fingerprint match found!")
            GPIO.output(22, GPIO.HIGH)
            time.sleep(2)
            GPIO.output(22, GPIO.LOW)
            print("Template position: #" + str(positionNumber))
            print("Accuracy score: " + str(accuracyScore))

    except Exception as e:
        print("Authentication failed!")
        print("Exception message: " + str(e))
