#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <asm/param.h>
#include <linux/jiffies.h>

static int jtime;
/* This function is called when the module is loaded. */
int hanwen_init(void)
{
	printk(KERN_INFO "Loading Hanwen's Module\n");
	printk(KERN_INFO "GOLDEN RATIO PRIME: %lu\n", GOLDEN_RATIO_PRIME);
	printk(KERN_INFO "HZ: %d, Jiffies: %d\n", HZ, jiffies);
	jtime=jiffies;
	return 0;
}

/* This function is called when the module is removed. */
void hanwen_exit(void)
{
	printk(KERN_INFO "GCD(3300,24): %lu\n", gcd(3300,24));
	printk(KERN_INFO "Removing Hanwen's Module\n");
	printk(KERN_INFO "Jiffies: %d\n", jiffies);
}

/* Macros for registering module entry and exit points */
module_init(hanwen_init);
module_exit(hanwen_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hanwen's First Module");
MODULE_AUTHOR("Hanwen");