#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
/*
 *in *.h
*/
static ssize_t kobj_show(struct kobject * , struct kobj_attribute * , char *);
static ssize_t kobj_store(struct kobject * , struct kobj_attribute * , const char * , size_t );

#define SUCCESS 0
#define BUF_LEN 10

static char msg[BUF_LEN] = "Hi1!";
static char *msg_Ptr;
static struct kobject *kobj;

static struct kobj_attribute kobj_att =
	__ATTR(myattr, 0664, kobj_show, kobj_store);

static struct attribute *sysfs_attrs[] = {
	&kobj_att.attr,
	NULL,
};

static struct attribute_group sysfs_group_attrs = {
	.attrs = sysfs_attrs,
};

static int __init init_mod(void)
{
	int ret;
	msg_Ptr = msg;
	kobj = kobject_create_and_add("kobj", kernel_kobj);
	ret = sysfs_create_group(kobj, &sysfs_group_attrs);
	pr_info("init sys\n");
	return ret;
}

static void __exit cleanup_mod(void)
{
	return kobject_put(kobj);
}

static ssize_t kobj_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("Msg read: %s\n", msg_Ptr);
	return SUCCESS;
}

static ssize_t kobj_store(struct kobject *kobj, struct kobj_attribute *attr, 
						const char *buf, size_t count)
{
	int n = count < BUF_LEN ? count : BUF_LEN;
	memcpy(msg_Ptr, buf, n);
	pr_info( "Msg write: %s\n", msg_Ptr);
	msg_Ptr[n-1] = '\0';
	return n;
}

module_init(init_mod);
module_exit(cleanup_mod);

MODULE_AUTHOR("AlexK");
MODULE_DESCRIPTION("myModule");
MODULE_LICENSE("GPL");
