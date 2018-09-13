#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
/*
 *in *.h
*/
int init_module(void);
void cleanup_module(void);
static int device_open (struct inode *, struct file *);
static int device_release (struct inode *, struct file *);
static ssize_t device_read (struct file *, char *, size_t, loff_t *);
static ssize_t device_write (struct file *,const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" // in /proc/devices
#define BUF_LEN 80

static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

int init_module(void)
{
	proc_create(DEVICE_NAME, 0666, NULL, &fops);
	pr_info("Посмотри в файл proc, там лежит chardev\n");
	pr_info("Попробуйте команды cat и echo для этого файла\n");
	return SUCCESS;
}

void cleanup_module(void)
{
	remove_proc_entry(DEVICE_NAME, NULL);
}
/*
 *Вызывается когда процесс пытвется открыть файл устройства cat /dev/mycharfile
*/
static int device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "Я в %d раз говорю вам Привет!!!\n", counter++);
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t lenght, loff_t * offset)
{
	return simple_read_from_buffer(buffer, lenght, offset, msg_Ptr, BUF_LEN);
}

static ssize_t device_write(struct file *filp,const char *buff, size_t len, loff_t * off)
{
	int ret = simple_write_to_buffer(msg_Ptr, BUF_LEN, off, buff, len);
	pr_info("Write! %s\n", msg_Ptr);
	return ret;
}