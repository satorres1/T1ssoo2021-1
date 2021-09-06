#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */

#include "../file_manager/manager.h"

int id_unico = 1;

int main(int argc, char const *argv[])
{
  int status;
  printf("I'm the DCCUBER process and my PID is: %i\n", getpid());

  char *filename = "input.txt";
  InputFile *data_in = read_file(filename);

  printf("Leyendo el archivo %s...\n", filename);
  printf("- Lineas en archivo: %i\n", data_in->len);
  printf("- Contenido del archivo:\n");

  printf("\t- ");
  for (int i = 0; i < 4; i++)
  {
    printf("%s, ", data_in->lines[0][i]);
  }
  printf("\n");

  printf("\t- ");
  for (int i = 0; i < 5; i++)
  {
    printf("%s, ", data_in->lines[1][i]);
  }
  printf("\n");

  // guardamos en varaibles los diferentes valores
  char* distancia_semaforo_1 = data_in->lines[0][0];
  char* distancia_semaforo_2 = data_in->lines[0][1];
  char* distancia_semaforo_3 = data_in->lines[0][2];
  char* distancia_bodega = data_in->lines[0][3];

  char* tiempo_creacion = data_in->lines[1][0];
  char* envios_necesarios = data_in->lines[1][1];
  char* tiempo_semaforo_1 = data_in->lines[1][2];
  char* tiempo_semaforo_2 = data_in->lines[1][3];
  char* tiempo_semaforo_3 = data_in->lines[1][4];
 
  int fabricapid;

  printf("Liberando memoria...\n");
  input_file_destroy(data_in);


  fabricapid = fork();

  if (fabricapid >= 0)
  { 
    // de aqui em adelente es post fork de fabrica

    if (fabricapid == 0)

    // de aqui en adelante es en el hijo fabrica
    {
    
      printf("Soy la fabrica.. envios necesarios %i \n", atoi(envios_necesarios));
      
      for (int i = 0; i < atoi(envios_necesarios); i++)
      {
        int repartidorpid = fork();
        if (repartidorpid == 0) {
          char *args[]={"./repartidor", NULL};
          execvp(args[0],args);
        }
        else
        {
          // dormir de acuerdo al tiempo de creación
          printf("a dormir\n");
          printf("tiempo de creacion %i", tiempo_creacion);
          sleep(1);
        }
      }

    }

    else

    // aqui aún es el proceso principal
    {
      int semaforo_1;
      int semaforo_2;
      int semaforo_3;
      char idChar[80];
      char fabricaChar[80];
      
      sprintf(fabricaChar, "%i", fabricapid);
  
      semaforo_1 = fork();
      if (semaforo_1 == 0)
        {
          sprintf(idChar, "%i", id_unico);
          char *args[]={"./semaforo", tiempo_semaforo_1, idChar, fabricaChar, NULL};
          execvp(args[0],args);
        }
      else
      {
        id_unico += 1;
        semaforo_2 = fork();
        if (semaforo_2 == 0)
        {
          sprintf(idChar, "%i", id_unico);
          char *args[]={"./semaforo", tiempo_semaforo_2, idChar, fabricaChar, NULL};
          execvp(args[0],args);
        }

        else
        {
          id_unico += 1;
          semaforo_3 = fork();
          if (semaforo_3 == 0)
          {
            sprintf(idChar, "%i", id_unico);
            char *args[]={"./semaforo", tiempo_semaforo_3, idChar, fabricaChar, NULL};
            execvp(args[0],args);
          }
        }
      }
    }
  }
  else /* fork returns -1 on failure */
  {
      perror("fork"); /* display error message */
      exit(0); 
  }

  wait(&status);
  printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
}
