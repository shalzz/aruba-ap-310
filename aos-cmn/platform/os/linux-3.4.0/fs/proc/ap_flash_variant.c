#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <asm/cputime.h>
#ifdef CONFIG_ARUBA_AP
#include <oem/aphw.h>
#endif

static int ap_flash_variant_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%u", ap_flash_variant);
	return 0;
}

static int ap_flash_variant_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ap_flash_variant_proc_show, NULL);
}

static const struct file_operations ap_flash_variant_proc_fops = {
	.open		= ap_flash_variant_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_ap_flash_variant_init(void)
{
	proc_create("fl_var", 0, NULL, &ap_flash_variant_proc_fops);
	return 0;
}
module_init(proc_ap_flash_variant_init);
