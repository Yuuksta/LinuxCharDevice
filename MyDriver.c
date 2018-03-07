#include <linux/init.h>          
#include <linux/module.h>         
#include <linux/device.h>         
#include <linux/kernel.h>         
#include <linux/fs.h>            
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static loff_t device_lseek(struct file *, loff_t , int);

#define SUCCESS 0
#define DEVICE_NAME "MyDriver"
#define  CLASS_NAME  "MyDev"
#define BUF_LEN 80	

static int Major;	
static int Device_Open = 0;
static int Device_Offset = 0;
			
static char msg[BUF_LEN];	
static char *msg_Ptr;
static struct class*  devClass  = NULL;
static struct device* devDevice = NULL;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
	.llseek = device_lseek
};

int init_module(void){
    Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}
	devClass = class_create(THIS_MODULE, CLASS_NAME);
	devDevice = device_create(devClass, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
	printk(KERN_ALERT "Registering char device successed with %d\n", Major);
	return SUCCESS;
}

void cleanup_module(void){
	unregister_chrdev(Major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file){
	if (Device_Open){
		return -1;
	}
	Device_Open++;
	Device_Offset = 0;
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file){
	Device_Open--;		
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
	int bytes_read = 0;
	int len = length;
	msg_Ptr = msg;
	if (*msg_Ptr == 0) return 0;
	msg_Ptr += Device_Offset;
	while (Device_Offset < strlen(msg) && len) {
		put_user(*(msg_Ptr++), buffer++);
		len--;
		bytes_read++;
		Device_Offset++;
	}
	return bytes_read;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t * offset){
	msg_Ptr = msg;
	msg_Ptr += Device_Offset;
	sprintf(msg_Ptr, "%s", buffer);
	Device_Offset += length;
	return length;
}

static loff_t device_lseek(struct file *filp, loff_t offset, int whence){
	if(whence == 0){
		Device_Offset = offset;
	}
	else if(whence == 1){
		Device_Offset += offset;
	}
	else if(whence == 2){
		Device_Offset = strlen(msg) + offset;
	}
	return Device_Offset;
}
