#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <asm/uaccess.h>	

#define DEVICE_NAME	"module_param_example_device"

// echo 42 > /sys/module/module_param/parameters/mp_setting
int		mp_setting	= 0;
module_param(mp_setting, int, 0644);

static int	major_number;
static int	isDevice_open = 0;

static int		device_open(struct inode *, struct file *);
static int		device_release(struct inode *, struct file *);
static ssize_t	device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0) {
		printk(KERN_ALERT "Registering device failed with code %d.\n", major_number);
		return (major_number);
	}

	printk(KERN_INFO "I was assigned major number %d.  To talk to\n", major_number);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/module_param_example_device c %d 0'.\n", major_number);
	printk(KERN_INFO "Try various minor numbers.  Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
	
	printk(KERN_INFO "module_param_example inserted.\n");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_INFO "module_param_example removed.\n");
}


/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
	if (isDevice_open)
		return -EBUSY;

	isDevice_open++;
	printk(KERN_INFO "mp_setting is equal to %d\n", mp_setting);

	return 0;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	isDevice_open--;

	// module_put(THIS_MODULE); 
	return 0;
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
