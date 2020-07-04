/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Bryan D. Payne (bdpayne@acm.org)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm_types.h>


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
//#include <linux/sched/signal.h>
#include <linux/sched.h>

#define MYMODNAME "FindOffsets "

static int my_init_module(
    void);
static void my_cleanup_module(
    void);

static int
my_init_module(
    void)
{
    struct task_struct *p = NULL;
    unsigned long commOffset;
    unsigned long tasksOffset;
    unsigned long mmOffset;
    unsigned long pidOffset;
    unsigned long pgdOffset;
    unsigned long addrOffset;

    printk(KERN_ALERT "Module %s loaded.\n\n", MYMODNAME);
    p = current;

    if (p != NULL) {
        commOffset = (unsigned long) (&(p->comm)) - (unsigned long) (p);
        tasksOffset =
            (unsigned long) (&(p->tasks)) - (unsigned long) (p);
        mmOffset = (unsigned long) (&(p->mm)) - (unsigned long) (p);
        pidOffset = (unsigned long) (&(p->pid)) - (unsigned long) (p);
        pgdOffset =
            (unsigned long) (&(p->mm->pgd)) - (unsigned long) (p->mm);
        addrOffset =
            (unsigned long) (&(p->mm->start_code)) -
            (unsigned long) (p->mm);
///
        unsigned long credOffset = 0;
        unsigned long euidOffset = 0;
        unsigned long egidOffset = 0;
        unsigned long stateOffset = 0;

        unsigned long utimeOffset = 0;
        unsigned long stimeOffset = 0;

        stateOffset = (unsigned long) (&(p->state)) - (unsigned long) (p);
        utimeOffset = (unsigned long) (&(p->utime)) - (unsigned long) (p);
        stimeOffset = (unsigned long) (&(p->stime)) - (unsigned long) (p);

        credOffset = (unsigned long) (&(p->cred)) - (unsigned long) (p);
        euidOffset = (unsigned long) (&(p->cred->euid)) - (unsigned long) (p->cred);
        egidOffset = (unsigned long) (&(p->cred->egid)) - (unsigned long) (p->cred);
///
        printk(KERN_ALERT "[domain name] {\n");
        printk(KERN_ALERT "    ostype = \"Linux\";\n");
        printk(KERN_ALERT "    sysmap = \"[insert path here]\";\n");
        printk(KERN_ALERT "    linux_name = 0x%x;\n",
               (unsigned int) commOffset);
        printk(KERN_ALERT "    linux_tasks = 0x%x;\n",
               (unsigned int) tasksOffset);
        printk(KERN_ALERT "    linux_mm = 0x%x;\n",
               (unsigned int) mmOffset);
        printk(KERN_ALERT "    linux_pid = 0x%x;\n",
               (unsigned int) pidOffset);
        printk(KERN_ALERT "    linux_pgd = 0x%x;\n",
               (unsigned int) pgdOffset);
///
        printk(KERN_ALERT "    linux_cred = 0x%x;\n", (unsigned int) credOffset);
        printk(KERN_ALERT "    linux_euid = 0x%x;\n", (unsigned int) euidOffset);
        printk(KERN_ALERT "    linux_egid = 0x%x;\n", (unsigned int) egidOffset);
        printk(KERN_ALERT "    linux_state = 0x%x;\n", (unsigned int) stateOffset);
        printk(KERN_ALERT "    linux_utime = 0x%x;\n", (unsigned int) utimeOffset);
        printk(KERN_ALERT "    linux_stime = 0x%x;\n", (unsigned int) stimeOffset);
///
        printk(KERN_ALERT "}\n");
    } else {
        printk(KERN_ALERT
               "%s: found no process to populate task_struct.\n",
               MYMODNAME);
    }
    
    struct task_struct *task;
    for_each_process(task)
    {
        printk("pid[%d] euid[%d] egid[%d] [%x] -> [%x] %s\n",
            task->pid, task->cred->euid, task->cred->egid,
            virt_to_phys(task), virt_to_phys(task->tasks.next) - tasksOffset, task->comm);
    }
    return 0;
}

static void
my_cleanup_module(
    void)
{
    printk(KERN_ALERT "Module %s unloaded.\n", MYMODNAME);
}

module_init(my_init_module);
module_exit(my_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nilushan Silva");
MODULE_DESCRIPTION("task_struct offset Finder");
