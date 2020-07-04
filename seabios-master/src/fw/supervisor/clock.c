#include "biosvar.h" // GET_LOW
#include "config.h"  // CONFIG_*
#include "output.h"  // dprintf
#include "stacks.h"  // yield
#include "util.h"    // timer_setup
#include "x86.h"     // cpuid

#include <time.h>

#include "microbench.h"

#define HPET_COUNTER 0x0f0
#define PORT_PIT_COUNTER0 0x0040
#define PORT_PIT_MODE 0x0043
#define PM_SEL_READBACK (3 << 6)
#define PM_READ_COUNTER0 (1 << 1)
#define PM_READ_VALUE (1 << 4)

#include "linux/asm/apicdef.h"

inline unsigned int hpet_readl(unsigned int a)
{
  return readl(BUILD_HPET_ADDRESS + a);
}

extern u16 TimerPort;
extern u8 ShiftTSC;

time_t rtc_now ( void );

void checkpoint(const char *what)
{
    dprintf(1, "%s : TSC = %llu\n", what, rdtscllp());
}

u64 tsc_now()
{
  static u64 tsc_begin, tsc_now;

  // if (CONFIG_TSC_TIMER && !port)
  // Read from CPU TSC
  tsc_now = rdtscll() >> GET_GLOBAL(ShiftTSC);
#if 0
  dprintf(1, "CPU TSC read %llu, diff %llu\n", tsc_now, tsc_now - tsc_begin);
  tsc_begin = rdtscll() >> GET_GLOBAL(ShiftTSC);
#endif
  return tsc_now;
}

u64 pmtimer_now()
{
  static u64 pm_begin, pm_now;

  u16 port = GET_GLOBAL(TimerPort);
  if (CONFIG_PMTIMER && port != PORT_PIT_COUNTER0)
    // Read from PMTIMER
    pm_now = timer_adjust_bits(inl(port), 0xffffff);
#if 0
  dprintf(1, "PMTIMER read %llu, diff %llu\n", pm_now, pm_now - pm_begin);
  pm_begin = timer_adjust_bits(inl(port), 0xffffff);
#endif
  return pm_now;
}

u64 pit_now()
{
  static u64 pit_begin, pit_now;

  // Read from PIT.
  outb(PM_SEL_READBACK | PM_READ_VALUE | PM_READ_COUNTER0, PORT_PIT_MODE);
  u16 v = inb(PORT_PIT_COUNTER0) | (inb(PORT_PIT_COUNTER0) << 8);
  pit_now = timer_adjust_bits(v, 0xffff);
#if 0
  dprintf(1, "PIT read %llu, diff %llu\n", pit_now, pit_now - pit_begin);
  pit_begin = timer_adjust_bits(v, 0xffff);
#endif
  return pit_now;
}

u64 hpet_now()
{
  static u64 hpet_begin, hpet_now;

  hpet_now = hpet_readl(HPET_COUNTER);
#if 0
  dprintf(1, "HPET read %llu, diff %llu\n", hpet_now, hpet_now - hpet_begin);
  hpet_begin = hpet_readl(HPET_COUNTER);
#endif
  return hpet_now;
}

void time_service(void)
{
    tsc_now();
    pmtimer_now();
    pit_now();
/*
    hpet_now();
    rtc_now();
*/
}
