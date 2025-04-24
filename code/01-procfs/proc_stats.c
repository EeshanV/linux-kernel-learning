#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/random.h>

struct my_stats {
  int temp;
  char status[16];
};

struct my_stats *stats;

static int my_show(struct seq_file *s, void *v){

  stats->temp = get_random_u32() % 100;

  if (stats->temp > 75)
  strncpy(stats->status, "OVERHEAT", sizeof(stats->status));
  else
  strncpy(stats->status, "OK", sizeof(stats->status));

  seq_printf(s,"Temp:%d\n", stats->temp);
  seq_printf(s,"status:%s\n", stats->status);

  return 0;
}

static int my_open(struct inode *inode, struct file *file){
  return single_open(file, my_show, stats);
}

static const struct proc_ops my_proc_ops = {
  .proc_open = my_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

static int __init proc_stats_init(void){

  struct proc_dir_entry *entry;

  stats = kmalloc(sizeof(*stats), GFP_KERNEL);
  if(!stats)
    return -ENOMEM;
  stats->temp = 42;
  strncpy(stats->status, "OK", sizeof(stats->status));

  entry = proc_create("my_stats", 0, NULL, &my_proc_ops);
  if(!entry){
    kfree(stats);
    return -ENOMEM;
  }

  printk(KERN_INFO "Proc stats module loaded\n");
  return 0;
}

static void __exit proc_stats_exit(void){

  remove_proc_entry("my_stats", NULL);
  printk(KERN_INFO "Proc stats module unloaded\n");
}

module_init(proc_stats_init);
module_exit(proc_stats_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eeshan");
MODULE_DESCRIPTION("procfs stat module");