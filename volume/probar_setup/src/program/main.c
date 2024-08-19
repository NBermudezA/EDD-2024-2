#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
    int id;
    char color[30]; 
} Puffle;

typedef struct {
    int id;
    int num_puffles;
    Puffle *puffles;
} Pinguin;

typedef struct {
    int K;
    int num_pinguins;
    Pinguin *pinguins;
} Store;

// Funciones de memoria y extraccion
void free_pinguin(Pinguin *pinguin);
void free_store(Store *store);
void extract_puffle_from_pinguin(Pinguin *pinguin, int puffle_index);

// Funciones de la tienda
void add_pinguin_to_store(Store *store, Pinguin pinguin);
void remove_pinguin_from_store(Store *store, int pinguin_id);
void info_store(Store *store);
void count_color(Store *store, char *color);
void runaway(Store *store, int pinguin_id, int puffle_id);
void buy_puffle(Store *store, int pinguin_id, int puffle_id, const char *color, int P);
void trading_puffles(Store *store, int pinguin_id1, int pinguin_id2, int puffle_id1, int puffle_id2);
void steal_puffle(Store *store, int pinguin_id1, int pinguin_id2, char *color, int n_limit);
void give_puffle(Store *store, int pinguin_id1, int pinguin_id2, int start, int end);

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// Función para liberar un pingüino
void free_pinguin(Pinguin *pinguin) {
    if (pinguin->puffles != NULL) {
        free(pinguin->puffles); // Liberar memoria de los puffles
        pinguin->puffles = NULL;
    }
}

// Función para liberar toda la memoria del Store
void free_store(Store *store) {
    if (store->pinguins != NULL) {
        for (int i = 0; i < store->num_pinguins; i++) {
            free_pinguin(&store->pinguins[i]);
        }
        free(store->pinguins); // Liberar memoria de los pingüinos
        store->pinguins = NULL;
    }
}

// Función para extraer un puffle de un pingüino
void extract_puffle_from_pinguin(Pinguin *pinguin, int puffle_index) {
    for (int j = puffle_index; j < pinguin->num_puffles - 1; j++) {
        pinguin->puffles[j] = pinguin->puffles[j + 1];
    }
    pinguin->num_puffles--;
    pinguin->puffles = realloc(pinguin->puffles, pinguin->num_puffles * sizeof(Puffle));
}

// Parte 1

void add_pinguin_to_store(Store *store, Pinguin pinguin) {
    if (store->num_pinguins >= store->K) {
        printf("Entry denied due to maximum capacity\n");
        return;
    }
    store->pinguins[store->num_pinguins++] = pinguin;
    printf("Penguin %d has entered with %d puffles\n", pinguin.id, pinguin.num_puffles);
}

void remove_pinguin_from_store(Store *store, int pinguin_id) {
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            printf("Leaving %d with %d puffles\n", pinguin_id, store->pinguins[i].num_puffles);
            free_pinguin(&store->pinguins[i]);
            for (int j = i; j < store->num_pinguins - 1; j++) {
                store->pinguins[j] = store->pinguins[j + 1];
            }
            store->num_pinguins--;
            store->pinguins = realloc(store->pinguins, store->num_pinguins * sizeof(Pinguin));
            return;
        }
    }
}

void info_store(Store *store){
    int count_puffles = 0;
    printf("STATUS:\n");
    printf("    Total penguins: %d\n", store->num_pinguins);
    for (int i = 0; i < store->num_pinguins; i++) {
        printf("    Penguin %d:\n", store->pinguins[i].id);
        for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
            printf("        Puffle %d\n", store->pinguins[i].puffles[j].id);
            count_puffles++;
        }
        if (store->pinguins[i].num_puffles == 0) {
            printf("        Does not have puffles\n");
        }
    }
    printf("    Total puffles: %d\n", count_puffles);
}

void count_color(Store *store, char *color){
    int count = 0;
    for (int i = 0; i < store->num_pinguins; i++) {
        for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
            if (strcmp(store->pinguins[i].puffles[j].color, color) == 0) {
                count++;
            }
        }
    }
    printf("Total amount %s: %d\n", color, count);
}

// Parte 2

void runaway(Store *store, int pinguin_id, int puffle_id){
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id) {
                    printf("%d run away from %d\n", puffle_id, pinguin_id);
                    extract_puffle_from_pinguin(&store->pinguins[i], j);
                    return;
                }
            }
        }
    }
}

void buy_puffle(Store *store, int pinguin_id, int puffle_id, const char *color, int P) {
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            if (store->pinguins[i].num_puffles < P) {
                // Realoca memoria para el nuevo puffle
                store->pinguins[i].puffles = realloc(store->pinguins[i].puffles, (store->pinguins[i].num_puffles + 1) * sizeof(Puffle));
                if (store->pinguins[i].puffles == NULL) {
                    printf("Memory allocation failed\n");
                    return;
                }

                // Asigna el nuevo puffle
                Puffle puffle;
                puffle.id = puffle_id;
                strcpy(puffle.color, color);
                store->pinguins[i].puffles[store->pinguins[i].num_puffles++] = puffle;

                // Imprime el resultado de la compra
                printf("Penguin %d bought puffle %d\n", pinguin_id, puffle_id);
            } else {
                printf("Purchase denied: Maximum number of puffles reached\n");
            }
            return;
        }
    }
    printf("Penguin with ID %d not found\n", pinguin_id);
}

void trading_puffles(Store *store, int pinguin_id1, int pinguin_id2, int puffle_id1, int puffle_id2){
    Puffle puffle1;
    Puffle puffle2;
    int indexPinguin1 = -1;
    int indexPuffle1 = -1;
    int indexPinguin2 = -1;
    int indexPuffle2 = -1;

    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id1) {
            indexPinguin1 = i;
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id1) {
                    puffle1 = store->pinguins[i].puffles[j];
                    indexPuffle1 = j;
                    break;
                }
            }
        }
        else if (store->pinguins[i].id == pinguin_id2) {
            indexPinguin2 = i;
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id2) {
                    puffle2 = store->pinguins[i].puffles[j];
                    indexPuffle2 = j;
                    break;
                }
            }
        }
    }

    if (indexPinguin1 != -1 && indexPuffle1 != -1 && indexPinguin2 != -1 && indexPuffle2 != -1) {
        store->pinguins[indexPinguin1].puffles[indexPuffle1] = puffle2;
        store->pinguins[indexPinguin2].puffles[indexPuffle2] = puffle1;
        printf("Exchange between %d and %d\n", pinguin_id1, pinguin_id2);
    }
}

void steal_puffle(Store *store, int pinguin_id1, int pinguin_id2, char *color, int n_limit){
    int index_id1 = -1;
    int index_id2 = -1;
    int steal_count = 0;

    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id1) {
            index_id1 = i;
        }
        else if (store->pinguins[i].id == pinguin_id2) {
            index_id2 = i;
        }
    }

    for (int i = 0; i < store->pinguins[index_id2].num_puffles && steal_count < n_limit; i++) {
        if (strcmp(store->pinguins[index_id2].puffles[i].color, color) == 0) {
            store->pinguins[index_id1].puffles = realloc(store->pinguins[index_id1].puffles, 
                                                         (store->pinguins[index_id1].num_puffles + 1) * sizeof(Puffle));
            store->pinguins[index_id1].puffles[store->pinguins[index_id1].num_puffles++] = store->pinguins[index_id2].puffles[i];
            extract_puffle_from_pinguin(&store->pinguins[index_id2], i);
            steal_count++;
            i--;
        }
    }

    if (steal_count > 0) {
        printf("Steal %d from %d\n", pinguin_id1, pinguin_id2);
        printf("    Total stolen puffles: %d\n", steal_count);
    }
}

void give_puffle(Store *store, int pinguin_id1, int pinguin_id2, int start, int end){
    printf("%d %d %d %d\n", pinguin_id1, pinguin_id2, start, end);
    printf("Starting\n");

    if (store->pinguins[pinguin_id1].num_puffles < end || start < 0 || end < start) {
        printf("Error: Invalid start or end indices for puffle transfer.\n"); // No deberia pasar
        return;
    }

    for (int i = start; i <= end; i++) {
        store->pinguins[pinguin_id2].puffles = realloc(store->pinguins[pinguin_id2].puffles, 
                                                       (store->pinguins[pinguin_id2].num_puffles + 1) * sizeof(Puffle));
        store->pinguins[pinguin_id2].puffles[store->pinguins[pinguin_id2].num_puffles++] = store->pinguins[pinguin_id1].puffles[i];
        extract_puffle_from_pinguin(&store->pinguins[pinguin_id1], i);
        i--;  
    }

    printf("Done\n");
}


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

  int K; // Capacidad de la tienda
  int P; // Número máximo de puffles
  int E; // Número de eventos (No considera los multiples inputs del evento ENTER)

  int result;
  result = fscanf(input_file, "%d", &K);
  if (result != 1) {
    printf("Error reading store capacity");
    return 1;
  }
  result = fscanf(input_file, "%d", &P);
  if (result != 1) {
    printf("Error reading maximum number of puffles");
    return 1;
  }
  result = fscanf(input_file, "%d", &E);
  if (result != 1) {
    printf("Error reading number of events");
    return 1;
  }


  /* Construcción de las estructuras */
    /* COMPLETAR */

  Store store = {K, 0, malloc(K * sizeof(Pinguin))};
  
  
  /* Eventos */
  char command[32];
  for (int i = 0; i < E; i++)
  {
    fscanf(input_file, "%s", command);
    if (string_equals(command, "ENTER")){
      int id;
      int N;
      fscanf(input_file, "%d %d", &id, &N);
      Pinguin pinguin = {id, N, malloc(N * sizeof(Puffle))};
     
      for (int j = 0; j < N; j++)
      {
        int puffle_id;
        char color[32];
        fscanf(input_file, "%d %s", &puffle_id, color);
        pinguin.puffles[j].id = puffle_id;
        strcpy(pinguin.puffles[j].color, color);
        
      }
      add_pinguin_to_store(&store, pinguin);
    } else if (string_equals(command, "LEAVE")) {
            int id;
            fscanf(input_file, "%d", &id);
            remove_pinguin_from_store(&store, id);
    } else if (string_equals(command, "STATUS")) { 
      info_store(&store); 
    } else if (string_equals(command, "COLOR-AMOUNT")) {
      char color[32];
      fscanf(input_file, "%s", color);
      count_color(&store, color);
    } else if (string_equals(command, "BUY-PUFFLE")){
      int pinguin_id;
      int puffle_id;
      char color[32];
      fscanf(input_file, "%d %d %s", &pinguin_id, &puffle_id, color);
      buy_puffle(&store, pinguin_id, puffle_id, color, P);
    } else if (string_equals(command, "RUNAWAY-PUFFLE")) {
      int pinguin_id;
      int puffle_id;
      fscanf(input_file, "%d %d", &pinguin_id, &puffle_id);
      runaway(&store, pinguin_id, puffle_id);
      
    } else if (string_equals(command, "TRADE-PUFFLE")) {
      int pinguin_id1;
      int puffle_id1;
      int pinguin_id2;
      int puffle_id2;
      fscanf(input_file, "%d %d %d %d", &pinguin_id1, &pinguin_id2, &puffle_id1, &puffle_id2);
      trading_puffles(&store, pinguin_id1, pinguin_id2, puffle_id1, puffle_id2);
    } else if (string_equals(command, "STEAL-PUFFLE")) {
      int pinguin_id1;
      int pinguin_id2;
      char color[32];
      int n_limit;
      fscanf(input_file, "%d %d %s %d", &pinguin_id1, &pinguin_id2, color, &n_limit);
      steal_puffle(&store, pinguin_id1, pinguin_id2, color, n_limit);
    } else if (string_equals(command, "GIVE-PUFFLE")) {
      int pinguin_id1;
      int pinguin_id2;
      int start;
      int end; 
      fscanf(input_file, "%d %d %d %d", &pinguin_id1, &pinguin_id2, &start, &end);
      fprintf(output_file, "give puffless en process\n");
      //give_puffle(&store, pinguin_id1, pinguin_id2, start, end);
    }
  }
  

/* Cerramos los archivos */
  fclose(input_file);
  fclose(output_file);

/* Liberamos memoria */
    /* COMPLETAR */
    for (int i = 0; i < store.num_pinguins; i++) {
        free(store.pinguins[i].puffles); // Memoria de los puffles
    }
    free(store.pinguins); // Memoria de los pingüinos


  return 0;
}