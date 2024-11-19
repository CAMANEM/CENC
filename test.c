//#include "gpipoLib.h"
#include <stdio.h>
#include "gpipoLib.h"



int main() {
		
		FILE *archivo = fopen("resultados.txt", "r");
		if (archivo == NULL) {
			fprintf(stderr, "Error al abrir el archivo para lectura.\n");
			return 1;
		}

		// Variables para almacenar los valores de stepsx y stepsy
		int stepsx, stepsy;
		char tipo[20]; // Para almacenar el tipo (Resistencia, Capacitor, Diodo)

		// Leer el archivo línea por línea con un bucle for
		// Suponiendo que el archivo tiene un número conocido de puntos (por ejemplo, 3 en este caso)
		// Para un número desconocido de puntos, usamos un bucle hasta que no haya más datos.
		char input;
		for (int i = 0; fscanf(archivo, "Punto %*d físico (stepsx, stepsy): (%d, %d), Tipo: %s\n", &stepsx, &stepsy, tipo) == 3; i++) {
			// Mostrar los valores leídos
			printf("Punto %d: stepsx = %d, stepsy = %d, Tipo = %s\n", i + 1, stepsx, stepsy, tipo);
			printf("Position the component bellow the sringe and Press Enter to continue");
			input = getchar();
			put_component(stepsx, stepsy);
			
		}

		fclose(archivo);
	
		return 0;
}
