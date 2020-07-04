#include "output.h"

void serial_debug_puts(char *s, int len)
{
    for (int i=0; i<len; ++i)
    {
        outb(s[i], 0x3f8);
    }
    outb('\n', 0x3f8);
}

int isgraph(int c) { return (unsigned)c - 0x21 < 0x5e; }

void _print_hex(const char *what, const void *v, const unsigned long l) {
  const unsigned char *p = v;
  unsigned long x, y = 0, z;
  dprintf(1, "%s contents: \n", what);
  for (x = 0; x < l;) {
    dprintf(1, "%02x ", p[x]);
    if (!(++x % 16) || x == l) {
      if ((x % 16) != 0) {
        z = 16 - (x % 16);
        if (z >= 8)
          dprintf(1, " ");
        for (; z != 0; --z) {
          dprintf(1, "   ");
        }
      }
      dprintf(1, " | ");
      for (; y < x; y++) {
        if ((y % 8) == 0)
          dprintf(1, " ");
        if (isgraph(p[y]))
          dprintf(1, "%c", p[y]);
        else
          dprintf(1, ".");
      }
      dprintf(1, "\n");
    } else if ((x % 8) == 0) {
      dprintf(1, " ");
    }
  }
}

int compare_testvector(const void *is, const unsigned long is_len,
                       const void *should, const unsigned long should_len) {
  int res = 0;
  if (is_len != should_len)
    res = is_len > should_len ? -1 : 1;
  else
    res = memcmp(is, should, is_len);

  if (res != 0) {
    _print_hex("SHOULD", should, should_len);
    _print_hex("IS    ", is, is_len);
  }

  return res;
}
