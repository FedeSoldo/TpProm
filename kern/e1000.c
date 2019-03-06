#include <kern/e1000.h>
#include <inc/string.h>
#include <inc/error.h>

// LAB 6: Your driver code here

static struct tx_desc tdescs[CANT_TDESCS];
static uint8_t tpackets[CANT_TDESCS][MAX_TPQT];

static struct rx_desc rdescs[CANT_RDESCS];
static uint8_t rpackets[CANT_RDESCS][MAX_RPQT];

int e1000_attach(struct pci_func *pcif)
{
  pci_func_enable(pcif);
  e1000_vaddr_map = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
  //cprintf("\nTengo: 0x%x\nEpero: 0x80080783\n\n", getreg(E1000_STATUS));

  e1000_transmit_init();

  e1000_receive_init();

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

  for (int i = 0; i < CANT_TDESCS; i++)
  {
      tdescs[i].addr = PADDR(&tpackets[i]);
      tdescs[i].status |= E1000_TXD_STAT_DD;
  }
}

int e1000_transmit(void* addr, uint16_t len)
{
    int tail = getreg(E1000_TDT);

    if (len > MAX_TPQT) return -E_INVAL;

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
    setreg(E1000_TDT, (tail + 1) % CANT_TDESCS);   //Update de TDT
    return 0;
}

void e1000_receive_init()
{
  setreg(E1000_RAL, 0x12005452);            //Lower bytes de la mac de qemu

  uint32_t high = 0x00005634 | E1000_RAH_AV; //Higher bytes de la mac de qemu
  setreg(E1000_RAH, high);

  setreg(E1000_RDBAL, PADDR(rdescs));
  setreg(E1000_RDBAH, 0);
  setreg(E1000_RDLEN, sizeof(rdescs));
  setreg(E1000_RDH, 0);
  setreg(E1000_RDT, CANT_RDESCS - 1);

  uint32_t rctl = E1000_RCTL_EN | E1000_RCTL_SZ_2048 | E1000_RCTL_SECRC;
  setreg(E1000_RCTL, rctl);

  for (int i = 0; i < CANT_RDESCS; i++)
  {
     rdescs[i].addr = PADDR(&rpackets[i]);
  }
}

int e1000_receive(void* addr, size_t size)
{
  uint32_t tail = getreg(E1000_RDT);
  uint32_t next = (tail + 1) % CANT_RDESCS;

  if (!(rdescs[next].status & E1000_RXD_STAT_DD)) //Cola vacia
    return -1;

  int lenght = rdescs[next].length;

  if (size < lenght) //Paquete muy largo
    return -1;

  memmove(addr, KADDR(rdescs[next].addr), lenght);
  rdescs[next].status &= ~E1000_RXD_STAT_DD;

  setreg(E1000_RDT, next);
  return lenght;
}
