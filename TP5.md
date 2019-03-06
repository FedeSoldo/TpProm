1. How did you structure your transmit implementation? In particular, what do you do if the transmit ring is full?

Si el "anillo de transmicion" se encuentra lleno devuelvo -1 e imprimo un mensaje para indicar que la causa fue esta
ya que tambien devuelvo -1 en caso que el tamaño del paquete supere el maximo (-E_INVAL es -1).

2. How did you structure your receive implementation? In particular, what do you do if the receive queue is empty and a user environment requests the next incoming packet?

Si la cola esta vacia mi valor de retorno es -1, de esta manera notifico que algo sucedio, e imprimo un mensaje de error al igual que para transmit, ya que devuelvo -1 tambien en caso que el paquete sea mas largo que lo que quiero recibir.

3. What does the web page served by JOS's web server say?

"This file came from JOS. Cheesy web page!"

4. How long approximately did it take you to do this lab?

30 horas aprox.
