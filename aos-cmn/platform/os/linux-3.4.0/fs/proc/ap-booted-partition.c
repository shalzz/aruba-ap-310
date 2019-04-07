#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <asm/cputime.h>

static int ap_booted_partition_proc_show(struct seq_file *m, void *v)
{
	extern unsigned ap_booted_partition;
	seq_printf(m, "%d", ap_booted_partition);
	return 0;
}

static int ap_booted_partition_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ap_booted_partition_proc_show, NULL);
}

static const struct file_operations ap_booted_partition_proc_fops = {
	.open		= ap_booted_partition_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_ap_booted_partition_init(void)
{
	proc_create("booted", 0, NULL, &ap_booted_partition_proc_fops);
	return 0;
}
module_init(proc_ap_booted_partition_init);
