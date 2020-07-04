// detect system slowdown overhead
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#define NUM 10000

static inline unsigned long rdtscllp(void)
{
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	return ((unsigned long) a) | (((unsigned long) d) << 32);
}


int main()
{
    static unsigned long now[1024];
    
    for (int i = 0; i < NUM; i++) {
        sleep(1);
        now[i] = rdtscllp();
    }
    
    for (int i = 1; i < NUM; i++) {
        printf("%lu\n", now[i]-now[i-1]);
    }
    
    return 0;
}

