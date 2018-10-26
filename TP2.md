TP2: Procesos de usuario
========================

env_alloc
---------

1. Asumiendo que los env_id son 0, y sabiendo que el tamaño del struct Env es 96 (60 en hexa)
pudimos determinar que los id son: 1000, 1060, 10c0, 1120 y 1180.

2. Para el caso de que se mate el proceso 630, y asumiendo que estan todos ocupados, el env_free
apuntara aqui, su id era fc40. Con esta informacion los ids del proceso que se mata y vuelve a
ejecutar para sus primeras 5 corridas son: 1ec40, 2ec40, 3ec40, 4ec40 y 5ec40.

env_init_percpu
---------------

Escribe 6 bytes en el GDTR (global descriptor table register). Estos bytes
representan una direccion de memoria base y un limite (tamaño de la tabla en bytes).

env_pop_tf
----------

1. Tras el primer movl de la funcion, se setea el stack pointer para que apunte al comienzo de Trapframe

2. Justo antes de la instrucción iret, en %esp se encuentra el Instruction Pointer que se cargará en el registro %eip y es la dirección de memoria a donde saltará la ejecución. En 8(%esp) se encuentra el Trap Number (trapno).

3. Para determinar un cambio de ring, la CPU siempre lleva control de los segment selectors. En particular, el code segment register (cs), no puede ser seteados por acciones como mov. Lo importante es que cs mantiene un campo llamado current privilege level (CPL). Este registro es siempre igual al privilegio actual de la CPU, por lo tanto, esta es la forma de la CPU de estar al tanto del nivel de privilegio actual.

gdb_hello
---------

1. make gdb
gdb -q -s obj/kern/kernel -ex 'target remote 127.0.0.1:26000' -n -x .gdbinit
Reading symbols from obj/kern/kernel...done.
Remote debugging using 127.0.0.1:26000
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000fff0 in ?? ()
(gdb) b env_pop_tf
Breakpoint 1 at 0xf0102e0c: file kern/env.c, line 467.
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0xf0102e0c <env_pop_tf>:	push   %ebp

Breakpoint 1, env_pop_tf (tf=0xf01bf000) at kern/env.c:467
467	{


2. make run-hello-nox-gdb
make[1]: Entering directory '/home/eche/Documents/JOS'
+ cc kern/init.c
+ ld obj/kern/kernel
+ mk obj/kern/kernel.img
make[1]: Leaving directory '/home/eche/Documents/JOS'
qemu-system-i386 -nographic -drive file=obj/kern/kernel.img,index=0,media=disk,format=raw -serial mon:stdio -gdb tcp:127.0.0.1:26000 -D qemu.log  -d guest_errors -S
main-loop: WARNING: I/O thread spun for 1000 iterations
6828 decimal is 15254 octal!
Physical memory: 131072K available, base = 640K, extended = 130432K
check_page_alloc() succeeded!
check_page() succeeded!
check_kern_pgdir() succeeded!
check_page_installed_pgdir() succeeded!
[00000000] new env 00001000
QEMU 2.8.1 monitor - type 'help' for more information
(qemu) info registers
EAX=003bc000 EBX=f01bf000 ECX=f03bc000 EDX=00000200
ESI=00010094 EDI=00000000 EBP=f0118fd8 ESP=f0118fbc
EIP=f0102e0c EFL=00000092 [--S-A--] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0010 00000000 ffffffff 00cf9300 DPL=0 DS   [-WA]
CS =0008 00000000 ffffffff 00cf9a00 DPL=0 CS32 [-R-]


3. (gdb) p tf
$1 = (struct Trapframe *) 0xf01bf000*


4. El calculo de N nos dio 17.
(gdb) print sizeof(struct Trapframe) / sizeof(int)
$2 = 17
(gdb) x/17x tf
0xf01bf000:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01bf010:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01bf020:	0x00000023	0x00000023	0x00000000	0x00000000
0xf01bf030:	0x00800020	0x0000001b	0x00000000	0xeebfe000
0xf01bf040:	0x00000023


5. (gdb) disas
Dump of assembler code for function env_pop_tf:
=> 0xf0102e0c <+0>:	push   %ebp
   0xf0102e0d <+1>:	mov    %esp,%ebp
   0xf0102e0f <+3>:	sub    $0xc,%esp
   0xf0102e12 <+6>:	mov    0x8(%ebp),%esp
   0xf0102e15 <+9>:	popa   
   0xf0102e16 <+10>:	pop    %es
   0xf0102e17 <+11>:	pop    %ds
   0xf0102e18 <+12>:	add    $0x8,%esp
   0xf0102e1b <+15>:	iret   
   0xf0102e1c <+16>:	push   $0xf01052a5
   0xf0102e21 <+21>:	push   $0x1dd
   0xf0102e26 <+26>:	push   $0xf010523a
   0xf0102e2b <+31>:	call   0xf01000a9 <panic>
End of assembler dump.


6. (gdb) si 4
=> 0xf0102e15 <env_pop_tf+9>:	popa   
0xf0102e15	468		asm volatile("\tmovl %0,%%esp\n"
(gdb) print sizeof(tf)
$1 = 4
(gdb) x/4x $sp
0xf01bf000:	0x00000000	0x00000000	0x00000000	0x00000000

No nos quedo claro si debia imprimir sizeof tf o el N de los puntos anteriores. De todas formas
efectivamente da lo mismo que antes.
(gdb) x/17x $sp
0xf01bf000:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01bf010:	0x00000000	0x00000000	0x00000000	0x00000000
0xf01bf020:	0x00000023	0x00000023	0x00000000	0x00000000
0xf01bf030:	0x00800020	0x0000001b	0x00000000	0xeebfe000
0xf01bf040:	0x00000023


7. Los primeros ocho valores, son los valores de los registros correspondientes al proceso. Son los registros que forman parte del struct PushRegs (edi, esi, ebp, oesp, ebx, edx, ecx, eax).
Luego, encontramos el valor 0x023 que corresponde al ES del TrapFrame y se repite para el DS.
Trapno y tf_err valen 0x0.
En la dirección de memoria 0xf01c0030 encontramos en primer lugar el Instruction Pointer, y luego el registro CS (y el padding 3). Seguido de ellos, se encuentra EFLAGS (cuyo valor es 0).
Por último se encuentran el Stack Pointer y en la dirección 0xf01c0040, encontramos el SS.



8. info registers
EAX=00000000 EBX=00000000 ECX=00000000 EDX=00000000
ESI=00000000 EDI=00000000 EBP=00000000 ESP=f01bf030
EIP=f0102e1b EFL=00000096 [--S-AP-] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
CS =0008 00000000 ffffffff 00cf9a00 DPL=0 CS32 [-R-]

Los cambios producidos son porque ya en este paso, hemos hecho un popal que restaura todos los registros de proposito general, se realizan dos pop de %ds y %es y por ultimo se saltea trapno y errorcode. Particularmente el cambio que vemos es debido a la ejecución de la instrucción popal.


9. (gdb) p $pc
$1 = (void (*)()) 0x800020*
(gdb) symbol-file obj/user/hello
Load new symbol table from "obj/user/hello"? (y or n) y
Reading symbols from obj/user/hello...done.
Error in re-setting breakpoint 1: Function "env_pop_tf" not defined.
(gdb) p $pc
$2 = (void (*)()) 0x800020 <_start>*

(qemu) info registers
EAX=00000000 EBX=00000000 ECX=00000000 EDX=00000000
ESI=00000000 EDI=00000000 EBP=00000000 ESP=eebfe000
EIP=00800020 EFL=00000002 [-------] CPL=3 II=0 A20=1 SMM=0 HLT=0
ES =0023 00000000 ffffffff 00cff300 DPL=3 DS   [-WA]
CS =001b 00000000 ffffffff 00cffa00 DPL=3 CS32 [-R-]

Luego de haber ejecutado iret, los registros de un environment de usuario son restaurados para vovler a su ejecución.


10. 
