#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

#define PORT_SMI_CMD             0x00b2

static inline unsigned long rdtscllp(void)
{
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main(void)
{
    unsigned long ticks, diff;
    unsigned long sum = 0;

    if (ioperm(PORT_SMI_CMD, 1, 1) != 0)
        err(EXIT_FAILURE, "ioperm");

#ifdef DEBUG
#define NUM 50
#else
#define NUM 100
#endif

#if 0
    while (1) {
        outb(0xa0, PORT_SMI_CMD);
        sleep(2);
    }
#endif

/* trigger smi */
    int n = NUM;

    ticks = rdtscllp();
    while (n--) {
        outb(0xa0, PORT_SMI_CMD);
    }
    diff = (rdtscllp() - ticks);
    printf("It took %ld us.\n", diff / 2808 / NUM);

    return EXIT_SUCCESS;

}
