#!/usr/bin/python

from random import randrange
from threading import Thread
import os
import time

###
#   Author: John Aedo
#   for COP 4600
#   Kernel Module Synchronization Test
#   Version 1.0
#
#   TODO:
#       - Allow for variable reads and writes on each iteration. i.e., random bytes read/written on each iter.
#       - Get the final output of the buffer at the end working.  Currently it displays garbage characters despite
#           dmesg output being correct.
#       - Convert writeDevice to system write() for consistency.
###



def readDevice(readBytes = 5, loopCount = 10):
    for x in range(loopCount):
        # Using the system call version of read() to have more control over how many bytes are read from the device.
        device = os.open("/dev/charkmod-out", os.O_RDONLY)
        fileContents = os.read(device, readBytes)
        os.close(device)
        print(f"read[{x}]: [{fileContents.decode("UTF-8")}]")
        
def writeDevice(inputString, loopCount = 10):
    for x in range(loopCount):
        device = open("/dev/charkmod-in", "w")
        bytesWritten = device.write(inputString)
        device.close()
        print(f"write[{x}]: {bytesWritten} bytes written.")

def main():
    writerThread = Thread(target=writeDevice, args=("Hello", 10))
    readerThread = Thread(target=readDevice, args=(3, 12))

    writerThread.start()
    readerThread.start()

    writerThread.join()
    readerThread.join()

    print("Done!")

#    device = os.open("/dev/charkmod-out", os.O_RDONLY)
#    remainingContents = os.read(device, 48)
#    os.close(device)
#    print(f"Remaining contents: [{remainingContents}]")

if __name__ == "__main__":
    main()
