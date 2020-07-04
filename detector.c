
static inline unsigned long rdtscllp(void)
{
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	return ((unsigned long) a) | (((unsigned long) d) << 32);
}

#define NUM 60
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

