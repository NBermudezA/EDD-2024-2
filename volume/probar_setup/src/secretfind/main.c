#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>







#define TABLE_SIZE 10



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



typedef struct pinguin {
    int id;                // ID del pingüino
    char *code_tree;       // Puntero al CODE_TREE (arreglo de enteros)
    int code_tree_size;
    struct pinguin *next;
} pinguin;

pinguin * hash_table[TABLE_SIZE];


pinguin *create_pinguin(int id, const char *code_tree_str, int code_tree_size) {
    pinguin *p = malloc(sizeof(pinguin));
    p->id = id;
    p->code_tree_size = code_tree_size;
    p->code_tree = malloc((p->code_tree_size + 1) * sizeof(char)); // +1 para el terminador nulo
    strcpy(p->code_tree, code_tree_str); // Copiamos la cadena
    p->next = NULL;
    return p;
}



// Link para entender mejor el hash: https://www.youtube.com/watch?v=2Ti5yvumFTU
void init_hash_table() {
  for (int i = 0; i < TABLE_SIZE; i++) {
    hash_table[i] = NULL;
  }
  // parte vacia
}

void print_table() {
  printf("start\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (hash_table[i] == NULL) {
      printf("Posicion %d: NULL\n", i);
    } else {
      printf("Posicion %d: ", i);
      pinguin *p = hash_table[i];
      while (p != NULL) {
        printf("-> %d", p->id);
        p = p->next;
      }
      printf("\n");
    }
  }
  printf("end\n");
}

unsigned int hash_function_test(int id) {
    unsigned int hash_value = 0;
    // Usamos una combinación de operaciones aritméticas para crear variabilidad en el hash
    hash_value = id * 31;  // Multiplicamos por un número primo para mayor dispersión
    hash_value = (hash_value ^ (hash_value >> 5)) * 31; // XOR y desplazar bits para mayor aleatoriedad

    return hash_value % TABLE_SIZE;  // Tomamos el módulo para asegurarnos de que esté dentro del rango de la tabla
}
 // Arreglarla para que no entren todos al mismo lugar

bool hash_table_insert(pinguin *p) {
    if (p == NULL) return false;
    int index = hash_function_test(p->id);
    // Ahora manejamos las colisiones
    p->next = hash_table[index];
    hash_table[index] = p;
    return true;

}

pinguin * hash_table_search(int id) {
  int index = hash_function_test(id);
  // Ahora manejamos las colisiones
  pinguin *p = hash_table[index];
  while (p != NULL && p->id != id) {
    p = p->next;
  }
  return p;
} 

pinguin * hash_table_delete(int id) {
    int index = hash_function_test(id);
  // Ahora manejamos las colisiones
  pinguin *p = hash_table[index];
  pinguin *prev = NULL;
  while (p != NULL && p->id != id) {
    prev = p;
    p = p->next;
  }
  if (p == NULL) {
    return false;
  }

  if (prev == NULL) {
    // Estamos en la cabeza de la lista
    hash_table[index] = p->next;
  } else {
    prev->next = p->next;
  }
  return false;
}


int main(int argc, char **argv) {
  check_arguments(argc, argv);

  FILE *input_file = fopen(argv[1], "r");
  FILE *output_file = fopen(argv[2], "w");

  if (input_file == NULL || output_file == NULL) {
    printf("Error al abrir archivos\n");
    return 1;
  }

  printf("Empezando\n");
  init_hash_table();
  printf("Tabla Inicializada\n");
  print_table();

   // Crear tabla hash
  //HashTable *hash_table = create_hash_table(10);

  int ID;
  int CODE_TREE_large = 0;
  int *CODE_TREE = NULL;
  int K; // Cantidad de consultas
  char buffer[64]; // Buffer para leer "END"
  int result;

  // Leemos hasta encontrar "END"
  printf("Leyendo\n");
  while (true) {
    result = fscanf(input_file, "%s", buffer);
    // Verificamos si leímos la palabra "END"
    if (string_equals(buffer, "END")) {
      printf("END\n");
      break;
    } else {
      // Si no es "END", convertimos la entrada a entero
      ID = atoi(buffer);  // Convertimos buffer a entero para ID
      result = fscanf(input_file, "%d", &CODE_TREE_large);
      if (result != 1) {
        printf("Error reading CODE_TREE\n");
        return 1;
      }
      CODE_TREE = (int *)malloc(CODE_TREE_large * sizeof(int));
      printf("Code tree\n");
      for (int i = 0; i < CODE_TREE_large; i++) {
        result = fscanf(input_file, "%d", &CODE_TREE[i]);
      }
      printf("Creando pinguino\n");
      // Crear el nodo y agregarlo a la tabla hash
      pinguin *pinguin = create_pinguin(ID, CODE_TREE, CODE_TREE_large);
      printf("Metiendo a tabla\n");
      hash_table_insert(&pinguin);
      printf("Metido\n");
      // Imprimimos los valores leídos para debug
      printf("ID: %d, large tree: %d, CODE_TREE: [", ID, CODE_TREE_large);
      for (int i = 0; i < CODE_TREE_large; i++) {
        printf("%d", CODE_TREE[i]);
        if (i < CODE_TREE_large - 1) {
          printf(", ");
        }
      }
      printf("]\n");
      //free(CODE_TREE);
    }
  }
  free(CODE_TREE);


  // Leemos el número de consultas
  result = fscanf(input_file, "%d", &K);
  if (result != 1) {
    printf("Error reading number of events\n");
    return 1;
  }

  // pinguin p1 = {.id = 1, .code_tree = "11111", .code_tree_size = 5};
  // // pinguin p2 = {.id = 2, .code_tree = 10101, .code_tree_size = 5};
  // // pinguin p3 = {.id = 3, .code_tree = 010101, .code_tree_size = 6};
  // // pinguin p4 = {.id = 4, .code_tree = 00000, .code_tree_size = 5};
  // // pinguin p5 = {.id = 5, .code_tree = 11100, .code_tree_size = 5};
  // // pinguin p6 = {.id = 6, .code_tree = 1110101, .code_tree_size = 7};

  // hash_table_insert(&p1);
  // // hash_table_insert(&p2);
  // // hash_table_insert(&p3);
  // // hash_table_insert(&p4);
  // // hash_table_insert(&p5);
  // // hash_table_insert(&p6);

  print_table();


  pinguin *p = hash_table_search(1);
  if (p != NULL) {
      printf("Pinguin encontrado ID: %d - CODE_TREE: %d\n", p->id , p->code_tree);
  } else {
      printf("Pinguin no encontrado\n");
  }

  p = hash_table_search(2);
  if (p != NULL) {
      printf("Pinguin encontrado ID: %d - CODE_TREE: %d\n", p->id , p->code_tree);
  } else {
      printf("Pinguin no encontrado\n");
  }

  hash_table_delete(2); 
  p = hash_table_search(2);
  if (p != NULL) {
      printf("Pinguin encontrado ID: %d - CODE_TREE: %d\n", p->id , p->code_tree);
  } else {
      printf("Pinguin no encontrado\n");
  }

  print_table();

  // printf("Pinguin => %u\n", hash_function_test(1));
  // printf("Pinguin => %u\n", hash_function_test(2));
  // printf("Pinguin => %u\n", hash_function_test(3));
  // printf("Pinguin => %u\n", hash_function_test(4));
  // printf("Pinguin => %u\n", hash_function_test(5));
  // printf("Pinguin => %u\n", hash_function_test(62));


  return 0;
}