#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

typedef struct {
    int id;
    int happiness;
} Penguin;

typedef struct {
    int id;
    int capacity;
    int num_penguins;
    int num_maxheap;
    int num_minheap;
    Penguin *minHeapMedian; // Heap para la mitad inferior de la mediana
    Penguin *maxHeapMedian; // Heap para la mitad superior de la mediana
    Penguin *minHeap;       // Heap que contiene el menor pingüino
    Penguin *maxHeap;
} Sector;


typedef struct {
    int num_sectors;      // Número de sectores en el festival
    Penguin *bigHeap;     // Puntero al heap que contiene todos los pingüinos
    int bigHeapSize;      // Tamaño del bigHeap
    Sector *sectors;      // Array dinámico de sectores
} Festival;


// Declaraciones de funciones
void init_festival(Festival *festival, int num_sectors, int *capacities);
void free_festival(Festival *festival);
void shift_up_min_heap(Penguin *heap, int index);
void shift_up_max_heap(Penguin *heap, int index);
void shift_down_min_heap(Penguin *heap, int index, int n); // Declaración añadida
void shift_down_max_heap(Penguin *heap, int index, int n); // Declaración añadida


// Funciones de eventos
void add_penguin(Sector *sector, int penguin_id, int happiness);
void add_penguin_festival(Festival *festival, int penguin_id,int happiness, int sector_id);
char* increase_happines(Sector *sector, int penguin_id, int happiness_increase);
char* festival_details(Festival *festival);
char* Leave(Festival *festival);
//char* Leave(Festival *festival);

#endif

#include <stdio.h>
#include <string.h>

void free_festival(Festival *festival) {
    for (int i = 0; i < festival->num_sectors; i++) {
        free(festival->sectors[i].minHeapMedian);
        free(festival->sectors[i].maxHeapMedian);
        free(festival->sectors[i].minHeap);
        free(festival->sectors[i].maxHeap);
        
        //free(festival->sectors[i].happiest_penguin);
    }
    free(festival->bigHeap);
    free(festival->sectors);
}

void init_festival(Festival *festival, int num_sectors, int *capacities) {
    festival->num_sectors = num_sectors;
    festival->sectors = malloc(num_sectors * sizeof(Sector));

    // Inicializar el bigHeap
    
    int capacidad_big_heap = 0;
    for (int i = 0; i < num_sectors; i++) {
        festival->sectors[i].id = i;
        festival->sectors[i].capacity = capacities[i];
        festival->sectors[i].num_penguins = 0;
        festival->sectors[i].num_maxheap = 0;
        festival->sectors[i].num_minheap = 0;
        festival->sectors[i].minHeapMedian = malloc(capacities[i] * sizeof(Penguin));
        festival->sectors[i].maxHeapMedian = malloc(capacities[i] * sizeof(Penguin));
        festival->sectors[i].minHeap = malloc(capacities[i] * sizeof(Penguin));
        festival->sectors[i].maxHeap = malloc(capacities[i] * sizeof(Penguin));
        capacidad_big_heap = capacidad_big_heap + capacities[i];
    }
    festival->bigHeap = malloc(num_sectors * sizeof(Penguin) * capacidad_big_heap); 
    festival->bigHeapSize = 0;
}

void print_heap(Penguin *heap, int size, const char *heapName, int sector_id) {
    printf("heap:%s - from: %d: ", heapName, sector_id);
    for (int i = 0; i < size; i++) {
        if (heapName == "Bigheap After"){
            printf("(%d, %d, %d) ", heap[i].id, heap[i].happiness, sector_id);
        }
        else {
            printf("(%d, %d) ", heap[i].id, heap[i].happiness); 
        }
        
    }
    printf("\n");
}

void shift_up_min_heap(Penguin *heap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre

    int parentIndex = (index - 1) / 2;

    if (heap[parentIndex].happiness < heap[index].happiness || 
        (heap[parentIndex].happiness == heap[index].happiness && heap[parentIndex].id < heap[index].id)) {
        // Intercambiar el padre con el hijo
        Penguin temp = heap[parentIndex];
        heap[parentIndex] = heap[index];
        heap[index] = temp;

        // Llamar recursivamente para ajustar el padre
        shift_up_min_heap(heap, parentIndex);
    }
}

void shift_up_max_heap(Penguin *heap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre

    int parentIndex = (index - 1) / 2;

    if ((heap[parentIndex].happiness > heap[index].happiness || 
        (heap[parentIndex].happiness == heap[index].happiness && heap[parentIndex].id > heap[index].id)) && heap[index].happiness > 0) {
        // Intercambiar el padre con el hijo
        Penguin temp = heap[parentIndex];
        heap[parentIndex] = heap[index];
        heap[index] = temp;

        // Llamar recursivamente para ajustar el padre
        shift_up_max_heap(heap, parentIndex);
    }
}

void shift_up_big_heap(Penguin *bigHeap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre

    int parentIndex = (index - 1) / 2;

    // Ajuste por felicidad y por id en caso de empate
    if (bigHeap[parentIndex].happiness > bigHeap[index].happiness || 
        (bigHeap[parentIndex].happiness == bigHeap[index].happiness && bigHeap[parentIndex].id > bigHeap[index].id)) {
        Penguin temp = bigHeap[parentIndex];
        bigHeap[parentIndex] = bigHeap[index];
        bigHeap[index] = temp;

        // Llamar recursivamente para ajustar el padre
        shift_up_big_heap(bigHeap, parentIndex);
    }
}

void shift_down_all_heap(Penguin *heap, int index, int n) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < n && heap[left].happiness < heap[smallest].happiness) {
        smallest = left;
    }
    if (right < n && heap[right].happiness < heap[smallest].happiness) {
        smallest = right;
    }
    if (smallest != index) {
        Penguin temp = heap[index];
        heap[index] = heap[smallest];
        heap[smallest] = temp;
        shift_down_all_heap(heap, smallest, n);
    }
}

void shift_down_min_heap(Penguin *heap, int index, int n) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // Ignorar (-1, 0) en las comparaciones
    if (left < n && (heap[left].happiness >= 0) && (heap[left].happiness < heap[smallest].happiness)) {
        smallest = left;
    }
    if (right < n && (heap[right].happiness >= 0) && (heap[right].happiness < heap[smallest].happiness)) {
        smallest = right;
    }
    if (smallest != index) {
        Penguin temp = heap[index];
        heap[index] = heap[smallest];
        heap[smallest] = temp;
        shift_down_min_heap(heap, smallest, n);
    }
}

void shift_down_max_heap(Penguin *heap, int index, int n) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // Ignorar (-1, 0) en las comparaciones
    if (left < n && (heap[left].happiness >= 0) && (heap[left].happiness > heap[largest].happiness)) {
        largest = left;
    }
    if (right < n && (heap[right].happiness >= 0) && (heap[right].happiness > heap[largest].happiness)) {
        largest = right;
    }
    if (largest != index) {
        Penguin temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;
        shift_down_max_heap(heap, largest, n);
    }
}


//Falta hacer conteo de ping en heaps, y si estan los dos heaps con los mismos se deberia meter al max, pero antes 
//se compara con el mayor del min ya que sino se pone en su lugar y ese se va al max y luego reordenar

void add_penguin_festival(Festival *festival, int penguin_id, int happiness, int sector_id) {
   
    // Agregar el nuevo pingüino al final del bigHeap
    Penguin newPenguin = {penguin_id, happiness, sector_id};
    festival->bigHeap[festival->bigHeapSize] = newPenguin; // Usar festival->bigHeap
    festival->bigHeapSize++; // Incrementar el tamaño del bigHeap

    // Ajustar el bigHeap para mantener la propiedad del heap
    shift_up_big_heap(festival->bigHeap, festival->bigHeapSize - 1);
    print_heap(festival->bigHeap, festival->bigHeapSize, "Bigheap After", sector_id);
}

void add_penguin(Sector *sector, int penguin_id, int happiness) {
    if (sector->num_penguins >= sector->capacity) {
        printf("Sector %d lleno\n", sector->id);
        return;
    }
    int entro = 0;
    Penguin newPenguin = {penguin_id, happiness};

    if (sector->num_maxheap == 0 && entro == 0) {
        sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
        shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
        sector->num_maxheap++;
        entro++;
    } else if (sector->num_penguins == 1 && entro == 0) {
        sector->minHeapMedian[sector->num_minheap] = newPenguin;
        shift_up_min_heap(sector->minHeapMedian, sector->num_minheap);
        sector->num_minheap++;
        printf("\n\nprimer  %d\n\n\n",  sector->num_minheap);
        entro++;
    }
    // Agregar al maxHeapMedian o minHeapMedian
    if (sector->num_maxheap == sector->num_minheap && entro == 0) {
        if (happiness < sector->minHeapMedian[0].happiness) {
            // Agregar a minHeapMedian
            sector->minHeapMedian[sector->num_minheap] = newPenguin;
            shift_up_min_heap(sector->minHeapMedian, sector->num_minheap);
            sector->num_minheap++;
        } else {
            // Agregar a maxHeapMedian
            sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
            shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
            sector->num_maxheap++;
        }
    }
    else if ((sector->num_maxheap > 0 && happiness >= sector->maxHeapMedian[0].happiness) && entro == 0) {
        // Agregar a maxHeapMedian
        printf("\n\nkokokokokokk\n\n");
        //if (sector->num_maxheap == sector->num_minheap && )
        sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
        shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
        sector->num_maxheap++;
        entro++;

    } else {
        // Agregar a minHeapMedian
        if(sector->num_maxheap != 0 && entro == 0){
            printf("\n\nssss\n\n");
            sector->minHeapMedian[sector->num_minheap] = newPenguin;
            shift_up_min_heap(sector->minHeapMedian, sector->num_minheap);
            sector->num_minheap++;
            entro++;
        }
    }

    // Agregar al heap de todos los pingüinos
    sector->minHeap[sector->num_penguins] = newPenguin;
    shift_up_max_heap(sector->minHeap, sector->num_penguins);
    sector->maxHeap[sector->num_penguins] = newPenguin;
    shift_up_min_heap(sector->maxHeap, sector->num_penguins);
    sector->num_penguins++;

    // // Equilibrar los heaps si es necesario
    // if (sector->num_penguins > 1 && 
    //     sector->maxHeapMedian[0].happiness > sector->minHeapMedian[0].happiness) {
    //     // Intercambiar el máximo de maxHeapMedian con el mínimo de minHeapMedian
    //     Penguin temp = sector->maxHeapMedian[0];
    //     sector->maxHeapMedian[0] = sector->minHeapMedian[0];
    //     sector->minHeapMedian[0] = temp;

    //     // Reajustar ambos heaps
    //     shift_down_max_heap(sector->maxHeapMedian, 0, sector->num_penguins);
    //     shift_down_min_heap(sector->minHeapMedian, 0, sector->num_penguins);
    // }

    // Actualizar el pingüino más feliz si es necesario
    // if (!sector->happiest_penguin || newPenguin.happiness > sector->happiest_penguin->happiness) {
    //     int indice = sector->num_penguins - 1;
    //     sector->happiest_penguin = &newPenguin; 
    //     printf("\n\nactualizando\n");
    // }

    // Logs después de reajustar los heaps
    print_heap(sector->minHeapMedian, sector->num_penguins, "MinHeap Median After", sector->id);
    print_heap(sector->maxHeapMedian, sector->num_penguins, "MaxHeap Median After", sector->id);
    print_heap(sector->minHeap, sector->num_penguins, "MinHeap After", sector->id);
    print_heap(sector->maxHeap, sector->num_penguins, "MaxHeap After", sector->id);
    printf("Happiest: %d with %d\n", sector->maxHeap[0].id, sector->maxHeap[0].happiness);
}




int calculate_median_happiness(Sector *sector) {
    int total_penguins = sector->num_penguins;

    if (total_penguins % 2 == 1) {
        return sector->maxHeapMedian[0].happiness; // Mediana en un número impar
    } else {
        return (sector->maxHeapMedian[0].happiness + sector->minHeapMedian[0].happiness) / 2; // Mediana en un número par
    }
}

char* increase_happines(Sector *sector, int penguin_id, int happiness_increase) {
    int index = -1;

    // Buscar el pingüino en el heap de todos
    for (int i = 0; i < sector->num_penguins; i++) {
        if (sector->minHeap[i].id == penguin_id) {
            index = i;
            break;
        }
    }

    // Aumentar la felicidad en el heap de todos
    sector->minHeap[index].happiness += happiness_increase;

    // Reajustar el heap de todos
    shift_up_min_heap(sector->minHeap, index);

    // Actualizar los heaps de mediana
    shift_up_min_heap(sector->minHeapMedian, index);
    shift_up_max_heap(sector->maxHeapMedian, index);

    // Actualizar el pingüino más feliz si es necesario
    shift_up_min_heap(sector->maxHeap, index);

    char *message = malloc(100);
    snprintf(message, 100, "Penguin %d happiness updated to %d\n", penguin_id, sector->minHeap[index].happiness);
    return message;
}

char* festival_details(Festival *festival) {
    size_t message_size = 1000;
    char *message = malloc(message_size);

    size_t used = 0;
    used += snprintf(message + used, message_size - used, "Festival Stats:\n");

    for (int i = 0; i < festival->num_sectors; i++) {
        Sector sector = festival->sectors[i];
        int total_penguins = sector.num_penguins;        
        int median_happiness = calculate_median_happiness(&sector);
        int happiest_penguin_id = sector.maxHeap[0].id;
        int saddest_penguin_id = sector.minHeap[0].id;

        char buffer[100];
        int needed = snprintf(buffer, sizeof(buffer), "    Place %d has %d penguin[s]:\n", sector.id, total_penguins);
        if (used + needed >= message_size) {
            message_size *= 2;  
            message = realloc(message, message_size);
        }
        strcat(message + used, buffer);
        used += needed;

        needed = snprintf(buffer, sizeof(buffer), "        %d %d %d\n",
                          median_happiness, happiest_penguin_id, saddest_penguin_id);
        if (used + needed >= message_size) {
            message_size *= 2;
            message = realloc(message, message_size);
        }
        strcat(message + used, buffer);
        used += needed;
    }
    return message;
}


char* Leave(Festival *festival) {
    int min_median = festival->sectors[0].maxHeap[0].happiness; //asi nos aseguramos que es mayor a la mediana
    int sector_index = 0; 

    // Paso 1: Encontrar el sector con la menor mediana
    for (int i = 0; i < festival->num_sectors; i++) {
        int median_happiness = calculate_median_happiness(&festival->sectors[i]);
        if (median_happiness < min_median) {
            min_median = median_happiness;
            sector_index = i;
        }
    }
    Sector *selected_sector = &festival->sectors[sector_index];
    printf("deberia salir %d del sector %d con %d\n", selected_sector->minHeap[0].id, sector_index, selected_sector->minHeap[0].happiness);
    char *message = malloc(100);
    snprintf(message, 100, "Penguin %d leaving sector %d\n", selected_sector->minHeap[0].id, sector_index);
    //remove_penguin_from_heaps(selected_sector, selected_sector->minHeap[0].id);
    return message;
}

// void remove_penguin_from_heaps(Sector *sector, int penguin_id) {
//     // Remove the penguin from the minHeap
//     sector->minHeap[0] = sector->minHeap[sector->num_penguins - 1];
//     sector->num_penguins--;
//     shift_down_max_heap(sector->minHeap, 0, sector->num_penguins);

//     // Remove the penguin from the maxHeap
//     sector->maxHeap[penguin_index] = sector->maxHeap[sector->num_penguins];
//     shift_down_min_heap(sector->maxHeap, penguin_index, sector->num_penguins);

//     // Remove the penguin from the minHeapMedian
//     int min_heap_median_index = -1;
//     for (int i = 0; i < sector->num_minheap; i++) {
//         if (sector->minHeapMedian[i].id == sector->minHeap[penguin_index].id) {
//             min_heap_median_index = i;
//             break;
//         }
//     }
//     sector->minHeapMedian[min_heap_median_index] = sector->minHeapMedian[sector->num_minheap - 1];
//     sector->num_minheap--;
//     shift_down_min_heap(sector->minHeapMedian, min_heap_median_index, sector->num_minheap);
// }


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}

static bool check_arguments(int argc, char **argv) {
  if (argc != 3) {
    printf("Modo de uso: %s INPUT OUTPUT\n", argv[0]);
    printf("Donde:\n");
    printf("\tINPUT es la ruta del archivo de input\n");
    printf("\tOUTPUT es la ruta del archivo de output\n");
    exit(1);
  }
  return true;
}

int main(int argc, char **argv) {
  check_arguments(argc, argv);

  FILE *input_file = fopen(argv[1], "r");
  FILE *output_file = fopen(argv[2], "w");

  int S; // Cantidad de sectores
  int K; // Capacidad de un sector
  int E; // Cantidad de eventos

  fscanf(input_file, "%d", &S);
  int sector_capacities[S];  // Array para almacenar las capacidades de cada sector

  for (int i = 0; i < S; i++) {
      fscanf(input_file, "%d", &K);
      sector_capacities[i] = K;  // Almacenar la capacidad del sector en el array
  }

  Festival festival;
  init_festival(&festival, S, sector_capacities);  // Inicializa el festival con los sectores y sus capacidades

  fscanf(input_file, "%d", &E);

  char command[32];
  /* Eventos */

  for (int i = 0; i < E; i++) {
    char *output = NULL;
    fscanf(input_file, "%s", command);
    // Manejar cada tipo de evento
    if (string_equals(command, "ENTER")) {
      int sector_id, penguin_id, happiness;
      fscanf(input_file, "%d %d %d", &sector_id, &penguin_id, &happiness);
      add_penguin(&festival.sectors[sector_id], penguin_id, happiness);
      add_penguin_festival(&festival, penguin_id, happiness, sector_id);
    }
    else if (string_equals(command, "HAPPINESS-BUFF")){
      int sector_id, penguin_id, happiness_increase;
      fscanf(input_file, "%d %d %d", &sector_id, &penguin_id, &happiness_increase);
      output = increase_happines(&festival.sectors[sector_id], penguin_id, happiness_increase);
    }
    else if (string_equals(command, "FESTIVAL")){
      output = festival_details(&festival);
    }
    else if (string_equals(command, "LEAVE")) {
      output = Leave(&festival);
    }
    
    // Continuar con más eventos...
    if (output) {
      fprintf(output_file, "%s", output); // Escribir todos los mensajes en el archivo
      free(output);  // Liberamos la memoria del mensaje
    }
  }


    
/* Cerramos los archivos */
  fclose(input_file);
  fclose(output_file);

/* Liberamos memoria */
  free_festival(&festival);
  
  return 0;
}
 