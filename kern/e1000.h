#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>
#include <kern/pmap.h>

#define E1000_STATUS 0x00008

int e1000_attach(struct pci_func *pcif);    //Funcion para pci.c

static uint8_t* e1000_vaddr_map; //Variable con direccion del mapeo

static uint32_t getreg(uint32_t offset);

static void setreg(uint32_t offset, uint32_t value);

#endif  // JOS_KERN_E1000_H
