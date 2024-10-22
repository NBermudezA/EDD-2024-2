#ifndef STRUCT_H
#define STRUCT_H
#include <stdio.h>

typedef struct Penguin {
    int id;
    int x;
    int y;
    int age;
    int weight;
} Penguin;

typedef struct TreeNode {
    Penguin penguin;
    int height;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Funciones para manejar el árbol
TreeNode* insert(TreeNode* root, Penguin penguin);
void print_tree(TreeNode* root);
int height(TreeNode* node);
void free_tree(TreeNode* root);

// Funciones de eventos
char* search_by_age(TreeNode* root, int age);
void search_by_age_helper(TreeNode* root, int age, char** message, int* first);
char* search_by_weight_range(TreeNode* root, int min, int max);
void search_by_weight_range_helper(TreeNode* root, int min, int max, char** message, int* first);

#endif // STRUCT_H
// struct.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

// Función para obtener la altura de un nodo
int height(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

// Función para crear un nuevo nodo
TreeNode* create_node(Penguin penguin) {
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    new_node->penguin = penguin;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->height = 1; // La altura inicial es 1
    return new_node;
}

// Función para calcular el factor de balance de un nodo
int balance_factor(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}


// Rotaciones para balancear el árbol AVL
// Rotación LL
TreeNode* ll_rotation(TreeNode* node) {
    TreeNode* new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));
    new_root->height = 1 + (height(new_root->left) > height(new_root->right) ? height(new_root->left) : height(new_root->right));
    return new_root;
}

// Rotación RR
TreeNode* rr_rotation(TreeNode* node) {
    TreeNode* new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));
    new_root->height = 1 + (height(new_root->left) > height(new_root->right) ? height(new_root->left) : height(new_root->right));
    return new_root;
}

// Rotación LR
TreeNode* lr_rotation(TreeNode* node) {
    node->left = rr_rotation(node->left);
    return ll_rotation(node);
}

// Rotación RL
TreeNode* rl_rotation(TreeNode* node) {
    node->right = ll_rotation(node->right);
    return rr_rotation(node);
}

// Insertar un pingüino en el árbol AVL
TreeNode* insert(TreeNode* node, Penguin penguin) {
    if (node == NULL) {
        return create_node(penguin);
    }

    if (penguin.age < node->penguin.age) {
        node->left = insert(node->left, penguin);
    } else if (penguin.age > node->penguin.age) {
        node->right = insert(node->right, penguin);
    } else { // Si la edad es igual, ordenar por ID
        if (penguin.id < node->penguin.id) {
            node->left = insert(node->left, penguin);
        } else {
            node->right = insert(node->right, penguin);
        }
    }

    // Actualizar la altura del nodo
    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));

    // Balancear el árbol
    int balance = balance_factor(node);

    // Casos de rotación
    if (balance > 1 && balance_factor(node->left) >= 0) {
        return ll_rotation(node);
    }
    if (balance > 1 && balance_factor(node->left) < 0) {
        return lr_rotation(node);
    }
    if (balance < -1 && balance_factor(node->right) <= 0) {
        return rr_rotation(node);
    }
    if (balance < -1 && balance_factor(node->right) > 0) {
        return rl_rotation(node);
    }

    return node;
}

// Función para imprimir el árbol con información de balance (en nivel de orden)
void print_tree(TreeNode* root) { // Esta despues se saca
    if (root == NULL) {
        printf("Empty tree\n");
        return;
    }

    TreeNode* queue[100]; // Cola simple con un tamaño fijo para la demostración
    int front = 0, rear = 0;

    queue[rear++] = root;
    queue[rear++] = NULL;

    while (front < rear) {
        TreeNode* cur = queue[front++];
        if (cur == NULL && front < rear) {
            printf("\n");
            queue[rear++] = NULL;
            continue;
        }
        if (cur != NULL) {
            int left_height = (cur->left) ? cur->left->height : 0;
            int right_height = (cur->right) ? cur->right->height : 0;
            int balance_factor = left_height - right_height;
            printf(" (ID: %d, Age: %d, Height: %d, Balance: %d) ", cur->penguin.id, cur->penguin.age, cur->height, balance_factor);
            if (cur->left) {
                queue[rear++] = cur->left;
            }
            if (cur->right) {
                queue[rear++] = cur->right;
            }
        }
    }
    printf("\n");
}

// Función para liberar memoria del árbol
void free_tree(TreeNode* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}



// Funciones de Eventos

// Función auxiliar para buscar pingüinos con una edad específica y construir el mensaje
void search_by_age_helper(TreeNode* root, int age, char** message, int* first) {
    if (root == NULL) {
        return;
    }

    // Recorrer el subárbol izquierdo
    search_by_age_helper(root->left, age, message, first);

    // Si la edad del nodo coincide, agregamos el ID del pingüino al mensaje
    if (root->penguin.age == age) {
        // Si es el primer elemento, solo agregamos el ID
        if (*first) {
            *message = realloc(*message, strlen(*message) + 20); // Ajustar el tamaño del mensaje
            sprintf(*message + strlen(*message), "%d", root->penguin.id);
            *first = 0; // La próxima vez, no entrará aca
        } else {
            // Si no es el primer elemento, agregamos " - ID"
            *message = realloc(*message, strlen(*message) + 25); // Ajustar el tamaño del mensaje
            sprintf(*message + strlen(*message), " - %d", root->penguin.id);
        }
    }

    // Recorrer el subárbol derecho
    search_by_age_helper(root->right, age, message, first);
}

// Función principal para buscar pingüinos por edad y construir el mensaje
char* search_by_age(TreeNode* root, int age) {
    // Inicializamos el mensaje con espacio suficiente para la cabecera
    char *message = malloc(100);
    sprintf(message, "SEARCH-BY-AGE %d\n", age);

    int first = 1; // Variable para determinar si es el primer elemento
    search_by_age_helper(root, age, &message, &first);

    // Agregar nueva línea al final
    message = realloc(message, strlen(message) + 2);
    strcat(message, "\n");

    return message;
}

// Función auxiliar para buscar pingüinos con un rango de peso específico y construir el mensaje
void search_by_weight_range_helper(TreeNode* root, int min, int max, char** message, int* first) {
    if (root == NULL) {
        return;
    }

    // Recorrer el subárbol izquierdo
    search_by_weight_range_helper(root->left, min, max, message, first);

    // Si el peso del nodo está en el rango, agregamos el ID del pingüino al mensaje
    if (root->penguin.weight >= min && root->penguin.weight <= max) {
        // Si es el primer elemento, solo agregamos el ID
        if (*first) {
            *message = realloc(*message, strlen(*message) + 20); // Ajustar el tamaño del mensaje
            sprintf(*message + strlen(*message), "%d", root->penguin.id);
            *first = 0; // La próxima vez, no entrará aca
        } else {
            // Si no es el primer elemento, agregamos " - ID"
            *message = realloc(*message, strlen(*message) + 25); // Ajustar el tamaño del mensaje
            sprintf(*message + strlen(*message), " - %d", root->penguin.id);
        }
        // Ya estan ordenados por ID
    }

    // Recorrer el subárbol derecho
    search_by_weight_range_helper(root->right, min, max, message, first);
}

// Función principal para buscar pingüinos por rango de peso y construir el mensaje
char* search_by_weight_range(TreeNode* root, int min, int max) {
    // Inicializamos el mensaje con espacio suficiente para la cabecera
    char *message = malloc(100);
    sprintf(message, "SEARCH-BY-WEIGHT-RANGE %d %d\n", min, max);

    int first = 1; // Variable para determinar si es el primer elemento
    search_by_weight_range_helper(root, min, max, &message, &first);

    // Agregar nueva línea al final
    message = realloc(message, strlen(message) + 2);
    strcat(message, "\n");

    return message;
}


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

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

  int N; // Cantidad de pingüinos
  int E; // Cantidad de consultas

  int result = fscanf(input_file, "%d", &N);
  if (result != 1) {
    printf("Error reading number of penguins");
    return 1;
  }
  int id;
  int x;
  int y;
  int age;
  int weight;

  TreeNode* root = NULL;
  for (int i = 0; i < N; i++) {
    /* Recibir datos de los pingüinos */
    fscanf(input_file, "%d %d %d %d %d", &id, &x, &y, &age, &weight);
    printf("ID: %d, X: %d, Y: %d, AGE: %d, WEIGHT: %d\n", id, x, y, age, weight);
    Penguin newPenguin = {id, x, y, age, weight};
    root = insert(root, newPenguin);
    //print_tree(root);
    
  }
  result = fscanf(input_file, "%d", &E);
  if (result != 1) {
    printf("Error reading number of events");
    return 1;
  }

  char command[32];
  /* consultas */
  for (int i = 0; i < E; i++)
  {

    char* message = NULL;
    /* Recibir consultas */
    fscanf(input_file, "%s", command);
    if (string_equals(command, "SEARCH-BY-AGE")) {
      int edad;
      fscanf(input_file, "%d", &edad);
      message = search_by_age(root, edad);

    }
    else if (string_equals(command, "SEARCH-BY-WEIGHT-RANGE")) {
      int min;
      int max;
      fscanf(input_file, "%d %d", &min, &max);
      message = search_by_weight_range(root, min, max);
    }



    if (message != NULL) {
      fprintf(output_file, "%s", message);
      free(message);
    }

  }

/* Cerramos los archivos */
  fclose(input_file);
  fclose(output_file);

/* Liberamos memoria */
  return 0;
}
 