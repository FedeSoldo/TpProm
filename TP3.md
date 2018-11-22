static_assert

El static_assert de JOS, hace los chequeos en tiempo de compilacion. De esta forma,
se lanzara un error de compilacion si la condicion x no se cumple. La forma en que hace esto es
usando un switch. Si el assert da false, nuestro switch estara compuesto por dos cases iguales
generando un error de compilacion.
