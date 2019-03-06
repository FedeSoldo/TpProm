#include <kern/e1000.h>
#include <inc/string.h>
#include <inc/error.h>

// LAB 6: Your driver code here

static struct tx_desc tdescs[CANT_DESCS];
static uint8_t tpackets[CANT_DESCS][MAX_PQT];

int e1000_attach(struct pci_func *pcif)
{
  pci_func_enable(pcif);
  e1000_vaddr_map = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
  //cprintf("\nTengo: 0x%x\nEpero: 0x80080783\n\n", getreg(E1000_STATUS));

  e1000_transmit_init();

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

void e1000_transmit_init()
{
  setreg(E1000_TDBAL, PADDR(tdescs));
  setreg(E1000_TDBAH, 0);
  setreg(E1000_TDLEN, sizeof(tdescs));
  setreg(E1000_TDH, 0);
  setreg(E1000_TDT, 0);

  uint32_t tctl = (E1000_TCTL_EN | E1000_TCTL_PSP | 0x00000010 | 0x00000040);   // Ultimos 2 valores son CT Y COLD
  setreg(E1000_TCTL, tctl);

  uint32_t tipg = (10 | 4 | 6);  //  (IPGT | IPGR1 | IPGR2) valores en el manual 13.4.34
  setreg(E1000_TIPG, tipg);

  for (int i = 0; i < CANT_DESCS; i++)
  {
      tdescs[i].addr = PADDR(&tpackets[i]);
      tdescs[i].status |= E1000_TXD_STAT_DD;
  }
}

int e1000_transmit(void* addr, uint16_t len)
{
    int tail = getreg(E1000_TDT);

    if (len > MAX_PQT) return -E_INVAL;

    if (!(tdescs[tail].status & E1000_TXD_STAT_DD))   //Chequeo si esta libre el proximo descriptor
    {
        cprintf("Transmit descriptors lleno\n");
        return -1;
    }

    //Copio el paquete
    memmove(KADDR(tdescs[tail].addr), addr, len);
    tdescs[tail].length = len;
    tdescs[tail].status &= ~E1000_TXD_STAT_DD;
    uint32_t cmdFlags = E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS;
    tdescs[tail].cmd = cmdFlags;
    setreg(E1000_TDT, (tail + 1) % CANT_DESCS);   //Update de TDT
    return 0;
}
