#TP1: Memoria virtual en JOS
===========================

##page2pa
Recibe un puntero a un struct PageInfo *pp y devuelve la direccion de memoria fisica donde comienza la pagina. A este puntero le resta la direccion donde se encuentra el primer struct PageInfo del arreglo de paginas para asi quitarle el offset y obtener el virtual page number, y le hace un shift PGSHIFT = 12, debido a que las physical page son de 4kb para alinearlo.

...


boot_alloc_pos
--------------

...


page_alloc
----------

...


