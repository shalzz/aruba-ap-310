#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <asm/cputime.h>
#include <oem/aptype.h>
#include <oem/aphw.h>

static int ap_fips_kat_proc_show(struct seq_file *m, void *v)
{
	extern aphw_t __ap_hw_info[];

	seq_printf(m, "%u", __ap_hw_info[xml_ap_model].fips_kat_support);
	return 0;
}

static int ap_fips_kat_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ap_fips_kat_proc_show, NULL);
}

static const struct file_operations ap_fips_kat_proc_fops = {
	.open		= ap_fips_kat_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_ap_fips_kat_init(void)
{
	proc_create("ap_fips_kat", 0, NULL, &ap_fips_kat_proc_fops);
	return 0;
}
module_init(proc_ap_fips_kat_init);
