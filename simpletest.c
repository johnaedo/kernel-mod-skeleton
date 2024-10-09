/**
 * @file   test.c
 * @author Derek Molloy
 * @date   7 April 2015
 * @version 0.1
 * @brief  A Linux user space program that communicates with the charkmod.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/charkmod.
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 *
 * Adapted for COP 4600 by John Aedo
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int errno = 0;
    if (argc != 3) {
        printf("Usage:  simpletest <module name> <buffer size>\n");
    }
    char devicename[10] = argv[1];
    int buffer_length = atoi(argv[2]);
    char devicepath[15] = "/dev/";
    strcat(devicepath, devicename);

    char *receive = (char *) malloc(buffer_length * sizeof(char));

    int ret, fd;
    char stringToSend[buffer_length];
    strcpy(stringToSend, "");
    printf("Starting device test code example...\n");
    fd = open("/dev/charkmod", O_RDWR); // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }
    printf("Type in a short string to send to the kernel module:\n");
    scanf("%[^\n]%*c", stringToSend); // Read in a string (with spaces)

    printf("Writing message to the device [%s].\n", stringToSend);
    ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
    if (ret < 0)
    {
        perror("Failed to write the message to the device.");
        return errno;
    }

    printf("Press ENTER to read back from the device...\n");
    getchar();

    printf("Reading from the device...\n");
    ret = read(fd, receive, buffer_length); // Read the response from the LKM
    if (ret < 0)
    {
        perror("Failed to read the message from the device.");
        return errno;
    }
    printf("The received message is: [%s]\n", receive);
    printf("End of the program\n");
    return 0;
}
