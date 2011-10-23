#include "types.h"
#include "mmu.h"
#include "kernel.h"
#include "x86.h"

struct segdesc  __attribute__((aligned(16))) bootgdt[NSEGS] = {
  // null
  SEGDESC(0, 0, 0),
  // 32-bit kernel code
  SEGDESC(0, 0xfffff, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G),
  // 64-bit kernel code
  SEGDESC(0, 0, SEG_R|SEG_CODE|SEG_S|SEG_DPL(0)|SEG_P|SEG_L|SEG_G),
  // kernel data
  SEGDESC(0, 0xfffff, SEG_W|SEG_S|SEG_DPL(0)|SEG_P|SEG_D|SEG_G)
};

struct intdesc idt[256] __attribute__((aligned(16)));

// boot.S
extern u64 trapentry[];

void
trap(void)
{
  panic("trap");
}

void
inittrap(void)
{
  volatile struct desctr dtr;
  uint64 entry;
  uint32 bits;
  int i;
  
  bits = INT_P | SEG_INTR64;  // present, interrupt gate
  for(i=0; i<256; i++) {
    entry = trapentry[i];
    idt[i] = INTDESC(KCSEG, entry, bits);
  }

  dtr.limit = sizeof(idt) - 1;
  dtr.base = (u64)idt;
  lidt((void *)&dtr.limit);

  // Reload GDT from kernel VA
  dtr.limit = sizeof(bootgdt) - 1;
  dtr.base = (u64)bootgdt;
  lgdt((void *)&dtr.limit);
}
