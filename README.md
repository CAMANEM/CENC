# CENC
CE4303 Operating Systems Project #2 - Pick&amp;Place CNC

## Primer Dealine: Domingo 3 de noviembre tener completas las siguiente 4 secciones:

## Servidor-Cluster (Emanuel Marín)

Crear un sistema distribuido con al menos 3 nodos (1 master y dos esclavos) utilizando OpenMPI. Para ello primero se harán pruebas con 3 ordenadores y luego se proborará el sistema con dos ordenadores y la Raspberry Pi como nodo master.

Nota: Por ahora el sistema distribuido será capaz de que el nodo master tome una imagen (de una base de datos local o en la nube), la divida en 16 cuadrantes y asigne a los nodos esclavos dichos cuadrantes para que le calculen la interpolación bilineal. El resultado final debe ser igual a haber aplicado interpolación bilineal a la imagen original.

## Biblioteca (Daniel Castro)

Se debe desarrollar o integrar una biblioteca en C capaz de analizar una imagen (o un cuadrante de esta) tal que se puedan reconocer colores y figuras en un diagrama de circuito PCB, para de esta forma mapear las coordenadas en las que se encuentran. 

Una vez mapeada las coordenadas y los tipos de figuras, la biblioteca debe ser capaz de definir la lógica de movimiento que deben hacer los stepper para colocar el componente físico (relacionado con el tipo de figura) en la placa PCB.

Nota: Daniel se pondrá de acuerdo con Chen y Oscar para definir una imagen o diagrama de circuito PCB de ejemplo que la biblioteca debe ser capaz de analizar y mapear.

## Hardware (Sebastian Chen  y Oscar Soto)

Definir el hardware necesario para poder colocar componentes en una placa PCB a partir steppers.

Crear una maqueta sólida y funcional que permita comprobar su funcionamiento.

## Driver (Sebastian Chen  y Oscar Soto)

Crear un driver y cargarlo en la Raspberry Pi que sea capaz de tomar el resultado que genera la biblioteca y a partir de eso activar los puertos GPIO que están conectados al hardware para poder colocar los componentes de forma automática.

Nota: Se necesita tener comunicación con Daniel para tener claro qué es lo que la biblioteca devuelve y a partir de eso hacer el driver.

Nota: De momento se puede suponer qué es lo que devolverá la biblioteca para poder avanzar con el driver y hacerle pruebas al hardware. Por ejemplo cuánto steps debe dar el GPIO X para que el servomotor o stepper X funcione, etc.