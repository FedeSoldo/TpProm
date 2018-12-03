// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW 0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	pte_t actual_PTE = uvpt[PGNUM(addr)];
	if (!(err & FEC_WR) || !(actual_PTE & PTE_COW)) panic("No es COW");

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	addr = ROUNDDOWN(addr, PGSIZE);

	if (sys_page_alloc(0, PFTEMP, PTE_W|PTE_U|PTE_P) < 0) panic("sys_page_alloc");
	memcpy(PFTEMP, addr, PGSIZE);

	if (sys_page_map(0, PFTEMP, 0, addr, PTE_W|PTE_U|PTE_P) < 0) panic("sys_page_map");

	if (sys_page_unmap(0, PFTEMP) < 0) panic("sys_page_unmap");

	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	//Calculo la direccion a partir del numero de pagina.
	void *addr = (void*) (pn*PGSIZE);

	if (uvpt[pn] & PTE_SHARE) sys_page_map(0, addr, envid, addr, uvpt[pn]&PTE_SYSCALL); //Copiamos directo porque tiene activado el share.
	else if ((uvpt[pn] & PTE_W) || (uvpt[pn] & PTE_COW))
		{
			if (sys_page_map(0, addr, envid, addr, PTE_COW|PTE_U|PTE_P) < 0)
				panic("Error 1");
			if (sys_page_map(0, addr, 0, addr, PTE_COW|PTE_U|PTE_P) < 0)
				panic("Error 2");
		}
		else sys_page_map(0, addr, envid, addr, PTE_U|PTE_P);

	//panic("duppage not implemented");
	return 0;
}

static void
dup_or_share(envid_t dstenv, void *addr, int perm)
{
	int r;

	bool escritura = perm == ( perm | PTE_W);
	if (!escritura) sys_page_map(0, addr, dstenv, addr, perm); //SE DUPLICA
	else {
		// This is NOT what you should do in your fork.
		if ((r = sys_page_alloc(dstenv, addr, PTE_P|PTE_U|PTE_W)) < 0)
			panic("sys_page_alloc: %e", r);
		if ((r = sys_page_map(dstenv, addr, 0, UTEMP, PTE_P|PTE_U|PTE_W)) < 0)
			panic("sys_page_map: %e", r);
		memmove(UTEMP, addr, PGSIZE);
		if ((r = sys_page_unmap(0, UTEMP)) < 0)
			panic("sys_page_unmap: %e", r);
	}
}


envid_t
fork_v0(void)
{
	uint8_t *addr;
	int r;

	envid_t id = sys_exofork();

	if (id < 0) panic("sys_exofork failed");

	if (id == 0)
	{	//SOY EL HIJO
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	//Hasta aca igual a dumbfork

	for (addr = 0; addr < (uint8_t*)UTOP; addr += PGSIZE)
	{
			pde_t actual_PDE = uvpd[PDX(addr)];
			if ((actual_PDE | PTE_P) != actual_PDE) continue;
			pte_t actual_PTE = uvpt[PGNUM(addr)];
			bool mapeada = (actual_PTE | PTE_P) == actual_PTE;

			if (mapeada) dup_or_share(id, addr, actual_PTE & PTE_SYSCALL);
	}

	//Aca volvemos a copiar a dumbfork
	// Start the child environment running
	if ((r = sys_env_set_status(id, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return id;

}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	//return fork_v0();

	//Instalo el manejador de pagefaults
	set_pgfault_handler(pgfault);

	uint8_t *addr;
	int r;
	//Hacemos como siempre el llamado a sys_exofork
	envid_t id = sys_exofork();

	if (id < 0) panic("sys_exofork failed");

	if (id == 0)
	{	//SOY EL HIJO
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	//Reservar memoria para la pila de excepciones del hijo, e instalar su manejador de excepciones.
	//ESto hay que hacerlo a mano como dijo Dato.
	if (sys_page_alloc(id, (void *)(UXSTACKTOP-PGSIZE), PTE_U|PTE_W|PTE_P) < 0)
	panic("Fallo el sys_page_alloc");
	extern void _pgfault_upcall();
	sys_env_set_pgfault_upcall(id, _pgfault_upcall);

	for (addr = 0; addr <(uint8_t*)UTOP; addr += PGSIZE)
	{
		pde_t actual_PDE = uvpd[PDX(addr)];
		if ((actual_PDE | PTE_P) != actual_PDE) continue;
		pte_t actual_PTE = uvpt[PGNUM(addr)];
		if ((actual_PTE & PTE_P)
			&& (actual_PTE & PTE_U))
		{
			if (PGNUM(addr) >= PGNUM(UXSTACKTOP - PGSIZE) && (PGNUM(addr) < PGNUM(UXSTACKTOP))) continue;
			duppage(id, PGNUM(addr));
		}
	}

	//Seteamos al hijo en ENV_RUNNABLE
	if ((r = sys_env_set_status(id, ENV_RUNNABLE)) < 0)
		panic("sys_env_set_status: %e", r);

	return id;
}



// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
