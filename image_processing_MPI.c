#include <magick/MagickCore.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
    int x;
    int y;
    int type; // 0 = Resistencia, 1 = Capacitor, 2 = Diodo
} Coordenada;
// Función encargada de obtener los puntos generales de la imagen clasificados por tipo
Coordenada *get_points(Coordenada *coords, int pixel_count, int *puntos_count) {
    int threshold = 25;
    Coordenada *puntos = NULL;
    *puntos_count = 0;

    for (int i = 0; i < pixel_count; i++) {
        bool es_nuevo_punto = true;

        // Revisar si el punto actual ya pertenece a un grupo existente
        for (int j = 0; j < *puntos_count; j++) {
            if(abs(coords[i].x - puntos[j].x) <= threshold && abs(coords[i].y - puntos[j].y) <= threshold){
                es_nuevo_punto = false;
                break;
            }
        }

        // Si es un nuevo punto, se agrega a la lista
        if (es_nuevo_punto) {
            (*puntos_count)++;
            puntos = realloc(puntos, (*puntos_count) * sizeof(Coordenada));
            puntos[*puntos_count - 1] = coords[i];
        }
    }
    return puntos;
}

// Función encargada de detectar las coordenadas de los pixeles de cada color
Coordenada *detect_color_clusters(const char *ruta_imagen, int *total_puntos,int *coord_count, int start, int finish) {
    MagickCoreGenesis(NULL, MagickTrue);
    ExceptionInfo *exception = AcquireExceptionInfo();

    // Cargar la imagen
    ImageInfo *image_info = CloneImageInfo((ImageInfo *)NULL);
    strcpy(image_info->filename, ruta_imagen);
    Image *image = ReadImage(image_info, exception);

    //int width = image->columns;
    int height = image->rows;
    //printf("Imagen cargada: %s\n", image->filename);
    //printf("Tamaño: %d x %d\n", width, height);

    // Obtener los píxeles
    CacheView *image_view = AcquireVirtualCacheView(image, exception);

    // Colores de referencia en RGB
    int resistenciaR = 239, resistenciaG = 18, resistenciaB = 52;
    int capacitorR = 171, capacitorG = 205, capacitorB = 18;
    int diodoR = 18, diodoG = 58, diodoB = 188;

    Coordenada *coords = NULL;

    for (int i = 0; i < height; i++) {
        for (int j = start; j < finish; j++) {
            // Obtener el píxel en la posición (i, j)
            const PixelPacket *pixel = GetCacheViewVirtualPixels(image_view, j, i, 1, 1, exception);

            // Clasificar el tipo de componente en base al color
            if (pixel->red >> 8 == resistenciaR && pixel->green >> 8 == resistenciaG && pixel->blue >> 8 == resistenciaB) {
                // Resistencia detectada
                (*coord_count)++;
                coords = realloc(coords, (*coord_count) * sizeof(Coordenada));
                coords[*coord_count - 1].x = j;
                coords[*coord_count - 1].y = i;
                coords[*coord_count - 1].type = 0;
            }
            else if (pixel->red >> 8 == capacitorR && pixel->green >> 8 == capacitorG && pixel->blue >> 8 == capacitorB) {
                // Capacitor detectado
                (*coord_count)++;
                coords = realloc(coords, (*coord_count) * sizeof(Coordenada));
                coords[*coord_count - 1].x = j;
                coords[*coord_count - 1].y = i;
                coords[*coord_count - 1].type = 1;
            }
            else if (pixel->red >> 8 == diodoR && pixel->green >> 8 == diodoG && pixel->blue >> 8 == diodoB) {
                // Diodo detectado
                (*coord_count)++;
                coords = realloc(coords, (*coord_count) * sizeof(Coordenada));
                coords[*coord_count - 1].x = j;
                coords[*coord_count - 1].y = i;
                coords[*coord_count - 1].type = 2;
            }
        }
    }
    
    // Liberar recursos
    //free(coords);
    image_view = DestroyCacheView(image_view);
    image = DestroyImage(image);
    image_info = DestroyImageInfo(image_info);
    exception = DestroyExceptionInfo(exception);
    MagickCoreTerminus();
    return coords;
}

// Función para obtener los puntos generales cercanos y calcular el punto medio entre ellos
Coordenada* calcular_puntos_medios(Coordenada *puntos, int puntos_count, int *medios_count) {
    int umbral = 110;
    Coordenada *puntos_medios = NULL;
    *medios_count = 0;

    for (int i = 0; i < puntos_count; i++) {
        for (int j = i + 1; j < puntos_count; j++) {
            if (puntos[i].type == puntos[j].type) {
                int distancia_x = abs(puntos[i].x - puntos[j].x);
                int distancia_y = abs(puntos[i].y - puntos[j].y);

                if (distancia_x <= umbral && distancia_y <= umbral) {
                    Coordenada punto_medio;
                    punto_medio.x = (puntos[i].x + puntos[j].x) / 2;
                    punto_medio.y = (puntos[i].y + puntos[j].y) / 2;
                    punto_medio.type = puntos[i].type;

                    (*medios_count)++;
                    puntos_medios = realloc(puntos_medios, (*medios_count) * sizeof(Coordenada));
                    puntos_medios[(*medios_count) - 1] = punto_medio;
                }
            }
        }
    }
    return puntos_medios;
}

// Función para realizar el mapeo del espacio de la imagen al espacio físico de 5x5 cm
Coordenada *convert_to_physical_space(Coordenada *puntos, int medios_count, int image_size) {
    float cantidad_steps_total = 250;  //cantidad de steps que hay para ir de un lado al otro de la placa física
    float mapeo = image_size / cantidad_steps_total;
    Coordenada *puntos_fisicos = malloc(medios_count * sizeof(Coordenada));

    //printf("tamaño de paso total %f\n", cantidad_steps_total);
    //printf("MAPEO %f\n", mapeo);
    
    for (int i = 0; i < medios_count; i++) {
        puntos_fisicos[i].x = puntos[i].x / mapeo;
        puntos_fisicos[i].y = puntos[i].y / mapeo;
        puntos_fisicos[i].type = puntos[i].type;
    }
    return puntos_fisicos;
}

// Función para concatenar las listas de coords
Coordenada *concatenate_coords(Coordenada *puntos1, int count1, Coordenada *puntos2, int count2) {
    int total_count = count1 + count2;
    Coordenada *puntos_concatenados = malloc(total_count * sizeof(Coordenada));

    // Copiar puntos1 a la nueva lista
    memcpy(puntos_concatenados, puntos1, count1 * sizeof(Coordenada));

    // Copiar puntos2 a continuación de puntos1 en la nueva lista
    memcpy(puntos_concatenados + count1, puntos2, count2 * sizeof(Coordenada));

    return puntos_concatenados;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <ruta_imagen>\n", argv[0]);
        return 1;
    }
    
    // Leer la ruta de la imagen
    const char *ruta_imagen = argv[1];
    
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int puntos_count = 0;
    int coord_count = 0;
    Coordenada *coords = NULL;

    // Definir el rango de trabajo para cada proceso
    int start, finish;
    if (rank == 0) {       // Nodo maestro
        // Recibir coords1 y coords2 de otros nodos
        int coords1_count, coords2_count;
        Coordenada *coords1 = NULL, *coords2 = NULL;

        // Recibir coords1 desde nodo 1
        MPI_Recv(&coords1_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        coords1 = malloc(coords1_count * sizeof(Coordenada));
        MPI_Recv(coords1, coords1_count * sizeof(Coordenada), MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recibir coords2 desde nodo 2
        MPI_Recv(&coords2_count, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        coords2 = malloc(coords2_count * sizeof(Coordenada));
        MPI_Recv(coords2, coords2_count * sizeof(Coordenada), MPI_BYTE, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Concatenar resultados en el nodo maestro
        Coordenada *all_coords = concatenate_coords(coords1, coords1_count, coords2, coords2_count);

        int total_coord_count = coords1_count + coords2_count;
        Coordenada *puntos = get_points(all_coords, total_coord_count, &puntos_count);

        // Postprocesamiento como en el código original
        int medios_count = 0;
        Coordenada *puntos_medios = calcular_puntos_medios(puntos, puntos_count, &medios_count);
        Coordenada *puntos_fisicos = convert_to_physical_space(puntos_medios, medios_count, 1000);

        // Mostrar resultados
        for (int i = 0; i < medios_count; i++) {
            printf("Punto %d físico (stepsx, stepsy): (%d, %d), Tipo: %s\n", i + 1, puntos_fisicos[i].x, puntos_fisicos[i].y,
                   puntos_fisicos[i].type == 0 ? "Resistencia" :
                   puntos_fisicos[i].type == 1 ? "Capacitor" : "Diodo");
        }
        
        
        
	printf("Agregar logica de continue con biblioteca \n");
	
		// Abrir el archivo para escribir
	FILE *archivo = fopen("resultados.txt", "w");
	if (archivo == NULL) {
	    fprintf(stderr, "Error al abrir el archivo para escribir.\n");
	    return 1;
	}

	// Mostrar resultados y escribir en el archivo
	for (int i = 0; i < medios_count; i++) {
	    // Mostrar en la terminal
	    printf("Punto %d físico (stepsx, stepsy): (%d, %d), Tipo: %s\n", 
		   i + 1, puntos_fisicos[i].x, puntos_fisicos[i].y,
		   puntos_fisicos[i].type == 0 ? "Resistencia" :
		   puntos_fisicos[i].type == 1 ? "Capacitor" : "Diodo");

	    // Escribir en el archivo
	    fprintf(archivo, "Punto %d físico (stepsx, stepsy): (%d, %d), Tipo: %s\n",
		    i + 1, puntos_fisicos[i].x, puntos_fisicos[i].y,
		    puntos_fisicos[i].type == 0 ? "Resistencia" :
		    puntos_fisicos[i].type == 1 ? "Capacitor" : "Diodo");
	}

	// Cerrar el archivo
	fclose(archivo);

	printf("Resultados guardados en 'resultados.txt'.\n");
	
	
	
        // Liberar memoria en el nodo maestro
        free(coords1);
        free(coords2);
        free(all_coords);
        free(puntos);
        free(puntos_medios);
        free(puntos_fisicos);
        
        

    } else if (rank == 1) {  // Nodo 1
        start = 0;
        finish = 499;
        Coordenada *coords1 = detect_color_clusters(ruta_imagen, &puntos_count, &coord_count, start, finish);

        // Enviar resultado al nodo maestro
        MPI_Send(&coord_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        sleep(5);
        MPI_Send(coords1, coord_count * sizeof(Coordenada), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

        free(coords1);

    } else if (rank == 2) {  // Nodo 2
        start = 499;
        finish = 1000;
        Coordenada *coords2 = detect_color_clusters(ruta_imagen, &puntos_count, &coord_count, start, finish);

        // Enviar resultado al nodo maestro
        MPI_Send(&coord_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        sleep(5);
        MPI_Send(coords2, coord_count * sizeof(Coordenada), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

        free(coords2);

    }

    MPI_Finalize();
    
    return 0;
}




// Compilación y ejecutar con mpi
//mpicc -o ip_MPI image_processing_MPI.c `pkg-config --cflags --libs MagickCore`
//mpirun -hostfile hosts ./ip_MPI Images/decrypted_image.png

// Instalación de la biblioteca
//sudo apt install pkgconf libmagickcore-dev
