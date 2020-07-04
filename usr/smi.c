#include <err.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/io.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>

#define N 1000
#define PORT_SMI_CMD 0x00b2

static inline unsigned long dune_get_ticks(void)
{
    unsigned long a, d;
    asm volatile("rdtsc" : "=a"(a), "=d"(d));
    return ((unsigned long)a) | (((unsigned long)d)<<32);
}

int main(int ac, char **av)
{
    unsigned long a, b, dif;
    int i;
    
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = 1000000;
    
    // time to trigger the smi
    if (0 != ioperm(PORT_SMI_CMD, 1, 1))
        err(EXIT_FAILURE, "ioperm");
        
    a = dune_get_ticks();
    
    int n = ac > 1 ? atoi(av[1]) : N;
for (i=0; i<n; ++i)
//while (1)
{
//    nanosleep(&t, NULL);
    asm volatile ("outb %b0,%w1": :"a" (0xa0), "Nd" (PORT_SMI_CMD));
}
    b = dune_get_ticks();
    dif = b - a;
    printf("this round %lu ticks == %lu us\n", dif / n, dif/2808 / n);
    
    return EXIT_SUCCESS;
}
