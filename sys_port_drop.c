#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/netfilter.h>
#include <linux/udp.h>
#include <uapi/linux/netfilter_decnet.h>
/*
 *in *.h
*/
static ssize_t kobj_show(struct kobject * , struct kobj_attribute * , char *);
static ssize_t kobj_store(struct kobject * , struct kobj_attribute * , const char * , size_t );
static unsigned int net_filter(void *, struct sk_buff *, const struct nf_hook_state *);

#define SUCCESS 0
#define BUF_LEN 10

static char msg[BUF_LEN] = "7777";
static char *msg_Ptr;
static struct kobject *kobj;
static int port = 7777;

static struct kobj_attribute kobj_att =
	__ATTR(myattr, 0664, kobj_show, kobj_store);

static struct attribute *sysfs_attrs[] = {
	&kobj_att.attr,
	NULL
};

static struct attribute_group sysfs_group_attrs = {
	.attrs = sysfs_attrs
};

static struct nf_hook_ops net_ops __read_mostly = {
	.hook = net_filter,
	.pf = NFPROTO_IPV4,
	.hooknum = NF_DN_LOCAL_IN,
	.priority = NF_DN_PRI_FIRST
};

static int __init init_mod(void)
{
	int ret;
	msg_Ptr = msg;
	kobj = kobject_create_and_add("kobj", kernel_kobj);
	ret = sysfs_create_group(kobj, &sysfs_group_attrs);
	nf_register_net_hook(&init_net, &net_ops);
	pr_info("init sys\n");
	return ret;
}

static void __exit cleanup_mod(void)
{
	nf_unregister_net_hook(&init_net, &net_ops);
	return kobject_put(kobj);
}

static unsigned int net_filter(void *pric, 
			struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	struct udphdr *udph = udp_hdr(skb);
	if (ntohs(udph->dest) == port){
		pr_info("Port %d drop\n", port);
		return NF_DROP;
	}
	else
		return NF_ACCEPT;
}

static ssize_t kobj_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("Port read: %d\n", port);
	return SUCCESS;
}

static ssize_t kobj_store(struct kobject *kobj, struct kobj_attribute *attr, 
						const char *buf, size_t count)
{
	int n = count < BUF_LEN ? count : BUF_LEN;
	memcpy(msg_Ptr, buf, n);
	msg_Ptr[n - 1] = '\0';
	kstrtoint(msg_Ptr, 0, &port);
	pr_info( "Port write: %d\n", port);
	return n;
}

module_init(init_mod);
module_exit(cleanup_mod);

MODULE_AUTHOR("AlexK");
MODULE_DESCRIPTION("myModule");
MODULE_LICENSE("GPL");
