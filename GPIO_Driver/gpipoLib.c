#include "gpipoLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *file_ptr;  // Puntero al archivo
char buffer[2058];
int x_steps;
int y_steps;

typedef enum {
		steppers_OFF = '0',
		stepperY_ON = '1'
} command;

/*
gcc -o gpipoLib gpipoLib.c
./gpipoLib
* 
gcc -c gpipoLib.c -o gpipoLib.o
ar rcs gpipoLib.a gpipoLib.o
* 

gcc -c test.c -o test.o
gcc -o test test.c -L. gpipoLib.a
./test
* 
* 
* Usar enums para acciones
* COnvertir esto a bliblioteca .a
* Llamar esto desde donde daniel
* hacerle .h a esto
*/

// Función para abrir el archivo en modo de escritura
int open_file(const char *path) {
    file_ptr = fopen(path, "w");
    if (file_ptr == NULL) {
        perror("Error al abrir el archivo");
        return -1;
    }
    printf("Archivo abierto exitosamente: %s\n", path);
    return 0;
}

// Función para escribir caracteres en el archivo
int write_to_file(const char *text) {
    if (file_ptr == NULL) {
        fprintf(stderr, "El archivo no está abierto.\n");
        return -1;
    }

    // Escribe el texto en el archivo
    if (fputs(text, file_ptr) == EOF) {
        perror("Error al escribir en el archivo");
        return -1;
    }
    printf("Texto escrito en el archivo: %s\n", text);
    return 0;
}

void decode_movements(int coords[], int width, int height) {
    // Empty function for future implementation
}

char step_to_write(int value) {
    switch (value) {
    	// 	OFF
        case 0: return steppers_OFF;
        //	X Axis
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        //	Y Axis
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        //	Z Axis
        case 9: return '9';
        case 10: return 'a';
        case 11: return 'b';
        case 12: return 'c';
        //	Picker off/on
        case 13: return 'd';
        case 14: return 'e';
        default:
            printf("No matching case\n");
            return '?';  // Default return if no match
    }
}

void move_x_axis(int steps) {
	x_steps = steps;
    for (int i = 0; i <= steps; i++) {
        char step = step_to_write((i % 4) + 1);
        strncat(buffer, &step, 1);  // Appends one character
    }
    strncat(buffer, "0", 1);  // Appends the terminating "0"
}

void move_y_axis(int steps) {
	y_steps = steps;
   for (int i = 0; i <= steps; i++) {
        char step = step_to_write((i % 4) + 5);
        strncat(buffer, &step, 1);  // Appends one character
    }
    strncat(buffer, "0", 1);  // Appends the terminating "0"
}

void return_to_origin(){

	 for (int i = x_steps; i >= 0; i--) {
        char step = step_to_write((i % 4) + 1);
        strncat(buffer, &step, 1);  // Appends one character
    }
    strncat(buffer, "0", 1);  // Appends the terminating "0"



	for (int i = y_steps; i >= 0; i--) {
        char step = step_to_write((i % 4) + 5);
        strncat(buffer, &step, 1);  // Appends one character
    }
    strncat(buffer, "0", 1);  // Appends the terminating "0"
}

void pick_up(int steps,int action) {
    
    // sube eje z
    if(action == 0){
        for (int i = steps; i >= 0; i--) {
            char step = step_to_write((i % 4) + 9);
            strncat(buffer, &step, 1);  // Appends one character
        }
        strncat(buffer, "0", 1);  // Appends the terminating "0"
        //strncat(buffer, "d", 1);  // Appends one character
    }
    
    // sube baja z
    if(action == 1){
        //strncat(buffer, "e", 1);  // Appends one character
        for (int i = 0; i <= steps; i++) {
            char step = step_to_write((i % 4) + 9);
            strncat(buffer, &step, 1);  // Appends one character
        }
        strncat(buffer, "0", 1);  // Appends the terminating "0"
    }
    /*
    else{
    strncat(buffer, "d", 1);  // Appends one character
    for (int i = steps; i >= 0; i--) {
        char step = step_to_write((i % 4) + 9);
        strncat(buffer, &step, 1);  // Appends one character
    }
    strncat(buffer, "0", 1);  // Appends the terminating "0"
    
    
    }
    */
    
}

void set_pump(int action){
    // TURN OFF 
    if(action == 0){
        strncat(buffer, "d", 1);  // Appends one character
    }
    // TURN ON
    if(action == 1){
        strncat(buffer, "e", 1);  // Appends one character
        usleep(250000);
    }
}

// Función para cerrar el archivo
void close_file() {
    if (file_ptr != NULL) {
        fclose(file_ptr);
        printf("Archivo cerrado exitosamente.\n");
    }
            printf("Sí funciona bro, todo tranqui.\n");

}

void put_component(int x, int y){
    
    const char *path = "/dev/gpipoDriver";  // Cambia esta ruta

    // Abrir el archivo
    if (open_file(path) != 0) {
        //error
    }
    set_pump(1);
    pick_up(100, 0);
    
    move_x_axis(x);
    move_y_axis(y);
    
    pick_up(50, 1);
    set_pump(0);
    pick_up(100, 0);
    
    return_to_origin();
    
    pick_up(50, 1);
    
    // Escribir en el archivo
    if (write_to_file(buffer) != 0) {
        close_file();
        //error
    }

    // Cerrar el archivo
    close_file();
    memset(buffer, 0, sizeof(buffer));
}
/*
int main() {
    
    put_component(99, 170);
    
    return 0;
    
}

*/
