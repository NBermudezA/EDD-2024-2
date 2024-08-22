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

// Funciones de memoria y extracción
void free_pinguin(Pinguin *pinguin);
void free_store(Store *store);
void extract_puffle_from_pinguin(Pinguin *pinguin, int puffle_index);

// Funciones de la tienda (retornan mensajes para guardar en archivo)
char* add_pinguin_to_store(Store *store, Pinguin pinguin);
char* remove_pinguin_from_store(Store *store, int pinguin_id);
char* info_store(Store *store);
char* count_color(Store *store, char *color);
char* runaway(Store *store, int pinguin_id, int puffle_id);
char* buy_puffle(Store *store, int pinguin_id, int puffle_id, const char *color, int P);
char* trading_puffles(Store *store, int pinguin_id1, int pinguin_id2, int puffle_id1, int puffle_id2);
char* steal_puffle(Store *store, int pinguin_id1, int pinguin_id2, char *color, int n_limit, int P);
char* give_puffle(Store *store, int pinguin_id1, int pinguin_id2, int start, int end, int P);

#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// La copie del main para usarla en las funciones
static bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}

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
            free_pinguin(&store->pinguins[i]); //Memoria de cada pinguin (puffles)
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

char* add_pinguin_to_store(Store *store, Pinguin pinguin) {
    if (store->num_pinguins >= store->K) {
        free_pinguin(&pinguin); // Liberar memoria si no se puede añadir el pingüino, por max de store
        char *message = malloc(100);
        snprintf(message, 100, "Entry denied due to maximum capacity\n");
        return message;                                                 
    }
    store->pinguins = realloc(store->pinguins, (store->num_pinguins + 1) * sizeof(Pinguin));
    store->pinguins[store->num_pinguins++] = pinguin;
    
    char *message = malloc(100);
    // El comando snprintf es para escribir una cadena en un buffer, en stack overflow me salio que era lo mejor para retornar los mensaje
    // y asi en el main los introduzco al archivo output
    // Ese comando lo vamos a usar en casi todas las funciones, ya que la mayoria retorna o trabaja con cadenas de texto, que son los mensajes que se van a ver en el output 
    snprintf(message, 100, "Penguin %d has entered with %d puffles\n", pinguin.id, pinguin.num_puffles); // Creamos mensaje
    return message;
}

char* remove_pinguin_from_store(Store *store, int pinguin_id) {
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            char *message = malloc(100);
            snprintf(message, 100, "Leaving %d with %d puffles\n", pinguin_id, store->pinguins[i].num_puffles);
            free_pinguin(&store->pinguins[i]);
            for (int j = i; j < store->num_pinguins - 1; j++) {
                store->pinguins[j] = store->pinguins[j + 1];
            }
            store->num_pinguins--;
            store->pinguins = realloc(store->pinguins, store->num_pinguins * sizeof(Pinguin));
            return message;
        }
    }
}

char* info_store(Store *store) {
    int count_puffles = 0;
    size_t message_size = 1000;
    char *message = malloc(message_size);

    size_t used = 0; // Este es unico mensaje que puede ser en verdad largo, por lo que la mejor opcion que encontre fue esta
                    // Todo se va uniendo al mensaje original, y el tamaño se va agrandando, y si en algun miunto supera el message_size, 
                    //simplemente este se agranda, esto para no usar mas memoria de la necesaria, y poder hacer el alloc con el tamaño siempre correcto
    used += snprintf(message + used, message_size - used, "STATUS:\n    Total penguins: %d\n", store->num_pinguins);

    for (int i = 0; i < store->num_pinguins; i++) {
        char buffer[100];
        int needed = snprintf(buffer, sizeof(buffer), "    Penguin %d:\n", store->pinguins[i].id);

        if (used + needed >= message_size) {
            message_size *= 2;  // Duplicamos el tamaño del buffer si es necesario
            message = realloc(message, message_size);
        }

        strcat(message + used, buffer);
        used += needed;

        for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
            needed = snprintf(buffer, sizeof(buffer), "        Puffle %d\n", store->pinguins[i].puffles[j].id);
            if (used + needed >= message_size) {
                message_size *= 2;
                message = realloc(message, message_size);
            }
            strcat(message + used, buffer);
            used += needed;
            count_puffles++;
        }

        if (store->pinguins[i].num_puffles == 0) {
            const char *no_puffles_msg = "        Does not have puffles\n";
            needed = strlen(no_puffles_msg); // calculamos la longitud de los caracteres
            if (used + needed >= message_size) {
                message_size *= 2;
                message = realloc(message, message_size);
            }

            strcat(message + used, no_puffles_msg);
            used += needed;
        }
    }

    char buffer[100];
    int needed = snprintf(buffer, sizeof(buffer), "    Total puffles: %d\n", count_puffles);

    if (used + needed >= message_size) {
        message_size *= 2;
        message = realloc(message, message_size);
    }

    strcat(message + used, buffer);

    return message;
}

char* count_color(Store *store, char *color) {
    int count = 0;
    for (int i = 0; i < store->num_pinguins; i++) {
        for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
            if (string_equals(store->pinguins[i].puffles[j].color, color)) {
                count++;
            }
        }
    }
    char *message = malloc(100);
    snprintf(message, 100, "Total amount %s: %d\n", color, count);
    return message;
}

// Parte 2

char* runaway(Store *store, int pinguin_id, int puffle_id) {
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id) {
                    extract_puffle_from_pinguin(&store->pinguins[i], j);
                    char *message = malloc(100);
                    snprintf(message, 100, "%d ran away from %d\n", puffle_id, pinguin_id);
                    return message;
                }
            }
        }
    }
}

char* buy_puffle(Store *store, int pinguin_id, int puffle_id, const char *color, int P) {
    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id) {
            if (store->pinguins[i].num_puffles < P) {
                store->pinguins[i].puffles = realloc(store->pinguins[i].puffles, (store->pinguins[i].num_puffles + 1) * sizeof(Puffle));

                Puffle puffle;
                puffle.id = puffle_id;
                strcpy(puffle.color, color);
                store->pinguins[i].puffles[store->pinguins[i].num_puffles++] = puffle;

                char *message = malloc(100);
                snprintf(message, 100, "Penguin %d bought %d\n", pinguin_id, puffle_id);
                return message;
            } else {
                char *message = malloc(100);
                snprintf(message, 100, "Purchase denied\n");
                return message;
            }
        }
    }
}

char* trading_puffles(Store *store, int pinguin_id1, int pinguin_id2, int puffle_id1, int puffle_id2) {
    Puffle puffle1;
    Puffle puffle2;
    int indexPinguin1 = -1;
    int indexPuffle1 = -1;
    int indexPinguin2 = -1;
    int indexPuffle2 = -1;

    for (int i = 0; i < store->num_pinguins; i++) { // Se buscan los pinguins
        if (store->pinguins[i].id == pinguin_id1) {
            indexPinguin1 = i;
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id1) { // Se buscan puffles
                    puffle1 = store->pinguins[i].puffles[j];
                    indexPuffle1 = j;
                    break;
                }
            }
        } else if (store->pinguins[i].id == pinguin_id2) {
            indexPinguin2 = i;
            for (int j = 0; j < store->pinguins[i].num_puffles; j++) {
                if (store->pinguins[i].puffles[j].id == puffle_id2) {// Se buscan puffles
                    puffle2 = store->pinguins[i].puffles[j];
                    indexPuffle2 = j;
                    break;
                }
            }
        }
    }

    if (indexPinguin1 != -1 && indexPuffle1 != -1 && indexPinguin2 != -1 && indexPuffle2 != -1) { // Se ve que esten todos los puffles y pinguinos
        store->pinguins[indexPinguin1].puffles[indexPuffle1] = puffle2;
        store->pinguins[indexPinguin2].puffles[indexPuffle2] = puffle1;
        char *message = malloc(100);
        snprintf(message, 100, "Exchange between %d and %d\n", pinguin_id1, pinguin_id2);
        return message;
    }
}

char* steal_puffle(Store *store, int pinguin_id1, int pinguin_id2, char *color, int n_limit, int P) {
    int index_id1 = -1;
    int index_id2 = -1;
    int steal_count = 0;

    for (int i = 0; i < store->num_pinguins; i++) { // Se buscan los pinguins
        if (store->pinguins[i].id == pinguin_id1) {
            index_id1 = i;
        } else if (store->pinguins[i].id == pinguin_id2) {
            index_id2 = i;
        }
    }

    for (int i = 0; i < store->pinguins[index_id2].num_puffles && steal_count < n_limit; i++) {
        if (string_equals(store->pinguins[index_id2].puffles[i].color, color)) {
            if (store->pinguins[index_id1].num_puffles < P) {
                store->pinguins[index_id1].puffles = realloc(store->pinguins[index_id1].puffles,
                                                            (store->pinguins[index_id1].num_puffles + 1) * sizeof(Puffle));
                store->pinguins[index_id1].puffles[store->pinguins[index_id1].num_puffles++] = store->pinguins[index_id2].puffles[i];
                extract_puffle_from_pinguin(&store->pinguins[index_id2], i);
                steal_count++;
                i--;
            }
        }
    }

    if (steal_count >= 0) { // Esto siempre se muestra, aunque sean 0 robados
        char *message = malloc(100);
        snprintf(message, 100, "Steal %d from %d\n    Total stolen puffles: %d\n", pinguin_id1, pinguin_id2, steal_count);
        return message;
    }
}

char* give_puffle(Store *store, int pinguin_id1, int pinguin_id2, int start, int end, int P) {
    int index_id1 = -1;
    int index_id2 = -1;

    for (int i = 0; i < store->num_pinguins; i++) {
        if (store->pinguins[i].id == pinguin_id1) {
            index_id1 = i;
        } else if (store->pinguins[i].id == pinguin_id2) {
            index_id2 = i;
        }
    }
    int given_puffles = 0;
    int num_tot_gift = end - start; // Para ver cuantos hay que entregar

    if (store->pinguins[index_id2].num_puffles + num_tot_gift < P) {
        for (int i = start; i <= end; i++) {
            store->pinguins[index_id2].puffles = realloc(store->pinguins[index_id2].puffles,
                                                        (store->pinguins[index_id2].num_puffles + 1) * sizeof(Puffle));
            store->pinguins[index_id2].puffles[store->pinguins[index_id2].num_puffles++] = store->pinguins[index_id1].puffles[i];
            extract_puffle_from_pinguin(&store->pinguins[index_id1], i);
            given_puffles++;
            end--;
            i--;
            
        }
    }
    if (given_puffles > 0) { // Mensaje cuando se envia alguno
        char *message = malloc(100);
        snprintf(message, 100, "Gift from %d to %d\n    Total given away puffles: %d\n", pinguin_id1, pinguin_id2, given_puffles);
        return message;
    }
    else { // Si no se pueden mandar los gifts
        char *message = malloc(100);
        snprintf(message, 100, "Gift denied: %d would be full\n", pinguin_id2);
        return message;
    }
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
  Store store = {K, 0, malloc(K * sizeof(Pinguin))};
  
  /* Eventos */
  char command[32];
  for (int i = 0; i < E; i++) {
    fscanf(input_file, "%s", command);
    char *output = NULL;

    if (string_equals(command, "ENTER")) {
      int id;
      int N;
      fscanf(input_file, "%d %d", &id, &N);
      Pinguin pinguin = {id, N, malloc(N * sizeof(Puffle))};

      for (int j = 0; j < N; j++) {
        int puffle_id;
        char color[32];
        fscanf(input_file, "%d %s", &puffle_id, color);
        pinguin.puffles[j].id = puffle_id;
        strcpy(pinguin.puffles[j].color, color); // Nose porque aveces no me dejaba hacer el string_equals
      }                                          // en los colores, por lo que esto me salio que use en stack overflow
      output = add_pinguin_to_store(&store, pinguin);
    } else if (string_equals(command, "LEAVE")) {
      int id;
      fscanf(input_file, "%d", &id);
      output = remove_pinguin_from_store(&store, id);
    } else if (string_equals(command, "STATUS")) { 
      output = info_store(&store); 
    } else if (string_equals(command, "COLOR-AMOUNT")) {
      char color[32];
      fscanf(input_file, "%s", color);
      output = count_color(&store, color);
    } else if (string_equals(command, "BUY-PUFFLE")) {
      int pinguin_id;
      int puffle_id;
      char color[32];
      fscanf(input_file, "%d %d %s", &pinguin_id, &puffle_id, color);
      output = buy_puffle(&store, pinguin_id, puffle_id, color, P);
    } else if (string_equals(command, "RUNAWAY-PUFFLE")) {
      int pinguin_id;
      int puffle_id;
      fscanf(input_file, "%d %d", &pinguin_id, &puffle_id);
      output = runaway(&store, pinguin_id, puffle_id);
    } else if (string_equals(command, "TRADE-PUFFLE")) {
      int pinguin_id1;
      int puffle_id1;
      int pinguin_id2;
      int puffle_id2;
      fscanf(input_file, "%d %d %d %d", &pinguin_id1, &pinguin_id2, &puffle_id1, &puffle_id2);
      output = trading_puffles(&store, pinguin_id1, pinguin_id2, puffle_id1, puffle_id2);
    } else if (string_equals(command, "STEAL-PUFFLE")) {
      int pinguin_id1;
      int pinguin_id2;
      char color[32];
      int n_limit;
      fscanf(input_file, "%d %d %s %d", &pinguin_id1, &pinguin_id2, color, &n_limit);
      output = steal_puffle(&store, pinguin_id1, pinguin_id2, color, n_limit, P);
    } else if (string_equals(command, "GIVE-PUFFLES")) {
      int pinguin_id1;
      int pinguin_id2;
      int start;
      int end; 
      fscanf(input_file, "%d %d %d %d", &pinguin_id1, &pinguin_id2, &start, &end);
      output = give_puffle(&store, pinguin_id1, pinguin_id2, start, end, P);
    }

    if (output) {
      fprintf(output_file, "%s", output); // Escribir todos los mensajes en el archivo
      free(output);  // Liberamos la memoria del mensaje
    }
  }
  
  /* Cerramos los archivos */
  fclose(input_file);
  fclose(output_file);

  /* Liberamos memoria */
  free_store(&store); // Solo falta store

  return 0;
}
