# static_assert

El static_assert de JOS, hace los chequeos en tiempo de compilacion. De esta forma,
se lanzara un error de compilacion si la condicion x no se cumple. La forma en que hace esto es
usando un switch. Si el assert da false, nuestro switch estara compuesto por dos cases iguales
generando un error de compilacion.

--------

# env_return

-  

- La diferencia mas obvia es que en este TP estamos contemplando el uso de muchos CPUs, lo que nos
permite correr varios procesos en distintos cores. Para el caso de la funcion env_destroy, esto significa
que cuando llamemmos a esta funcion, ya no estaremos destruyendo el unico proceso en ejecucion. Si el proceso que recibimos es el que se esta ejecutando se corre un nuevo proceso. Por otro lado, si el proceso que recibimos esta corriendo en otro CPU, generaremos un proceso zombie a traves del estado ENV_DYING que sera liberado la proxima vez que se tope con el kernel.

------------

#envid2env

- Como al pasarle 0 a envid2env recibimos el current enviroment, sys_env_destroy lo va a destruir.

- Se envia la senial 9 a cada proceso en el grupo de procesos de llamada.

- Tendremos un error porque ENVX no permite parametros negativos.

- Se envia la senial 9 a todos los procesos que el proceso que llama tenga permisos para enviar seniales.

------------

# dumbfork

1. No, no se propagará porque cuando estamos haciendo page_alloc alocamos una nueva página, pero no actualizamos nuestra cuenta de páginas. Esto debe hacerlo quien llame a la función. Por lo tanto cuando llamemos a dumbfork, se hará la copia del address space sin tener en cuenta la nueva página alocada.

2. No, no se preserva. De hecho cuando hacemos la copia del address space al proceso hijo, podemos ver en duppage que todos las páginas son mapeadas con permisos de escritura y lectura.

3. En duppage, primero se aloca una página a través de sys_page_alloc. Se mapea la página del proceso padre a una página del proceso hijo. Por último, se deshace el mapeo temporal.

4.

5. ROUNDDOWN se usa para redondear para abajo dejando la dirección como multiplo de PGSIZE. El parámetro addr que usamos es la dirección actual. Antes de copiar el stack, copiamos todo el resto del address space a través de un for. Cuando termina ese for, addr queda con la última dirección con la que salió del for. Es decir, la dirección del stack. Se usa ROUNDDOWN y no ROUNDUP porque sino podríamos pasarnos del espacio que tiene el stack.

------------

# multicore_init

1. Lo que se copia con memmove es el entry code del proceso en cuestión. Se copia en la dirección virtual obtenida de MPENTRY_PADDR.

2. La variable mpentry_kstack se usa porque aunque estemos usando todos los procesadores que no son de booteo, necesitamos saber qué stack usar desde mpentry.S para determinado CPU.
Justamente en los comentarios de mpentry.S, nos dicen que es similar a boot.S excepto que no es necesario habilitar A20 y que se usa MPBOOTPHYS para calcular direcciones absolutas. Entonces si tuvieramos que reservar el espacio como lo hace boot.S, no deberíamos usar MPBOOTPHYS. El problema con esto es que mentry.S es cargada por el bootloader sin ningún tratamiento especial mientras que tiene que ser cargado por un CPU bootstrap by bootloader en 0x7000. Entonces hay que delimitar la dirección de carga original.

3. ```(gdb) watch mpentry_kstack
Hardware watchpoint 1: mpentry_kstack
(gdb) continue
Continuing.
The target architecture is assumed to be i386
=> 0xf010019b <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0x0
New value = (void *) 0xf024b000 <percpu_kstacks+65536>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
(gdb) bt
#0  boot_aps () at kern/init.c:109
#1  0xf0100229 in i386_init () at kern/init.c:55
#2  0xf0100047 in relocated () at kern/entry.S:89
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 1 (CPU#0 [running]) boot_aps () at kern/init.c:109
  2    Thread 2 (CPU#1 [halted ]) 0x000fd412 in ?? ()
  3    Thread 3 (CPU#2 [halted ]) 0x000fd412 in ?? ()
  4    Thread 4 (CPU#3 [halted ]) 0x000fd412 in ?? ()
(gdb) continue
Continuing.
=> 0xf010019b <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0xf024b000 <percpu_kstacks+65536>
New value = (void *) 0xf0253000 <percpu_kstacks+98304>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 1 (CPU#0 [running]) boot_aps () at kern/init.c:109
  2    Thread 2 (CPU#1 [running]) spin_lock (lk=0xf01213c0 <kernel_lock>) at kern/spinlock.c:71
  3    Thread 3 (CPU#2 [halted ]) 0x000fd412 in ?? ()
  4    Thread 4 (CPU#3 [halted ]) 0x000fd412 in ?? ()
(gdb) thread 2
[Switching to thread 2 (Thread 2)]
#0  spin_lock (lk=0xf01213c0 <kernel_lock>) at kern/spinlock.c:71
71		while (xchg(&lk->locked, 1) != 0)
(gdb) bt
#0  spin_lock (lk=0xf01213c0 <kernel_lock>) at kern/spinlock.c:71
#1  0xf010006d in lock_kernel () at ./kern/spinlock.h:33
#2  0xf01002b9 in mp_main () at kern/init.c:134
#3  0x00007060 in ?? ()
(gdb) p cpunum()
$1 = 1
(gdb) thread 1
[Switching to thread 1 (Thread 1)]
#0  boot_aps () at kern/init.c:111
111			while(c->cpu_status != CPU_STARTED)
(gdb) p cpunum()
$2 = 0
(gdb) continue
Continuing.
=> 0xf010019b <boot_aps+127>:	mov    %esi,%ecx

Thread 1 hit Hardware watchpoint 1: mpentry_kstack

Old value = (void *) 0xf0253000 <percpu_kstacks+98304>
New value = (void *) 0xf025b000 <percpu_kstacks+131072>
boot_aps () at kern/init.c:109
109			lapic_startap(c->cpu_id, PADDR(code));```

4.

5.

-----------

# sys_ipc_try_send
