## Learning Objective 
To gain a fuller understanding of kernel modules by creating one.

## Development 
Use gcc and the C programming language, on the version of Linux we have in our VM.

## Assignment
Write a character-mode Linux device driver as a kernel module.

### Your driver must:

- Store bytes written to it up to a constant buffer size (at least 1KB)
- Allow them to be read back out in FIFO fashion
- Remove them from the buffer as they are read back out

#### Additionally, the driver must handle the following overflow conditions:

- If not enough buffer is available to store a write request, the driver must store only up to the amount available
- If not enough data is available to service a read request, the driver must respond with only the amount available (including 0 bytes)

### The driver will need to:

- Successfully initialize and de-initialize itself, including registering itself and obtaining a new major device number.
- Report using **printk** each time its character device is opened, closed, read or written.  Examples:
	- kmod: device opened
	- kmod: device closed
	- kmod: X bytes written
	- kmod: Y bytes read

To do this, you will need to implement the expected device driver functions to:

#### Initialize the kernel module, _including registering the device_.

See the sample file for this process.

#### De-initialize the kernel module.

…including de-registering the device.  Again, please reference the sample code.

#### Open the device.

#### Close the device.

#### Read from the device.

#### Write to the device.

Below are some resources that will help you structure a device driver.  Feel free to ask the professor and the teaching assistants for advice, but you should read over the relevant bits of these resources first.

Peter Jay Salzman, Michael Burian, Ori Pomerantz.  _The Linux Kernel Module Programming Guide._  [http://www.tldp.org/LDP/lkmpg/2.6/html/](http://www.tldp.org/LDP/lkmpg/2.6/html/)

Derek Molloy.  _Writing a Linux Kernel Module - Part 2: A Character Device_.  [http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/](http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/)

## Submitting

Zip up your code, and upload it to Webcourses.  Include the makefile.
