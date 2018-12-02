# Parte 1

- Se recomienda leer la función diskaddr() en el archivo fs/bc.c. ¿Qué es super->s_nblocks?

	Super es el superbloque. En este caso lo que se consulta es la cantidad de bloques que contiene el superbloque. Se está usando en la función de diskaddr para hacer cierto chequeo de sanidad.
