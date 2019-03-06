#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver

	int r = 0, perm;
	uint32_t req;

	while (1)
	{
		envid_t envid;
		req = ipc_recv(&envid, &nsipcbuf, &perm);

		if (req != NSREQ_OUTPUT || envid != ns_envid)
				continue;

		r = sys_e1000_transmit(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);
		if (r < 0) panic("Fallo en e1000 transmit\n");

	}
}
