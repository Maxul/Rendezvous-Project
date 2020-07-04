#include "output.h" // dprintf
#include "string.h" // memcpy
#include "x86.h" // inb

#include "smx.h"
#include "sodium.h"
#include "microbench.h"

extern void *shmem_base;

unsigned long overhead;

#pragma GCC push_options
#pragma GCC optimize("O0")
#pragma GCC optimize("no-omit-frame-pointer")
static void microbench_compare_malloc_memcpy()
{
    unsigned long ticks, cycles;
	int i;

	synch_tsc();
	ticks = rdtscllp();

for (i = 0; i < N; i++) {
    unsigned char * p = (unsigned char *)sodium_malloc(PKT_SIZE);
    sodium_free(p);
}
    cycles = (rdtscllp() - ticks) / N;
	dprintf(1, "malloc took %lu cycles\n", cycles);
	dprintf(1, "malloc took %lu us\n", cycles / cpu_freq());

	ticks = rdtscllp();

    char a[PKT_SIZE], b[PKT_SIZE];
for (i = 0; i < N; i++) {
    memcpy(b, a, PKT_SIZE);
}
    cycles = (rdtscllp() - ticks) / N;
	dprintf(1, "memcpy took %lu cycles\n", cycles);
	dprintf(1, "memcpy took %lu us\n", cycles / cpu_freq());

}
#pragma GCC pop_options

int aesni_test()
{
    unsigned long ticks;
	overhead = measure_tsc_overhead();
	dprintf(1, "TSC overhead is %ld\n", overhead);

    unsigned char      *ad = "";
    unsigned char      *ciphertext;
    unsigned char      *decrypted;
    unsigned char      *detached_ciphertext;
    unsigned char      *expected_ciphertext;
    unsigned char      *key;
    unsigned char      *message;
    unsigned char      *mac;
    unsigned char      *nonce;
    char               *hex;
    unsigned long long  found_ciphertext_len;
    unsigned long long  found_mac_len;
    unsigned long long  found_message_len;
    size_t              ad_len = strlen(ad);
    size_t              ciphertext_len;
    size_t              detached_ciphertext_len;
    size_t              i = 0U;
    size_t              message_len = 1<<10;

    ciphertext_len = message_len + crypto_aead_aes256gcm_ABYTES;

    key = (unsigned char *) sodium_malloc(crypto_aead_aes256gcm_KEYBYTES);
    nonce = (unsigned char *) sodium_malloc(crypto_aead_aes256gcm_NPUBBYTES);
    mac = (unsigned char *) sodium_malloc(crypto_aead_aes256gcm_ABYTES);

    message = (unsigned char *) sodium_malloc(message_len);
    ad = (unsigned char *) sodium_malloc(ad_len);
    ciphertext = (unsigned char *) sodium_malloc(ciphertext_len);
    decrypted = (unsigned char *) sodium_malloc(message_len);

    memset(key, 0xf2, crypto_aead_aes256gcm_KEYBYTES);
    memset(message, 'X', message_len);

	synch_tsc();
	ticks = rdtscllp();

for (i = 0; i < N; i++) {

    crypto_aead_aes256gcm_encrypt(ciphertext, &found_ciphertext_len,
                                      message, message_len,
                                      ad, ad_len, NULL, nonce, key);

    found_message_len = 1;
    if (crypto_aead_aes256gcm_decrypt(decrypted, &found_message_len,
                                          NULL, ciphertext, ciphertext_len,
                                          ad, ad_len, nonce, key) != 0) {
        dprintf(1, "Verification of test vector #%u failed\n", (unsigned int) i);
    }

}
    u32 cycles = (rdtscllp() - ticks - overhead) / N;
	dprintf(1, "AESNI took %lu cycles\n", cycles);
	dprintf(1, "AESNI took %lu us\n", cycles / cpu_freq());

    compare_testvector(decrypted, message_len, message, message_len);

    sodium_free(message);
    sodium_free(ad);
    sodium_free(ciphertext);
    sodium_free(decrypted);

    sodium_free(key);
    sodium_free(mac);
    sodium_free(nonce);

    return 0;
}

void microbench_time_service()
{
    unsigned char      *ad = "";
    unsigned char      *ciphertext;
    unsigned char      *decrypted;
    unsigned char      *detached_ciphertext;
    unsigned char      *expected_ciphertext;
    unsigned char      *key;
    unsigned char      *nonce;
    unsigned char      *message;
    char               *hex;
    unsigned long long  found_ciphertext_len;
    unsigned long long  found_mac_len;
    unsigned long long  found_message_len;
    size_t              ad_len = strlen(ad);
    size_t              ciphertext_len;
    size_t              detached_ciphertext_len;
    size_t              i = 0U;
    size_t              message_len = 1<<12;

    ciphertext_len = message_len + crypto_aead_aes256gcm_ABYTES;

    message = (unsigned char *) sodium_malloc(message_len);
    key = (unsigned char *) sodium_malloc(crypto_aead_aes256gcm_KEYBYTES);
    nonce = (unsigned char *) sodium_malloc(crypto_aead_aes256gcm_NPUBBYTES);

    ciphertext = (unsigned char *) sodium_malloc(ciphertext_len);
    decrypted = (unsigned char *) sodium_malloc(message_len);

    memset(key, 0xca, crypto_aead_aes256gcm_KEYBYTES);
    memset(nonce, 0xff, crypto_aead_aes256gcm_NPUBBYTES);

checkpoint("response to SGX and start to copy");

    memcpy(ciphertext, shmem_base + 0x64, ciphertext_len);

checkpoint("copy to SMRAM done");

    found_message_len = 1;
    if (crypto_aead_aes256gcm_decrypt(decrypted, &found_message_len,
                                          NULL, ciphertext, ciphertext_len,
                                          ad, ad_len, nonce, key) != 0) {
        dprintf(1, "Verification of test vector #%u failed\n", (unsigned int) i);
    }
//    _print_hex("msg from SGX", decrypted, message_len);

checkpoint("decrypt done");

    time_service();

//    memset(ciphertext, 0, ciphertext_len);
    memset(message, 'B', message_len);
checkpoint("time service done");

    crypto_aead_aes256gcm_encrypt(ciphertext, &found_ciphertext_len,
                                      message, message_len,
                                      ad, ad_len, NULL, nonce, key);

checkpoint("SMRAM encrypt done");

    memcpy(shmem_base + 0x64, ciphertext, ciphertext_len);

checkpoint("copy to channel done");
//    _print_hex("cipher from SGX", ciphertext, ciphertext_len);

    sodium_free(message);
    sodium_free(ad);
    sodium_free(ciphertext);
    sodium_free(decrypted);

    sodium_free(key);
    sodium_free(nonce);

    return 0;
}

void time_service(void);

void loopback()
{
    char msg[MSG_SIZE];
    int msg_len = 0;

    memset(msg, 0x0, MSG_SIZE);
    while (0 == get_out_buffer(msg, &msg_len)) {
//            _print_hex("msg from SGX", msg, msg_len);
checkpoint("time service begin");
    time_service();
checkpoint("time service done");
        put_in_buffer(msg, msg_len);
    }
}

/**
    Since the native memset provided by SeaBIOS is so inefficient in 32-bit SMM
    therefore we need to replace it for better performance
*/
#define memset __builtin_memset
#undef memset

#if 0
static inline
void *__constant_c_and_count_memset(void *s, unsigned long pattern,
				    size_t count)
{
	switch (count) {
	case 0:
		return s;
	case 1:
		*(unsigned char *)s = pattern & 0xff;
		return s;
	case 2:
		*(unsigned short *)s = pattern & 0xffff;
		return s;
	case 3:
		*(unsigned short *)s = pattern & 0xffff;
		*((unsigned char *)s + 2) = pattern & 0xff;
		return s;
	case 4:
		*(unsigned long *)s = pattern;
		return s;
	}

#define COMMON(x)							\
	asm volatile("rep ; stosl"					\
		     x							\
		     : "=&c" (d0), "=&D" (d1)				\
		     : "a" (eax), "0" (count/4), "1" ((long)s)	\
		     : "memory")

	{
		int d0, d1;
#if __GNUC__ == 4 && __GNUC_MINOR__ == 0
		/* Workaround for broken gcc 4.0 */
		register unsigned long eax asm("%eax") = pattern;
#else
		unsigned long eax = pattern;
#endif

		switch (count % 4) {
		case 0:
			COMMON("");
			return s;
		case 1:
			COMMON("\n\tstosb");
			return s;
		case 2:
			COMMON("\n\tstosw");
			return s;
		default:
			COMMON("\n\tstosw\n\tstosb");
			return s;
		}
	}

#undef COMMON
}

static inline
void *__constant_c_memset(void *s, unsigned long c, size_t count)
{
	int d0, d1;
	asm volatile("rep ; stosl\n\t"
		     "testb $2,%b3\n\t"
		     "je 1f\n\t"
		     "stosw\n"
		     "1:\ttestb $1,%b3\n\t"
		     "je 2f\n\t"
		     "stosb\n"
		     "2:"
		     : "=&c" (d0), "=&D" (d1)
		     : "a" (c), "q" (count), "0" (count/4), "1" ((long)s)
		     : "memory");
	return s;
}

static inline void *__memset_generic(void *s, char c, size_t count)
{
	int d0, d1;
	asm volatile("rep\n\t"
		     "stosb"
		     : "=&c" (d0), "=&D" (d1)
		     : "a" (c), "1" (s), "0" (count)
		     : "memory");
	return s;
}

static inline unsigned long read_tsc(void)
{
	unsigned int a, d;
	asm volatile("rdtscp" : "=a" (a), "=d" (d) : : "%rbx", "%rcx");
	return (unsigned long) a;
}

#define NUM	10000
#define PAGE_SIZE (1<<14)

void mem_bench()
{
	int i;
	unsigned long ticks;

    asm volatile("cpuid" : : : "%rax", "%rbx", "%rcx", "%rdx");

int sz = 1<<10;
void *ram = 0x00;
for (sz = (1<<10); sz <= (1<<16); sz <<= 1) {
	dprintf(1, "\nmemseting %u KB data on DDR4\n", sz>>10);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    memset(ram, 0xcc, sz);
	}
	dprintf(1, "memset() took %lu cycles\n", (read_tsc() - ticks) / NUM);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    __builtin_memset(ram, 0xcc, sz);
	}
	dprintf(1, "__builtin_memset() took %lu cycles\n", (read_tsc() - ticks) / NUM);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    __memset_generic(ram, 0xcc, sz);
	}
	dprintf(1, "__memset_generic() took %lu cycles\n", (read_tsc() - ticks) / NUM);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    __constant_c_memset(ram, 0xcc, sz);
	}
	dprintf(1, "__constant_c_memset() took %lu cycles\n", (read_tsc() - ticks) / NUM);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    __constant_c_and_count_memset(ram, 0xcc, sz);
	}
	dprintf(1, "__constant_c_and_count_memset() took %lu cycles\n", (read_tsc() - ticks) / NUM);
}

static char mem[PAGE_SIZE], data[PAGE_SIZE];

for (sz = (1<<10); sz <= PAGE_SIZE; sz <<= 1) {
	dprintf(1, "\ncopying %u KB data on DDR4\n", sz>>10);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    memcpy(mem, data, sz);
	}
	dprintf(1, "memcpy() took %lu cycles\n", (read_tsc() - ticks) / NUM);

	ticks = read_tsc();
	for (i = 0; i < NUM; i++) {
	    memmove(mem, data, sz);
	}
	dprintf(1, "memmove() took %lu cycles\n", (read_tsc() - ticks) / NUM);
}


}
#endif
