#include <kern/e1000.h>
#include <inc/string.h>

// LAB 6: Your driver code here

int e1000_attach(struct pci_func *pcif)
{
  pci_func_enable(pcif);
  e1000_vaddr_map = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
  cprintf("\nTengo: 0x%x\nEpero: 0x80080783\n\n", getreg(E1000_STATUS));

  return 0;
}

static uint32_t
getreg(uint32_t offset)
{
  return *(volatile uint32_t *) (e1000_vaddr_map + offset);
}

static void
setreg(uint32_t offset, uint32_t value)
{
  *(volatile uint32_t *) (e1000_vaddr_map + offset) = value;
}
