#define SSE4_1_FLAG 0x080000
#define SSE4_2_FLAG 0x100000

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx) {
  /* ecx is often an input as well as an output. */
  asm volatile("cpuid"
               : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
               : "0"(*eax), "2"(*ecx));
}

void test_cpu_feature() {
  unsigned eax, ebx, ecx, edx;
  eax = 1; /* processor info and feature bits */

  const int CHECKBITS = SSE4_1_FLAG | SSE4_2_FLAG;
  eax = ebx = ecx = edx = 0;
  eax = 1;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  dprintf(1, "mmx available: 0x%0x\n", (edx >> 23) & 0x1);
  dprintf(1, "sse available: 0x%0x\n", (edx >> 25) & 0x1);
  dprintf(1, "sse4 available: 0x%0x\n", (ecx)&CHECKBITS == CHECKBITS);

  native_cpuid(&eax, &ebx, &ecx, &edx);
  dprintf(1, "eax: %x ebx: %x ecx: %x edx: %x\n", eax, ebx, ecx, edx);

  dprintf(1, "stepping %d\n", eax & 0xF);                 // Bit 3-0
  dprintf(1, "model %d\n", (eax >> 4) & 0xF);             // Bit 7-4
  dprintf(1, "family %d\n", (eax >> 8) & 0xF);            // Bit 11-8
  dprintf(1, "processor type %d\n", (eax >> 12) & 0x3);   // Bit 13-12
  dprintf(1, "extended model %d\n", (eax >> 16) & 0xF);   // Bit 19-16
  dprintf(1, "extended family %d\n", (eax >> 20) & 0xFF); // Bit 27-20

  // if smx set - SGX global enable is supported
  dprintf(1, "smx: %d\n", (ecx >> 6) & 1); // CPUID.1:ECX.[bit6]

  eax = ebx = ecx = edx = 0;
  eax = 1;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  dprintf(1, "sse4 available: 0x%0x\n", (ecx));

  /* Extended feature bits (EAX=07H, ECX=0H)*/
  dprintf(1, "\nExtended feature bits (EAX=07H, ECX=0H)\n");
  eax = 7;
  ecx = 0;

  dprintf(1, "eax: %x ebx: %x ecx: %x edx: %x\n", eax, ebx, ecx, edx);

  // CPUID.(EAX=07H, ECX=0H):EBX.SGX = 1,
  dprintf(1, "sgx available: %d\n", (ebx >> 2) & 0x1);

  /* CPUID Leaf 12H, Sub-Leaf 0 Enumeration of Intel SGX Capabilities
   * (EAX=12H,ECX=0) */
  dprintf(1, "\nCPUID Leaf 12H, Sub-Leaf 0 of Intel SGX Capabilities "
             "(EAX=12H,ECX=0)\n");
  eax = 0x12;
  ecx = 0;
  native_cpuid(&eax, &ebx, &ecx, &edx);
  dprintf(1, "eax: %x ebx: %x ecx: %x edx: %x\n", eax, ebx, ecx, edx);

  dprintf(1, "sgx 1 supported: %d\n", eax & 0x1);
  dprintf(1, "sgx 2 supported: %d\n", (eax >> 1) & 0x1);
  dprintf(1, "MaxEnclaveSize_Not64: %x\n", edx & 0xFF);
  dprintf(1, "MaxEnclaveSize_64: %x\n", (edx >> 8) & 0xFF);
}
