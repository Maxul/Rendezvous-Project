#include <linux/module.h>       // Needed by all modules
#include <linux/kernel.h>       // KERN_INFO
#include <linux/sched.h>        // for_each_process, pr_info
#include <linux/signal.h>

int isgraph(int c) { return (unsigned)c - 0x21 < 0x5e; }

void _print_hex(const char *what, const void *v, const unsigned long l) {
  const unsigned char *p = v;
  unsigned long x, y = 0, z;
  pr_info("%s contents:", what);
  
  for (x = 0; x < l;) {
    pr_info("%02x ", p[x]);
    if (!(++x % 16) || x == l) {
      if ((x % 16) != 0) {
        z = 16 - (x % 16);
        if (z >= 8)
          pr_info(" ");
        for (; z != 0; --z) {
          pr_info("   ");
        }
      }
      pr_info(" | ");
      for (; y < x; y++) {
        if ((y % 8) == 0)
          pr_info(" ");
        if (isgraph(p[y]))
          pr_info("%c", p[y]);
        else
          pr_info(".");
      }
      pr_info("\n");
    } else if ((x % 8) == 0) {
      pr_info(" ");
    }
  }
}

void procs_info_print(void)
{
    char *target = "bash";
    
    struct task_struct* task_list;

    size_t process_counter = 0;
    for_each_process(task_list) {
//        pr_info("pid[%4d] euid[%4d] egid[%4d] state[%1d] [%p] -> [%p] %s\n",
//        task_list->pid, task_list->cred->euid, task_list->cred->egid, task_list->state, task_list, (task_list->tasks.next), task_list->comm);
//        uint64_t next = (task_list->tasks.next);
//        pr_info("pid[%4d] [%p] -> [%p] %s\n", task_list->pid, virt_to_phys(task_list), virt_to_phys(next-0x788), task_list->comm);
pr_info("pid %d cred %p euid %p egid %p", task_list->pid, virt_to_phys(&task_list->cred), virt_to_phys(&task_list->cred->euid), virt_to_phys(&task_list->cred->egid));
        ++process_counter;
//            printk("uid VA %p ==> %p GA !!!\n", &(task_list->cred->euid), virt_to_phys(&(task_list->cred->euid)));
//            printk("gid VA %p ==> %p GA !!!\n", &(task_list->cred->egid), virt_to_phys(&(task_list->cred->egid)));
    }
    printk(KERN_INFO "== Number of process: %zu\n", process_counter);
#if 0
    struct task_struct *va_init = &init_task;
    printk("VA %p ==> %p GA !!!\n", va_init, virt_to_phys(va_init));
    uint64_t next = (task_list->tasks.next);
    pr_info("pid[%4d] [%p] -> [%p] %p\n", task_list->pid, task_list, next-0x788, virt_to_phys(&(task_list->comm)));
    
    pr_info("swapper task_struct %p -> %p\n", virt_to_phys(va_init), virt_to_phys(va_init->tasks.next));

pr_info("task %p -> %p\n", va_init, virt_to_phys(va_init));
uint64_t aaa = va_init->tasks;
pr_info("tasks %p -> %p\n", (va_init->tasks), virt_to_phys(&(va_init->tasks)));
pr_info("task.next %p\n", &(va_init->tasks.next));
    pr_info("size task_struct %lx\n", sizeof (struct task_struct));
#endif
}

int init_module(void)
{
        printk(KERN_INFO "[ INIT ==\n");
        procs_info_print();
        return 0;
}

void cleanup_module(void)
{
        printk(KERN_INFO "== CLEANUP ]\n");
}

MODULE_LICENSE("MIT");

