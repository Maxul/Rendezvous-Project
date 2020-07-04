#include "string.h"
#include "x86.h"
#include "util.h"

#define NS 1000000000
#define US 1000000
#define MS 1000

//  Linux/GCC

#include "microbench.h"

u32 cpu_freq() {
#if 0
  u32 old, new;
  int delayms = 1000;
  old = rdtscllp();
  mdelay(delayms);
  new = rdtscllp();
  
  u32 diff = (new - old) / (delayms * 1000);
  dprintf(1, "%lu\n", diff);
#endif
  u32 diff = 2808;
  return diff;
}
