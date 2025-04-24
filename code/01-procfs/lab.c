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
    .proc_handler = proc_dointvec,
  },
  { }
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
  val = simple_strol(kbuf, NULL, 0);
  my_enabled = (int)val;
  return count;
}

static int __init my_module_init(void){

}