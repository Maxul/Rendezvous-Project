#include "output.h" // dprintf
#include "string.h" // memcpy
#include "x86.h" // inb

#include "smx.h"
#include "microbench.h"

#define CONFIG_X86_32
#include "linux/asm/apicdef.h"
#include "apic.h"

/**
    Since the native memset provided by SeaBIOS is so inefficient in 32-bit SMM
    therefore we need to replace it for better performance
*/
#define memset __builtin_memset

void checkpoint(const char *what);
u64 tsc_now();
void randombytes_buf(void * const buf, const size_t size);
void init_ring_buffer();
void pcnet32_handler();
void loopback();
void microbench_time_service();

static inline void enable_sse()
{
    asm volatile("movl %cr0, %eax\n");
    asm volatile("andl %ax, 0xFFFB\n");
    asm volatile("orl %ax, 0x2\n");
    asm volatile("movl %eax, %cr0\n");
    asm volatile("movl %cr4, %eax\n");
    asm volatile("orl $1536, %ax\n");
    asm volatile("movl %eax, %cr4\n");
}

/* Allowed int vectors are from 0x10 to 0xFE */

#define IOAPIC_BASE	(void *)IO_APIC_DEFAULT_PHYS_BASE
#define LOCAL_APIC_BASE	(void *)APIC_DEFAULT_PHYS_BASE

#define NMI_VECTOR			0x02
#define APIC_BASE (LOCAL_APIC_BASE)

void trigger_self_ipi(int vec)
{
    enable_x2apic();
    apic_icr_write(APIC_DEST_SELF | APIC_DEST_PHYSICAL | APIC_DM_FIXED | vec, 0);
    asm volatile ("nop");
}

#define IOREDTBL(x) (0x10 + 2 * x)

#define KBD_IRQ 1
#define NIC_IRQ 11
#define RTC_IRQ 8
int nic_vec = 0, kbd_vec = 0, rtc_vec = 0;

static void show_ioredtbl()
{
    int vector_address = 0;

    for (int i = 0; i < 24; ++i) {
        vector_address = 0xff & ioapic_read_reg(IOREDTBL(i));
        if (0x00 != vector_address)
            dprintf(1, "IRQ #%d\tIOREDTBL[0x%02x] addr: 0x%02x\n", i, IOREDTBL(i), vector_address);
        if (KBD_IRQ == i)
            kbd_vec = vector_address;
        if (NIC_IRQ == i)
            nic_vec = vector_address;
        if (RTC_IRQ == i)
            rtc_vec = vector_address;
    }
    dprintf(1, "\n");
}

void supervisor()
{

//dprintf(1, "%lu\n", rdtscll());
#if 0
    static char isFirstPhrase = 1;
    if (isFirstPhrase) {

        show_ioredtbl();

        if ( 0x26 == rtc_vec ) {
	        ioapic_write_reg(IOREDTBL(RTC_IRQ), APIC_DM_SMI);
            dprintf(1, "=== >>> === >>> QEMU RTC HAS BEEN TAKEN <<< === <<< ===\n");
            trigger_self_ipi(0x26);
        }

        isFirstPhrase = 0;
    } else {
        trigger_self_ipi(0x26);
        dprintf(1, "%lu\n", rdtscll());
    }
#endif
}

#undef memset

