#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
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
#define SIZE_MSGPTR 8192

static int Major;
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
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
		pr_info("Регистрация char девайса не %d не удалась", Major);
		return Major;
	}

	pr_info("Присвоен большой основной номер %d\n", Major);
	pr_info("Для того чтобы обратится к девайсу, создайте файл в папке dev, 'mknod /dev/%s c %d 0'\n", DEVICE_NAME, Major);
	pr_info("Попробуйте различные ноомера. Попробуйте команды cat и echo для файла\n");
	pr_info("Удалите файл и модуль после завершения\n");

	return SUCCESS;
}

void cleanup_module(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
/*
 *	if(ret < 0)
 *		pr_info("Ошибка в unregister_chrdev %d\n", ret);
*/
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
/*	int bytes_read = 0;
	if (*msg_Ptr == 0)
		return 0;
*/
	return simple_read_from_buffer(buffer, lenght, offset, msg_Ptr, SIZE_MSGPTR);
/*
	while (lenght && *msg_Ptr){
		put_user(*(msg_Ptr++), buffer++);
		lenght--;
		bytes_read++;
	}
	return bytes_read;
*/
}

static ssize_t device_write(struct file *filp,const char *buff, size_t len, loff_t * off)
{

	pr_info("Прошу прощения, такая операция не поддерживается\n");
	return -EINVAL;

}