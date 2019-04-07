#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <asm/cputime.h>

static int osbuild_proc_show(struct seq_file *m, void *v)
{
	extern char aruba_build_string[];
	seq_printf(m, "%s", aruba_build_string);
	return 0;
}

static int osbuild_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, osbuild_proc_show, NULL);
}

static const struct file_operations osbuild_proc_fops = {
	.open		= osbuild_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_osbuild_init(void)
{
	proc_create("osbuild", 0, NULL, &osbuild_proc_fops);
	return 0;
}
module_init(proc_osbuild_init);
