/**
 * File:	lkmasg1.c
 * Adapted for Linux 5.15 by: John Aedo
 * Forked by: Jake Norris and D'Antae Aronne
 * Class:	COP4600-SP23
 */

#include <linux/module.h>	  // Core header for modules.
#include <linux/device.h>	  // Supports driver model.
#include <linux/kernel.h>	  // Kernel header for convenient functions.
#include <linux/fs.h>		  // File-system support.
#include <linux/uaccess.h>	  // User access copy function support.
#include <linux/slab.h>
#include <linux/gfp.h>

#define DEVICE_NAME "lkmasg1" // Device name.
#define CLASS_NAME "char"	  ///< The device class -- this is a character device driver
#define SUCCESS 0
#define BUF_LEN 1024          // Max length of a message

MODULE_LICENSE("GPL");						 ///< The license type -- this affects available functionality
MODULE_AUTHOR("John Aedo");					 ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("lkmasg1 Kernel Module"); ///< The description -- see modinfo
MODULE_VERSION("0.1");						 ///< A version number to inform users

/**
 * Important variables that store data and keep track of relevant information.
 */
static int major_number; // Stores the major number of the device driver
static int device_open = 0; // Boolean to track whether the device is open

static struct class *lkmasg1Class = NULL;	///< The device-driver class struct pointer
static struct device *lkmasg1Device = NULL; ///< The device-driver device struct pointer

static struct msgs
{
    char *msg;
    int msg_size;
    struct msgs *next;
};

static struct queue
{
    struct msgs *top;
    struct msgs *bottom;
}*q;

//static char msg[BUF_LEN]; // Message the device will give when asked
//static int msg_size; // Size of the message written to the device
static int all_msg_size;// Size of all the messages written to the device
/**
 * Prototype functions for file operations.
 */
static int open(struct inode *, struct file *);
static int close(struct inode *, struct file *);
static ssize_t read(struct file *, char *, size_t, loff_t *);
static ssize_t write(struct file *, const char *, size_t, loff_t *);

/**
 * File operations structure and the functions it points to.
 */
static struct file_operations fops =
	{
		.owner = THIS_MODULE,
		.open = open,
		.release = close,
		.read = read,
		.write = write,
};

/**
 * Initializes module at installation
 */
int init_module(void)
{
	printk(KERN_INFO "lkmasg1: installing module.\n");

	// Allocate a major number for the device.
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0)
	{
		printk(KERN_ALERT "lkmasg1 could not register number.\n");
		return major_number;
	}
	printk(KERN_INFO "lkmasg1: registered correctly with major number %d\n", major_number);

	// Register the device class
	lkmasg1Class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(lkmasg1Class))
	{ // Check for error and clean up if there is
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(lkmasg1Class); // Correct way to return an error on a pointer
	}
	printk(KERN_INFO "lkmasg1: device class registered correctly\n");

	// Register the device driver
	lkmasg1Device = device_create(lkmasg1Class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(lkmasg1Device))
	{								 // Clean up if there is an error
		class_destroy(lkmasg1Class); // Repeated code but the alternative is goto statements
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(lkmasg1Device);
	}
	printk(KERN_INFO "lkmasg1: device class created correctly\n"); // Made it! device was initialized

	return SUCCESS;
}

/*
 * Removes module, sends appropriate message to kernel
 */
void cleanup_module(void)
{
	printk(KERN_INFO "lkmasg1: removing module.\n");
	device_destroy(lkmasg1Class, MKDEV(major_number, 0)); // remove the device
	class_unregister(lkmasg1Class);						  // unregister the device class
	class_destroy(lkmasg1Class);						  // remove the device class
	unregister_chrdev(major_number, DEVICE_NAME);		  // unregister the major number
	printk(KERN_INFO "lkmasg1: Goodbye from the LKM!\n");
	unregister_chrdev(major_number, DEVICE_NAME);
	return;
}

/*
 * Opens device module, sends appropriate message to kernel
 */
static int open(struct inode *inodep, struct file *filep)
{
	// Return an error if the device is already open, and report to the kernel.
	if (device_open)
	{
		printk(KERN_INFO "lkmasg1: device is busy.\n");
		return -EBUSY;
	}

	q = kmalloc(sizeof(struct queue), GFP_KERNEL);

	all_msg_size = 0;
	// Increment to indicate we have now opened the device
	device_open++;

	// Return success upon opening the device without error, and report to the kernel.
	printk(KERN_INFO "lkmasg1: device opened.\n");
	return SUCCESS;
}

/*
 * Closes device module, sends appropriate message to kernel
 */
static int close(struct inode *inodep, struct file *filep)
{
	// Decrement to indicate the device is now closed
	device_open--;

	// Return success upon opening the device without error, and report it to the kernel.
	printk(KERN_INFO "lkmasg1: device closed.\n");

	kfree(q);
	return SUCCESS;
}

/*
 * Reads from device, displays in userspace, and deletes the read data
 */
static ssize_t read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	// Send the message to user space, and store the number of bytes that could not be copied
	// On success, this should be zero.
	int uncopied_bytes = copy_to_user(buffer, q->top->msg, q->top->msg_size);
	struct msgs *ptr = kmalloc(sizeof(struct msgs), GFP_KERNEL);

	// If the message was successfully sent to user space, report this
	// to the kernel and return success.
	if (uncopied_bytes == 0)
	{
		if (q->top != NULL)
		{
			ptr = q->top;
			q->top = q->top->next;
			if (q->top == NULL)
			{
				q->bottom = NULL;
			}
			ptr->next = NULL;
      			all_msg_size -= ptr->msg_size;
			kfree(ptr);
		}		
		printk(KERN_INFO "lkmasg1: read stub");
		return SUCCESS;
	}

	// Return with an error indicating bad address if we cannot copy the message to user space.
	printk(KERN_INFO "lkmasg1: failed to read stub");
	return -EFAULT;
}

/*
 * Writes to the device
 */
static ssize_t write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	// If the file is larger than the amount of bytes the device can hold, return an error.
	int remaining_bytes = -1;
	if ((len + all_msg_size) > BUF_LEN)
	{
		remaining_bytes = BUF_LEN - all_msg_size;
	}

	if (all_msg_size == 0){
		q->top = NULL;
		q->bottom = NULL;
	}

	// Write the input to the device, and update the length of the message.
	// Work as a FIFO queue, so that multiple messages can be stored.
	struct msgs *ptr = kmalloc(sizeof(struct msgs), GFP_KERNEL);

	if (remaining_bytes == -1)
	{
		int msg_mem_size = (len + 1) * sizeof(char);
		ptr->msg = kmalloc(msg_mem_size, GFP_KERNEL);
		sprintf(ptr->msg, "%s", buffer);
	}
	else
	{
		int msg_mem_size = (remaining_bytes + 1) * sizeof(char);
		ptr->msg = kmalloc(msg_mem_size, GFP_KERNEL);
		sprintf(ptr->msg, "%.*s", remaining_bytes, buffer);
	}


	ptr->msg_size = len + 1;
	all_msg_size += len +1;

	ptr->next=NULL;
	if (q->top==NULL && q->bottom==NULL)
	{
		q->top = q->bottom = ptr;
	}
	else
	{
		q->bottom->next=ptr;
		q->bottom=ptr;
	}

	// Return success upon writing the message to the device without error, and report it to the kernel.
	printk(KERN_INFO "lkmasg1: write stub");
	return SUCCESS;
}