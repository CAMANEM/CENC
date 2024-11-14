#include <magick/MagickCore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
    int type; // 0 = Resistencia, 1 = Capacitor, 2 = Diodo
} Coordenada;

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
    const char *ruta_imagen = "images/PCB1000x1000.png";
    
    //
    int puntos0_count = 0;
    int coord0_count = 0;
    int start0 = 0;
    int finish0 = 333;
    //
    int puntos1_count = 0;
    int coord1_count = 0;
    int start1 = finish0;
    int finish1 = 666;
    //
    int puntos2_count = 0;
    int coord2_count = 0;
    int start2 = finish1;
    int finish2 = 1000;
    //
    int total_coord_count = 0;
    int puntos_count = 0;
    int medios_count = 0;
    


    // Procesamiento Distribuido
    Coordenada *coords0 = detect_color_clusters(ruta_imagen, &puntos0_count, &coord0_count, start0, finish0);
    Coordenada *coords1 = detect_color_clusters(ruta_imagen, &puntos1_count, &coord1_count, start1, finish1);
    Coordenada *coords2 = detect_color_clusters(ruta_imagen, &puntos2_count, &coord2_count, start2, finish2);


    // Concatenar antes de enviar a get_points
    Coordenada *temp_coords = concatenate_coords(coords0, coord0_count, coords1, coord1_count);
    Coordenada *all_coords = concatenate_coords(temp_coords, coord0_count + coord1_count, coords2, coord2_count);

    // Filtrar puntos agrupados
    Coordenada *puntos = get_points(all_coords, coord0_count + coord1_count + coord2_count, &puntos_count);




    // Mostrar los puntos generales detectados
    printf("Puntos generales detectados: %d\n", puntos_count);
    for (int i = 0; i < puntos_count; i++) {
        printf("Punto %d: (%d, %d), Tipo: %s\n", i + 1, puntos[i].x, puntos[i].y,
               puntos[i].type == 0 ? "Resistencia" :
               puntos[i].type == 1 ? "Capacitor" : "Diodo");
    }

    Coordenada *puntos_medios = calcular_puntos_medios(puntos, puntos_count, &medios_count);
    printf("\nPuntos medios detectados: %d\n", medios_count);
    for (int i = 0; i < medios_count; i++) {
        printf("Punto medio %d: (%d, %d), Tipo: %s\n", i + 1, puntos_medios[i].x, puntos_medios[i].y,
               puntos_medios[i].type == 0 ? "Resistencia" :
               puntos_medios[i].type == 1 ? "Capacitor" : "Diodo");
    }
    

    int image_size = 1000;
    Coordenada *puntos_fisicos = convert_to_physical_space(puntos_medios, medios_count, image_size);

    printf("\nPuntos medios en espacio físico:\n");
    for (int i = 0; i < medios_count; i++) {
         printf("Punto %d físico (stepsx, stepsy) : (%d, %d), Tipo: %s\n", i + 1, puntos_fisicos[i].x, puntos_fisicos[i].y,
               puntos_fisicos[i].type == 0 ? "Resistencia" :
               puntos_fisicos[i].type == 1 ? "Capacitor" : "Diodo");
    }
    
    free(coords0);
    free(coords1);
    free(coords2);
    free(temp_coords);
    free(all_coords);
    free(puntos);
    free(puntos_medios);
    free(puntos_fisicos);


    return 0;
}





// Compilación y ejecutar
// gcc image_processing.c -o image_processing `pkg-config --cflags --libs MagickCore` &&  ./image_processing


//Instalación de la librería
//sudo apt install pkgconf libmagickcore-dev