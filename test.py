#!/usr/bin/python

from random import randrange
import sys
import os
import random
import string

def generateRandomString(length=1024):
    alphanumeric_characters = string.ascii_letters + string.digits
    return ''.join(random.choice(alphanumeric_characters) for _ in range(length))

def readDevice(bytesToRead, deviceName):
    device = os.open(f"/dev/{deviceName}", os.O_RDONLY)
    fileContents = os.read(device, bytesToRead)
    os.close(device)
    return fileContents

def writeDevice(inputString, deviceName):
    device = os.open(f"/dev/{deviceName}", os.O_RDWR)
    bytesWritten = os.write(device, inputString.encode())
    os.close(device)
    return bytesWritten

def main():
    deviceName = sys.argv[1]
    bufferSize = int(sys.argv[2])
    capacity = bufferSize

    totalBytesWritten = 0
    totalBytesRead = 0
    
    random.seed()

    
    print("TEST CASE 1:  Read Empty Buffer (should return empty string, 5 pts)")
    print(f"read: {readDevice(5, deviceName).decode('UTF-8')}")
          
    print("TEST CASE 2: Fill the buffer (should return 1024, 5 pts)")
    print(f"write: {writeDevice(generateRandomString(bufferSize), deviceName)}")
    capacity = 0

    print("TEST CASE 3: Write to full buffer (should return 0, 5 pts)")
    print(f"write: {writeDevice(generateRandomString(5), deviceName)}")
    
    print("TEST CASE 4: Empty the buffer (should return several lines of text, 5 pts)")
    print(f"read: {readDevice(bufferSize, deviceName).decode('UTF-8')}")
    capacity = bufferSize

    print("TEST CASE 5: arbitrary writes and reads. (10 pts)")
    for i in range(5):
        bytesToWrite = randrange(400)
        if (bytesToWrite > capacity):
            bytesToWrite = capacity
            capacity =0
        else:
            capacity -= bytesToWrite
        print(f"\tShould return {bytesToWrite}")
        totalBytesWritten = totalBytesWritten + bytesToWrite
        print(f"\twrite: {writeDevice(generateRandomString(bytesToWrite), deviceName)}")
    for i in range(5):
        bytesToRead = randrange(400)
        if (bytesToRead > (bufferSize - capacity)):
            bytesToRead = (bufferSize - capacity)
            capacity = bufferSize
        else:
            capacity += bytesToRead
        print(f"\tReading {bytesToRead} characters")
        totalBytesRead = totalBytesRead + bytesToRead
        print(f"\tread: {readDevice(bytesToRead, deviceName).decode('UTF-8')}")
    
    print(f"Total Bytes Written: {totalBytesWritten}")
    print(f"Total Bytes Read: {totalBytesRead}")
    difference = (totalBytesWritten - totalBytesRead)
    actualDiff = difference if difference > -1 else 0

    if (actualDiff == 0):
        message = "empty"
    else:
        message = f"{actualDiff} characters"
    print(f"TEST CASE 6: emptying the remaining buffer (should be {message}, 10 pts).")
    device = os.open(f"/dev/{deviceName}", os.O_RDONLY)
    remainingContents = os.read(device, 1024)
    os.close(device)
    print(f"Remaining contents: [{remainingContents}]")

    print("DONE")

if __name__ == "__main__":
    main()
