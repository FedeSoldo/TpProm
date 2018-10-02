TP1: Memoria virtual en JOS
===========================

page2pa
-------------

Recibe un puntero a un struct PageInfo *pp y devuelve la direccion de memoria fisica donde comienza la pagina. A este puntero le resta la direccion donde se encuentra el primer struct PageInfo del arreglo de paginas para asi quitarle el offset y obtener el virtual page number, y le hace un shift PGSHIFT = 12, debido a que las physical page son de 4kb para alinearlo.



boot_alloc_pos
--------------

a.

b. make gdb
gdb -q -s obj/kern/kernel -ex 'target remote 127.0.0.1:26000' -n -x .gdbinit
Reading symbols from obj/kern/kernel...done.
Remote debugging using 127.0.0.1:26000
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000fff0 in ?? ()
(gdb) b boot_alloc
Breakpoint 1 at 0xf0100a59: file kern/pmap.c, line 89.
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0xf0100a59 <boot_alloc>:	mov    %eax,%edx

Breakpoint 1, boot_alloc (n=981) at kern/pmap.c:89
89	{
(gdb) si
=> 0xf0100a5b <boot_alloc+2>:	cmpl   $0x0,0xf0117538
98		if (!nextfree)
(gdb) p nextfree 
$1 = 0x0
(gdb) p (char*) end
$2 = 0xf0100702 <cons_init> "U\211\345\203\354\b\350\266\373\377\377\350\277\372\377\377\200=4u", <incomplete sequence \360>
(gdb) si
=> 0xf0100a62 <boot_alloc+9>:	je     0xf0100a9f <boot_alloc+70>
0xf0100a62	98		if (!nextfree)
(gdb) si
=> 0xf0100a9f <boot_alloc+70>:	mov    $0xf011894f,%eax
101			nextfree = ROUNDUP((char *) end, PGSIZE);
(gdb) 
=> 0xf0100aa4 <boot_alloc+75>:	and    $0xfffff000,%eax
0xf0100aa4	101			nextfree = ROUNDUP((char *) end, PGSIZE);
(gdb) 
=> 0xf0100aa9 <boot_alloc+80>:	mov    %eax,0xf0117538
0xf0100aa9	101			nextfree = ROUNDUP((char *) end, PGSIZE);
(gdb) 
=> 0xf0100aae <boot_alloc+85>:	jmp    0xf0100a64 <boot_alloc+11>
0xf0100aae	101			nextfree = ROUNDUP((char *) end, PGSIZE);
(gdb) 
=> 0xf0100a64 <boot_alloc+11>:	test   %edx,%edx
110		if (n != 0)
(gdb) p nextfree 
$3 = 0xf0118000 ""
(gdb) p (char*) end
$4 = 0xf0100702 <cons_init> "U\211\345\203\354\b\350\266\373\377\377\350\277\372\377\377\200=4u", <incomplete sequence \360>
(gdb) si
=> 0xf0100a66 <boot_alloc+13>:	je     0xf0100ab0 <boot_alloc+87>
0xf0100a66	110		if (n != 0)
(gdb) 
=> 0xf0100a68 <boot_alloc+15>:	mov    0xf0117538,%eax
112			result = nextfree;
(gdb) 
=> 0xf0100a6d <boot_alloc+20>:	lea    0xfff(%eax,%edx,1),%edx
113			nextfree = ROUNDUP((char *) (nextfree+n), PGSIZE);
(gdb) 
=> 0xf0100a74 <boot_alloc+27>:	and    $0xfffff000,%edx
0xf0100a74	113			nextfree = ROUNDUP((char *) (nextfree+n), PGSIZE);
(gdb) 
=> 0xf0100a7a <boot_alloc+33>:	mov    %edx,0xf0117538
0xf0100a7a	113			nextfree = ROUNDUP((char *) (nextfree+n), PGSIZE);
(gdb) 
=> 0xf0100a80 <boot_alloc+39>:	cmp    $0x400000,%edx
114			if ((int)nextfree > PGSIZE*1024) panic("boot_alloc: out of memory\n");
(gdb) p nextfree 
$5 = 0xf0119000 ""
(gdb) p (char*) end
$6 = 0xf0100702 <cons_init> "U\211\345\203\354\b\350\266\373\377\377\350\277\372\377\377\200=4u", <incomplete sequence \360>



page_alloc
----------

Como se explico anteriormente, page2pa recibe un puntero a un struct PageInfo *pp y devuelve la direccion de memoria fisica donde comienza la pagina. Por otro lado, page2kva, utiliza la macro KADDR que toma una direccion fisica y devuelve la kernel virtual address correspondiente. Entonces, page2kva recibe tambien un struct PageInfo *pp que convierte a direccion fisica usando page2pa, para luego usar la macro KADDR y asi devolver una kernel virtual address.
