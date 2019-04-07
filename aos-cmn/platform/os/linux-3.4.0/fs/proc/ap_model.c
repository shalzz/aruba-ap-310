#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <asm/cputime.h>

static int apmodel_proc_show(struct seq_file *m, void *v)
{
	extern unsigned xml_ap_model;
	seq_printf(m, "%u", xml_ap_model);
	return 0;
}

static int apmodel_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, apmodel_proc_show, NULL);
}

static const struct file_operations apmodel_proc_fops = {
	.open		= apmodel_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_apmodel_init(void)
{
	proc_create("ap_model", 0, NULL, &apmodel_proc_fops);
	return 0;
}
module_init(proc_apmodel_init);
