#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>
#include <kern/pmap.h>

#define E1000_STATUS 0x00008

//Transmit
#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */
#define E1000_TCTL     0x00400  /* TX Control - RW */
#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */

#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */

#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */
#define E1000_TXD_CMD_RS     0x08000000 >> 24 /* Report Status */
#define E1000_TXD_CMD_EOP    0x01000000 >> 24/* End of Packet */


#define CANT_DESCS 32


int e1000_attach(struct pci_func *pcif);    //Funcion para pci.c

static uint8_t* e1000_vaddr_map; //Variable con direccion del mapeo

static uint32_t getreg(uint32_t offset);

static void setreg(uint32_t offset, uint32_t value);

void e1000_transmit_init();

// Transmit Descriptor
struct tx_desc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__ ((packed));

#endif  // JOS_KERN_E1000_H
