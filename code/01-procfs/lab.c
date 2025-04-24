/*
Build a kernel module that creates a `/proc/my_module` directory.

Inside this directory, add two entries:
  - `status`: read-only, uses `seq_file` to display a counter.
  - `control`: read-write, toggles an internal flag when written to.

Register a `sysctl` entry at `/proc/sys/kernel/my_module/enabled` that reflects the same flag as `control`.

Ensure all created proc entries and the sysctl entry are properly removed when the module is unloaded.
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/random.h>
#include <linux/sysctl.h>

#define DIR_NAME "lab"
#define FILE_STATS "stats"
#define FILE_VAL "value"

static struct proc_dir_entry *proc_dir, *status_entry, *control_entry;
static struct ctl_table_header *sysctl_hdr;

static int my_enabled = 0;

static int status_show(struct seq_file *s, void *v);
static int status_open(struct inode *inode, struct file *file);
static ssize_t control_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);

static const struct proc_ops status_ops = {
  .proc_open = status_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

static const struct proc_ops control_ops = {
  .proc_open = status_open,
  .proc_read = seq_read,
  .proc_write = control_write,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

static struct ctl_table  my_sysctl_table[] = {
  {
    .procname = "enabled",
    .data = &my_enabled,
    .maxlen = sizeof(int),
    .mode = 0644,
    .proc_handler = proc_dointvec
  },
};

static int status_show(struct seq_file *s, void *v){
  static int counter;
  seq_printf(s, "counter=%d\n", counter++);
  return 0;
}

static int status_open(struct inode *inode, struct file *file){
  return single_open(file, status_show, NULL);
}

static ssize_t control_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos){
  char kbuf[16];
  long val;
  if (count >= sizeof(kbuf))
    return -EINVAL;
  if (copy_from_user(kbuf, buf, count))
    return -EFAULT;
  kbuf[count] = '\0';
  val = simple_strtol(kbuf, NULL, 0);
  my_enabled = (int)val;
  return count;
}

static int __init procfs_lab_init(void){

  proc_dir = proc_mkdir(DIR_NAME, NULL);
  if (!proc_dir){
    pr_err("procfs_lab: unable to create /proc/%s\n", DIR_NAME);
    return -ENOMEM;
  }

  status_entry = proc_create(FILE_STATS, 0444, proc_dir, &status_ops);
  if (!status_entry) {
    pr_err("procfs_lab: failed to create %s/%s\n", DIR_NAME, FILE_STATS);
    goto err_remove_dir;
  }

  control_entry = proc_create(FILE_VAL, 0644, proc_dir, &control_ops);
  if (!control_entry) {
      pr_err("procfs_lab: failed to create %s/%s\n", DIR_NAME, FILE_VAL);
      goto err_remove_stats;
  }

  sysctl_hdr = register_sysctl("kernel/" DIR_NAME, my_sysctl_table);
  if (!sysctl_hdr) {
      pr_err("procfs_lab: register_sysctl(\"kernel/%s\") failed\n", DIR_NAME);
      goto err_remove_control;
  }

  pr_info("procfs_lab: module loaded\n");
  return 0;

err_remove_control:
  remove_proc_entry(FILE_VAL, proc_dir);
err_remove_stats:
  remove_proc_entry(FILE_STATS, proc_dir);
err_remove_dir:
  remove_proc_entry(DIR_NAME, NULL);
  return -ENOMEM;
}

static void __exit procfs_lab_exit(void)
{
  if (sysctl_hdr)
      unregister_sysctl_table(sysctl_hdr);

  if (control_entry)
      remove_proc_entry(FILE_VAL, proc_dir);

  if (status_entry)
      remove_proc_entry(FILE_STATS, proc_dir);

  if (proc_dir)
      remove_proc_entry(DIR_NAME, NULL);

  pr_info("procfs_lab: module unloaded\n");
}


module_init(procfs_lab_init);
module_exit(procfs_lab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("procfs stat module");