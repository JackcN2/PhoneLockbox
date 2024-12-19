from pyfingerprint.pyfingerprint import PyFingerprint
import time

try:
    # Initialize the fingerprint sensor
    f = PyFingerprint('/dev/serial0', 57600, 0xFFFFFFFF, 0x00000000)

    if (f.verifyPassword() == False):
        raise ValueError("The given fingerprint sensor password is wrong!")

except Exception as e:
    print("The fingerprint sensor could not be initialized!")
    print("Exception message: " + str(e))
    exit(1)

print("Currently used templates: " + str(f.getTemplateCount()) + "/" + str(f.getStorageCapacity()))

try:
    print("Waiting for finger to enroll...")

    # Wait for the finger to be read for the first time
    while (f.readImage() == False):
        pass

    # Convert the image to a binary template
    f.convertImage(0x01)

    # Check if the fingerprint is already enrolled
    result = f.searchTemplate()
    positionNumber = result[0]

    if positionNumber >= 0:
        print("Fingerprint already exists at position #" + str(positionNumber))
        exit(0)

    print("Remove finger...")
    time.sleep(2)

    print("Place the same finger again...")

    # Wait for the same finger to be read a second time
    while (f.readImage() == False):
        pass

    # Convert the second image
    f.convertImage(0x02)

    # Compare the two images
    if (f.compareCharacteristics() == 0):
        raise Exception("Fingers do not match")

    # Create a template
    f.createTemplate()

    # Store the template in the next available slot
    positionNumber = f.storeTemplate()
    print("Fingerprint enrolled successfully!")
    print("Stored at position #" + str(positionNumber))

except Exception as e:
    print("Enrollment failed!")
    print("Exception message: " + str(e))
