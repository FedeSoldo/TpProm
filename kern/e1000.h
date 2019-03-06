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


#define CANT_TDESCS 32
#define MAX_TPQT 1518


//Receive
#define E1000_RCTL     0x00100  /* RX Control - RW */
#define E1000_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808  /* RX Descriptor Length - RW */
#define E1000_RDH      0x02810  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818  /* RX Descriptor Tail - RW */

#define E1000_RAL      0x05400  /* Receive Address - RW Array Low*/
#define E1000_RAH      0x05404  /* Receive Address - RW Array High*/
#define E1000_RAH_AV  (0x1 << 31)        /* Receive descriptor valid */

#define E1000_RCTL_EN			0x00000002    /* enable */
#define E1000_RCTL_LPE			0x00000020    /* long packet enable */
#define E1000_RCTL_LBM_NO		0x00000000    /* no loopback mode */
#define E1000_RCTL_RDMTS_HALF	0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_MO_0			0x00000000    /* multicast offset 11:0 */
#define E1000_RCTL_BAM			0x00008000    /* broadcast enable */
#define E1000_RCTL_SECRC		0x04000000    /* Strip Ethernet CRC */
#define E1000_RCTL_BSEX			0x02000000    /* Buffer size extension */
#define E1000_RCTL_SZ_2048      0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SZ_4096      0x00030000    /* rx buffer size 4096 */

#define E1000_RXD_STAT_DD       0x01    /* Descriptor Done */


#define CANT_RDESCS 128
#define MAX_RPQT 2048


int e1000_attach(struct pci_func *pcif);    //Funcion para pci.c

static uint8_t* e1000_vaddr_map; //Variable con direccion del mapeo

static uint32_t getreg(uint32_t offset);

static void setreg(uint32_t offset, uint32_t value);

void e1000_transmit_init();

int e1000_transmit(void* addr, uint16_t len);

void e1000_receive_init();

int e1000_receive(void* addr, size_t size);

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

struct rx_desc
{
    uint64_t addr;
    uint16_t length;
    uint16_t cso;
    uint8_t status;
    uint8_t errs;
    uint16_t special;
} __attribute__ ((packed));

#endif  // JOS_KERN_E1000_H
