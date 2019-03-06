#include "ns.h"

extern union Nsipc nsipcbuf;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.

	int r;
	size_t size = sizeof(union Nsipc);

	int input_wait = 50;

  r = sys_page_alloc(0, &nsipcbuf, PTE_W|PTE_U|PTE_P);
	if (r < 0) cprintf("Fallo en sys_page_alloc\n");

	while (1)
	{
		//Leo paquete, si esta la cola vacia hago yield
  	if ((r = sys_e1000_receive(nsipcbuf.pkt.jp_data, size)) < 0)
		{
			sys_yield();
			continue;			//Salto a la proxima iteracion cuando vuelvo, r sigue en -1
		}

		nsipcbuf.pkt.jp_len = r;
    ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_U|PTE_P|PTE_W);


		//Parte del hint
		for (int i = 0; i < input_wait; i++)
        sys_yield();
	}
}
