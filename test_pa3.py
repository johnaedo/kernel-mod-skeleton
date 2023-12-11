#!/usr/bin/python

from random import randrange
from threading import Thread
import os
import time

def readDevice(readBytes, loopCount):
    for x in range(loopCount):
        device = os.open("/dev/charkmod-out", os.O_RDONLY)
        fileContents = os.read(device, readBytes)
        os.close(device)
        print(f"read[{x}]: [{fileContents.decode("UTF-8")}]")
        
def writeDevice(inputString, loopCount):
    for x in range(loopCount):
        device = open("/dev/charkmod-in", os.O_RDWR)
        bytesWritten = device.write(device, inputString)
        os.close(device)
        print(f"write[{x}]: {bytesWritten} bytes written.")

def main():
    writerThread = Thread(target=writeDevice, args=("Hello", 4))
    readerThread = Thread(target=readDevice, args=(3, 7))

    writerThread.start()
    readerThread.start()

    writerThread.join()
    readerThread.join()

    print("Done!")

    device = os.open("/dev/charkmod-out", os.O_RDONLY)
    remainingContents = os.read(device, 48)
    os.close(device)
    print(f"Remaining contents: [{remainingContents}]")

if __name__ == "__main__":
    main()
