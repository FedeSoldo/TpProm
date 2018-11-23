#static_assert

El static_assert de JOS, hace los chequeos en tiempo de compilacion. De esta forma,
se lanzara un error de compilacion si la condicion x no se cumple. La forma en que hace esto es
usando un switch. Si el assert da false, nuestro switch estara compuesto por dos cases iguales
generando un error de compilacion.

--------

#env_return

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

------------

#multicore_init

-----------

#sys_ipc_try_send
