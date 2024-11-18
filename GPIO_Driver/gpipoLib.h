#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *file_ptr;  // Puntero al archivo
char buffer[2058];
int x_steps;
int y_steps;


int open_file(const char *path);

int write_to_file(const char *text);

void decode_movements(int coords[], int width, int height);

char step_to_write(int value);

void move_x_axis(int steps);

void move_y_axis(int steps);

void return_to_origin();

void pick_up(int steps,int action);

void set_pump(int action);

void close_file();

void put_component(int x, int y);
