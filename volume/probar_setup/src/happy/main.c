#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>

typedef struct {
    int id;
    int happiness;
    int sector_id;
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
    Penguin *maxHeap;       // Heap que contiene el mayor pingüino
} Sector;


typedef struct {
    int num_sectors;      // Número de sectores en el festival
    Penguin *bigHeap;     // Puntero al heap que contiene todos los pingüinos
    int bigHeapSize;      // Tamaño del bigHeap
    Sector *sectors;      // Array dinámico de sectores
} Festival;


//Funciones extras
void init_festival(Festival *festival, int num_sectors, int *capacities);
void free_festival(Festival *festival);
void shift_up_min_heap(Penguin *heap, int index);
void shift_up_max_heap(Penguin *heap, int index);
void shift_down_min_heap(Penguin *heap, int index, int n); 
void shift_down_max_heap(Penguin *heap, int index, int n); 
void shift_down_big_heap(Penguin *bigHeap, int big_heap_index, int bigHeapSize);
void remove_penguin_from_heaps(Festival *festival, Sector *sector, int penguin_id, int happiness);



// Funciones de eventos parte 1
void add_penguin(Sector *sector, int penguin_id, int happiness);
void add_penguin_festival(Festival *festival, int penguin_id,int happiness, int sector_id);
char* increase_happines(Festival *festival, int sector_id, int penguin_id, int happiness_increase);
char* festival_details(Festival *festival);
char* Leave(Festival *festival);
char* Medians(Festival *festival);

#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool string_equals(const char *string1, char *string2) {
  return !strcmp(string1, string2);
}

void free_festival(Festival *festival) {
    for (int i = 0; i < festival->num_sectors; i++) {
        free(festival->sectors[i].minHeapMedian);
        free(festival->sectors[i].maxHeapMedian);
        free(festival->sectors[i].minHeap);
        free(festival->sectors[i].maxHeap);
    }
    free(festival->bigHeap);
    free(festival->sectors);
}

void init_festival(Festival *festival, int num_sectors, int *capacities) {
    festival->num_sectors = num_sectors;
    festival->sectors = malloc(num_sectors * sizeof(Sector));
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

        // Inicializamos todos los valores a cero
        memset(festival->sectors[i].minHeapMedian, 0, capacities[i] * sizeof(Penguin));
        memset(festival->sectors[i].maxHeapMedian, 0, capacities[i] * sizeof(Penguin));
        memset(festival->sectors[i].minHeap, 0, capacities[i] * sizeof(Penguin));
        memset(festival->sectors[i].maxHeap, 0, capacities[i] * sizeof(Penguin));

        capacidad_big_heap += capacities[i];
    }
    festival->bigHeap = malloc(sizeof(Penguin) * capacidad_big_heap); 
    festival->bigHeapSize = 0;
}

void print_heap(Penguin *heap, int size, const char *heapName, int sector_id) {
    printf("heap:%s - from: %d: ", heapName, sector_id);
    if (size == 0) {
        printf("heap:%s - from: %d: (empty)\n", heapName, sector_id);
        return;
    }
    for (int i = 0; i < size; i++) {
        if (string_equals(heapName, "Bigheap After")) {
            printf("(%d, %d, %d) ", heap[i].id, heap[i].happiness, sector_id);
        }
        else {
            printf("(%d, %d) ", heap[i].id, heap[i].happiness); 
        }
        
    }
    printf("\n");
    //Funcion para ir viendo como se ven los heaps
}


// Funciones de shift para mover los pinguinos a lo largo de los heaps
// Todas muy parecidas, todas las que son up son sacadas de las clases, las down las tuve que modificar un poco
void shift_up_min_heap(Penguin *heap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre
    int parentIndex = (index - 1) / 2;
    if (heap[parentIndex].happiness < heap[index].happiness || 
        (heap[parentIndex].happiness == heap[index].happiness && heap[parentIndex].id < heap[index].id)) {
        // Intercambiar el padre con el hijo
        Penguin temp = heap[parentIndex];
        heap[parentIndex] = heap[index];
        heap[index] = temp;
        shift_up_min_heap(heap, parentIndex);
    }
}

void shift_up_max_heap(Penguin *heap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre

    int parentIndex = (index - 1) / 2;

    if ((heap[parentIndex].happiness > heap[index].happiness || 
        (heap[parentIndex].happiness == heap[index].happiness && heap[parentIndex].id > heap[index].id)) && heap[index].happiness > 0) {
        Penguin temp = heap[parentIndex];
        heap[parentIndex] = heap[index];
        heap[index] = temp;
        shift_up_max_heap(heap, parentIndex);
    }
}

void shift_up_big_heap(Penguin *bigHeap, int index) {
    if (index == 0) return; // Si es el nodo raíz, no tiene padre
    int parentIndex = (index - 1) / 2;
    if (bigHeap[parentIndex].happiness > bigHeap[index].happiness || 
        (bigHeap[parentIndex].happiness == bigHeap[index].happiness && bigHeap[parentIndex].id > bigHeap[index].id)) {
        Penguin temp = bigHeap[parentIndex];
        bigHeap[parentIndex] = bigHeap[index];
        bigHeap[index] = temp;
        shift_up_big_heap(bigHeap, parentIndex);
    }
}

void shift_down_big_heap(Penguin *heap, int index, int n) {
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
        shift_down_big_heap(heap, smallest, n);
    }
}

void shift_down_min_heap(Penguin *heap, int index, int n) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

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

int calculate_median_happiness(Sector *sector) {
    //Funcion de calculo de mediana
    int total_penguins = sector->num_penguins;

    if (total_penguins % 2 == 1) {
        return sector->maxHeapMedian[0].happiness; // Mediana en un número impar
    } else {
        return (sector->maxHeapMedian[0].happiness + sector->minHeapMedian[0].happiness) / 2; // Mediana en un número par
    }
}

// Acá no cumplo con la complejidad, no tuve tiempo de buscar la forma correcta asique recorri los heaps para extraer las cosas
void remove_penguin_from_heaps(Festival *festival, Sector *sector, int penguin_id, int happiness) {
    // Lo sacamos del minheap
    sector->minHeap[0] = sector->minHeap[sector->num_penguins - 1];
    sector->num_penguins--;
    shift_down_min_heap(sector->minHeap, 0, sector->num_penguins);
    printf("\n\nsacado de minheap\n\n");

    // Lo sacamos del maxHeap
    int index = -1;
    for (int i = 0; i < sector->num_maxheap; i++) {
    if (sector->maxHeap[i].id == penguin_id && sector->maxHeap[i].happiness == happiness) {
        index = i;
        break;
    }
    } if (index != -1) {
        sector->maxHeap[index] = sector->maxHeap[sector->num_maxheap];
        sector->num_maxheap--; 
        shift_down_max_heap(sector->maxHeap, index, sector->num_maxheap);
        printf("\n\nsacado de maxheap\n\n");
    }


    // Lo sacamos del minHeapMedian
    index = -1;
    for (int i = 0; i <= sector->num_minheap; i++) {
        if (sector->minHeapMedian[i].id == penguin_id && sector->minHeapMedian[i].happiness == happiness) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        sector->minHeapMedian[index] = sector->minHeapMedian[sector->num_minheap];
        sector->num_minheap--;
        shift_down_min_heap(sector->minHeapMedian, index, sector->num_minheap);
        printf("\n\nsacado de medianMinheap\n\n");
    }

    // Lo sacamos del bigHeap
    index = -1;
    for (int i = 0; i <= festival->bigHeapSize; i++) {
        if (festival->bigHeap[i].id == penguin_id && festival->bigHeap[i].happiness == happiness) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        festival->bigHeap[index] = festival->bigHeap[festival->bigHeapSize - 1];
        festival->bigHeapSize--;
        shift_down_big_heap(festival->bigHeap, index, festival->bigHeapSize);
        printf("\n\nsacado de bigheap\n\n");
    }
    return;
}



// FUnciones de Eventos

//Falta arreglar el remove pinguin - los shifts down

//Acá se meten todos los pinguinos a un heap en donde estan todos, de todos los sectores
void add_penguin_festival(Festival *festival, int penguin_id, int happiness, int sector_id) {
   
    Penguin newPenguin = {penguin_id, happiness, sector_id};
    festival->bigHeap[festival->bigHeapSize] = newPenguin; 
    festival->bigHeapSize++; 
    shift_up_big_heap(festival->bigHeap, festival->bigHeapSize - 1);
    print_heap(festival->bigHeap, festival->bigHeapSize, "All After", 0);
}

//Acá se meten los pinguinos a sus respectivos heaps y sectores
//Hay unos cuantos prints para ir debuggeando, pero lo que importa se retorna al main
void add_penguin(Sector *sector, int penguin_id, int happiness) {
    if (sector->num_penguins >= sector->capacity) {
        printf("Sector %d lleno\n", sector->id);
        return;
    }
    int entro = 0; // Variable para que no entren a mas de un heap cuando no es necesario
    Penguin newPenguin = {penguin_id, happiness , sector->id};

    // Casos base, primeros pinguinos que entran
    //Siempre primero al max, y el segundo al min
    if (sector->num_maxheap == 0 && entro == 0) {
        sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
        shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
        sector->num_maxheap++;
        entro++;
    } else if (sector->num_penguins == 1 && entro == 0) {
        // Reemplazar el mínimo de minHeapMedian con el nuevo pingüino
        sector->minHeapMedian[0] = newPenguin;
        shift_up_min_heap(sector->minHeapMedian, sector->num_minheap);
        // Agregar el antiguo mínimo al maxHeapMedian
        sector->num_minheap++;
        entro++;
    }

    // Agregar al maxHeapMedian o minHeapMedian, este es el caso en que ambos tengan la misma cantidad de pingüinos
    if (sector->num_maxheap == sector->num_minheap && entro == 0) {
        // Si la felicidad del nuevo pingüino es menor que la del mínimo en minHeapMedian
        if (happiness < sector->minHeapMedian[0].happiness) {
            // Mover el mínimo de minHeapMedian al maxHeapMedian
            Penguin temp = sector->minHeapMedian[0]; 
            sector->minHeapMedian[0] = sector->minHeapMedian[sector->num_minheap - 1]; 
            sector->num_minheap--; 
            shift_down_min_heap(sector->minHeapMedian, 0, sector->num_minheap); 

            // Agregar el pingüino movido a maxHeapMedian
            sector->maxHeapMedian[sector->num_maxheap] = temp;
            shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap); 
            sector->num_maxheap++; 

            // Agregar el nuevo pingüino al minHeapMedian
            sector->minHeapMedian[sector->num_minheap] = newPenguin;
            shift_up_min_heap(sector->minHeapMedian, sector->num_minheap); 
            sector->num_minheap++; 
        } else {
            // Agregar a maxHeapMedian (si no entra en el caso anterior)
            sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
            shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
            sector->num_maxheap++;
        }
    }

    // Acá ya es para ingresar a los median de forma regular
    else if ((sector->num_maxheap > 0 && happiness >= sector->maxHeapMedian[0].happiness) && entro == 0) {
        if (sector->num_maxheap - sector->num_minheap <= 1){
            // Mover el menor del maxHeapMedian al minHeapMedian
            Penguin temp = sector->maxHeapMedian[0];
            sector->maxHeapMedian[0] = sector->maxHeapMedian[sector->num_maxheap - 1]; 
            sector->num_maxheap--;
            shift_down_max_heap(sector->maxHeapMedian, 0, sector->num_maxheap); 

            // Agregar el pingüino movido a minHeapMedian
            sector->minHeapMedian[sector->num_minheap] = temp;
            shift_up_min_heap(sector->minHeapMedian, sector->num_minheap); 
            sector->num_minheap++; 
        }
        // Agregar a maxHeapMedian
        sector->maxHeapMedian[sector->num_maxheap] = newPenguin;
        shift_up_max_heap(sector->maxHeapMedian, sector->num_maxheap);
        sector->num_maxheap++;
        entro++;

    } else {
        // Agregar a minHeapMedian
        if(sector->num_maxheap != 0 && entro == 0){
            sector->minHeapMedian[sector->num_minheap] = newPenguin;
            shift_up_min_heap(sector->minHeapMedian, sector->num_minheap);
            sector->num_minheap++;
            entro++;
        }
    }

    // Agregar al heap de todos los pingüinos, al min y max
    sector->minHeap[sector->num_penguins] = newPenguin;
    shift_up_max_heap(sector->minHeap, sector->num_penguins);
    sector->maxHeap[sector->num_penguins] = newPenguin;
    shift_up_min_heap(sector->maxHeap, sector->num_penguins);
    sector->num_penguins++;

    // Logs después de reajustar los heaps, esto son solo para debuggear y ver como van los heaps
    print_heap(sector->minHeapMedian, sector->num_penguins, "MinHeap Median After", sector->id);
    print_heap(sector->maxHeapMedian, sector->num_penguins, "MaxHeap Median After", sector->id);
    print_heap(sector->minHeap, sector->num_penguins, "MinHeap After", sector->id);
    print_heap(sector->maxHeap, sector->num_penguins, "MaxHeap After", sector->id);
    printf("Happiest: %d with %d\n", sector->maxHeap[0].id, sector->maxHeap[0].happiness);
    
}

// Falta cumplir con la complejidad
char* increase_happines(Festival *festival, int sector_id, int penguin_id, int happiness_increase) {
    // Funcion para evento de incrementar happiness
    int index = -1;
    // Buscar el pingüino en el heap de big
    for (int i = 0; i < festival->bigHeapSize; i++) {
        if (festival->bigHeap[i].id == penguin_id) {
            index = i;
            break;
        }
    }
    // Aumentar la felicidad en el heap de todos
    festival->bigHeap[index].happiness += happiness_increase;
    shift_up_big_heap(festival->bigHeap, index);

    index = -1;
    // Buscar el pingüino en el heap de min
    for (int i = 0; i < festival->sectors[sector_id].num_penguins; i++) {
        if (festival->sectors[sector_id].minHeap[i].id == penguin_id) {
            index = i;
            break;
        }
    }
    // Aumentar la felicidad en el heap de min
    festival->sectors[sector_id].minHeap[index].happiness += happiness_increase;
    shift_up_min_heap(festival->sectors[sector_id].minHeap, index);

    index = -1;
    // Buscar el pingüino en el heap de max
    for (int i = 0; i < festival->sectors[sector_id].num_penguins; i++) {
        if (festival->sectors[sector_id].maxHeap[i].id == penguin_id) {
            index = i;
            break;
        }
    }
    // Aumentar la felicidad en el heap de max
    festival->sectors[sector_id].maxHeap[index].happiness += happiness_increase;
    shift_up_min_heap(festival->sectors[sector_id].maxHeap, index);
    


    index = -1;
    for (int i = 0; i < festival->sectors[sector_id].num_minheap; i++) {
        if (festival->sectors[sector_id].minHeapMedian[i].id == penguin_id) {
            index = i;
            break;
        }
    }
    if (index != -1){
        // Lo aumentamos en minHeapMedian
        festival->sectors[sector_id].minHeapMedian[index].happiness += happiness_increase;
        shift_up_min_heap(festival->sectors[sector_id].minHeapMedian, index);
    }
    else{
        for (int i = 0; i < festival->sectors[sector_id].num_minheap; i++) {
            if (festival->sectors[sector_id].maxHeapMedian[i].id == penguin_id) {
                index = i;
                break;
            }
        }
        // Lo aumentamos en maxHeapMedian
        festival->sectors[sector_id].maxHeapMedian[index].happiness += happiness_increase;
        shift_up_max_heap(festival->sectors[sector_id].maxHeapMedian, index);
    }

    //Ahora ver si hay que mover un pinguino desde minMedian hasta el maxMeian
    int swap = 0;
    if (festival->sectors[sector_id].minHeapMedian[0].happiness >=  festival->sectors[sector_id].minHeapMedian[0].happiness) {
        if ((festival->sectors[sector_id].minHeapMedian[0].id > festival->sectors[sector_id].minHeapMedian[0].id) && (festival->sectors[sector_id].minHeapMedian[0].happiness ==  festival->sectors[sector_id].minHeapMedian[0].happiness)) {
            swap++;
        }
        else if (festival->sectors[sector_id].minHeapMedian[0].happiness >  festival->sectors[sector_id].maxHeapMedian[0].happiness) {
            swap++;
        }
    }
    // realizamos el swap en caso de ser necesario
    if (swap > 0) {
        Penguin minPenguin = festival->sectors[sector_id].minHeapMedian[0];
        Penguin maxPenguin = festival->sectors[sector_id].maxHeapMedian[0];
        festival->sectors[sector_id].minHeapMedian[0] = maxPenguin;
        festival->sectors[sector_id].maxHeapMedian[0] = minPenguin;
        shift_down_min_heap(festival->sectors[sector_id].minHeapMedian, 0, festival->sectors[sector_id].num_minheap);
        shift_down_max_heap(festival->sectors[sector_id].maxHeapMedian, 0 , festival->sectors[sector_id].num_maxheap);
        for (int i = 0; i < festival->sectors[sector_id].num_maxheap; i++) {
            shift_up_big_heap(festival->sectors[sector_id].maxHeapMedian, i);
        } // Acá era para que no quede el swap de primero, y que se mantenga la propiedad de heaps, pero no tuve tiempo de hacerlo bien
    }

    char *message = malloc(100);
    snprintf(message, 100, "Penguin %d happiness updated to %d\n", penguin_id, festival->sectors[sector_id].minHeap[index].happiness);
    return message;
}

char* festival_details(Festival *festival) {
    size_t message_size = 1000;
    char *message = malloc(message_size);

    size_t used = 0;// Este es unico mensaje que puede ser en verdad largo, por lo que la mejor opcion que encontre fue esta
                    // Todo se va uniendo al mensaje original, y el tamaño se va agrandando, y si en algun miunto supera el message_size, 
                    //simplemente este se agranda, esto para no usar mas memoria de la necesaria, y poder hacer el alloc con el tamaño siempre correcto
    used += snprintf(message + used, message_size - used, "Festival Stats\n");

    for (int i = 0; i < festival->num_sectors; i++) {
        Sector sector = festival->sectors[i];
        int total_penguins = sector.num_penguins;        
        int median_happiness = calculate_median_happiness(&sector);
        int happiest_penguin_id = sector.maxHeap[0].id;
        int saddest_penguin_id = sector.minHeap[0].id;

        char buffer[100];
        int needed = snprintf(buffer, sizeof(buffer), "    Place %d has %d penguin[s]\n", sector.id, total_penguins);
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
    int min_median = festival->sectors[0].maxHeap[0].happiness; //asi tomamos un valor del cual nos aseguramos que es mayor o igual a la mediana
    int sector_index = 0; 

    // Primero encontramos el sector con la menor mediana
    for (int i = 0; i < festival->num_sectors; i++) {
        int median_happiness = calculate_median_happiness(&festival->sectors[i]);
        if (median_happiness < min_median) {
            min_median = median_happiness;
            sector_index = i;
        }
    }

    Sector *selected_sector = &festival->sectors[sector_index];
    // Encontrar el pinguino que tiene que salir es facil por el minheap
    printf("deberia salir %d del sector %d con %d\n", selected_sector->minHeap[0].id, sector_index, selected_sector->minHeap[0].happiness);
    char *message = malloc(100);
    snprintf(message, 100, "Penguin %d leaving sector %d\n", selected_sector->minHeap[0].id, sector_index);
    //Acá lo removemos de todos lados
    remove_penguin_from_heaps(festival, selected_sector, selected_sector->minHeap[0].id, selected_sector->minHeap[0].happiness);
    return message;
}

// Falta cumplir con la complejidad
char* Medians(Festival *festival) {
    // Funcion para calcular la mejor y peor mediana
    //Si solo hay uno es peor y mejor, asique inicialmente es 'el
    int temp_median = calculate_median_happiness(&festival->sectors[0]);
    int lower_median = temp_median;
    int best_median = temp_median;
    int lower_median_id = festival->sectors[0].id; 
    int best_median_id = festival->sectors[0].id;

    for (int i = 0; i < festival->num_sectors; i++) {
        int temp_median = calculate_median_happiness(&festival->sectors[i]);
        // Buscamos al con menor mediana
        if (lower_median >= temp_median) {
            // Caso de empate de mediana, el con mayor id se mantiene como menor
            if (lower_median == temp_median && lower_median_id < i){
                lower_median = temp_median;
                lower_median_id = i;
            }
            else {
                lower_median = temp_median;
                lower_median_id = i;
            }
        }

         // Buscamos al con mayor mediana
        if (best_median <= temp_median) {
            // Caso de empate de mediana, el con mayor id se mantiene como mayor
            if (best_median == temp_median && best_median_id > i){
                best_median = temp_median;
                best_median_id = i;
            }
            else {
                best_median = temp_median;
                best_median_id = i;
            }
        }
    }

    printf("Medians resume:\n   %d v/s %d\n %d - %d\n", best_median_id, lower_median_id, best_median, lower_median);
    // Ahora construimos el mensaje para el output
    char *message = malloc(100);
    snprintf(message, 100, "Medians resume:\n    %d v/s %d\n    %d - %d\n", best_median_id, lower_median_id, best_median, lower_median);
    return message;
}

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    else if (string_equals(command, "HAPPINESS_BUFF")){
      int sector_id, penguin_id, happiness_increase;
      fscanf(input_file, "%d %d %d", &sector_id, &penguin_id, &happiness_increase);
      output = increase_happines(&festival, sector_id, penguin_id, happiness_increase);
    }
    else if (string_equals(command, "FESTIVAL")){
      output = festival_details(&festival);
    }
    else if (string_equals(command, "LEAVE")) {
      output = Leave(&festival);
    }
    else if (string_equals(command, "MEDIANS")) {
      output = Medians(&festival);
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
 