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

  int E; // Cantidad de eventos

  int result;
  result = fscanf(input_file, "%d", &E);
  if (result != 1) {
    printf("Error reading events");
    return 1;
  }
  
  /* Instanciamos nuestras estructuras */


  char command[32];
  for (int i = 0; i < E; i++) {
    result = fscanf(input_file, "%s", command);
    if (result != 1) {
      printf("Error reading event");
      return 1;
    }
    printf("%s\n", command);
  }


/* Cerramos los archivos */
  fclose(input_file);
  fclose(output_file);

/* Liberamos memoria */

  return 0;
}
 