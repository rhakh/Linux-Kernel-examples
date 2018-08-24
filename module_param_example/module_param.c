#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	

int		setting	= 0;
module_param(setting, int, 0644);

static int		device_open(struct inode *, struct file *);
static int		device_release(struct inode *, struct file *);
static ssize_t	device_read(struct file *, char *, size_t, loff_t *);
static ssize_t	device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

// void	loop(void);
// void	loop(void)
// {
// 	struct timespec	start, now;

// 	getnstimeofday(&start);
// 	while (1)
// 	{
// 		getnstimeofday(&now);
// 		if (now.tv_sec - start.tv_sec >= 1) {
// 			if (setting != 0)
// 				printk(KERN_INFO "setting = %d\n", setting);
// 			getnstimeofday(&start);
// 		}
// 	}
// }

int init_module(void)
{
	printk(KERN_INFO "module_param_example inserted.\n");
	
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "module_param_example removed.\n");
}


/*
 * Methods
 */

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
	/*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, 
	 * return 0 signifying end of file 
	 */
	if (*msg_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *msg_Ptr) {

		/* 
		 * The buffer is in the user data segment, not the kernel 
		 * segment so "*" assignment won't work.  We have to use 
		 * put_user which copies data from the kernel data segment to
		 * the user data segment. 
		 */
		put_user(*(msg_Ptr++), buffer++);

		length--;
		bytes_read++;
	}

	/* 
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*  
 * Called when a process writes to dev file: echo "hi" > /dev/hello 
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}
