#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include "../file_manager/manager.h"


int turnos_sem1 = -1;
int turnos_sem2 = -1;
int turnos_sem3 = -1;
int turnos_bodega = -1;

int estado_s1;
int estado_s2;
int estado_s3;
int id_repartidor;

void handler_sigabrt(int sig)
{

  FILE *fp;
  char nombre_archivo[27];
  sprintf(nombre_archivo, "repartidor_%i.txt", id_repartidor);
  fp = fopen( nombre_archivo, "w");
  fprintf(fp, "%i,%i,%i,%i\n", turnos_sem1, turnos_sem2, turnos_sem3, turnos_bodega);
  fclose(fp);
  srand(time(0));
  int randomnumber;
  randomnumber = rand() % 3;
  sleep(randomnumber);
  kill(getppid(), SIGUSR2);
  printf("Termino FORZADAMENTE repartidor con ID: %i\n", getpid());
  exit(0);
  
}

void handler_sigint_repartidor(int sig)
{
  printf("PROCESO REPARTIDOR INTENTANDO TERMINAR POR SIGINT, NO PASA NADA\n");
}


void actualizar_semaforo(int sig, siginfo_t *siginfo, void *ucontext) {
  int semaforo = siginfo -> si_value.sival_int;
  if (semaforo == 1)
  {
    if (estado_s1 == 1)
    {
      estado_s1 = 0;
    }
    else if (estado_s1 == 0)
    {
      estado_s1 = 1;
    }
  }

  if (semaforo == 2)
  {
    if (estado_s2 == 1)
    {
      estado_s2 = 0;
    }
    else if (estado_s2 == 0)
    {
      estado_s2 = 1;
    }
  }

  if (semaforo == 3)
  {
    if (estado_s3 == 1)
    {
      estado_s3 = 0;
    }
    else if (estado_s3 == 0)
    {
      estado_s3 = 1;
    }
  }
}
int main(int argc, char const *argv[])
{
  printf("I'm the REPARTIDOR process and my PID is: %i\n", getpid());
  //int distancia_semaforo_1 = atoi(argv[1]);
  int distancia_semaforo_1 = 2;
  int distancia_semaforo_2 = 5;
  int distancia_semaforo_3 = 77;
  int distancia_bodega = 9;
  //printf("DISTANCIA SEMAFORO 1: %i\n", distancia_semaforo_1);
  //int distancia_semaforo_2 = atoi(argv[2]);
  //printf("DISTANCIA SEMAFORO 2: %i\n", distancia_semaforo_2);
  //int distancia_semaforo_3 = atoi(argv[3]);
  //printf("DISTANCIA SEMAFORO 3: %i\n", distancia_semaforo_3);
  //int distancia_bodega = atoi(argv[4]);
  //printf("DISTANCIA A BODEGA: %i\n", distancia_bodega);
  int id_repartidor = atoi(argv[4]);
  printf("ID REPARTIDOR: %i\n", id_repartidor);

  //estado_s1 = atoi(argv[6]);
  estado_s1 = 1;
  printf("\nESTADO SEMAFORO 1 AL CREAR: %i\n", estado_s1);
  printf("\nESTADO SEMAFORO 1 AL CREAR COMO STR: %s\n", argv[5]);

  estado_s2 = 1;
  //estado_s1 = atoi(argv[7]);
  printf("\nESTADO SEMAFORO 2 AL CREAR: %i\n", estado_s2);
   printf("\nESTADO SEMAFORO 2 AL CREAR COMO STR: %s\n", argv[6]);

  estado_s3 = 1;
  //estado_s1 = atoi(argv[8]);
  printf("\nESTADO SEMAFORO 3 AL CREAR: %i\n", estado_s3);
   printf("\nESTADO SEMAFORO 3 AL CREAR COMO STR: %s\n", argv[7]);

  int llego = false;
  int turnos = 0;
  int posicion = 0;
  connect_sigaction(SIGUSR1, actualizar_semaforo);
  signal(SIGABRT, handler_sigabrt);
  signal(SIGINT, handler_sigint_repartidor);
  while (llego == false)
  {
    sleep(1);
    
    if (posicion+1 == distancia_semaforo_1)
    {
      
      if (estado_s1 == 0)
      {
        turnos++;
      }
      if (estado_s1 == 1)
      {
        posicion++;
        turnos++;
        turnos_sem1 = turnos;
      }
      
    }
    else if (posicion+1 == distancia_semaforo_2)
    {
      
      if (estado_s2 == 0)
      {
        turnos++;
      }
      if (estado_s2 == 1)
      {
        posicion++;
        turnos++;
        turnos_sem2 = turnos;
      }
    }
    else if (posicion+1 == distancia_semaforo_3)
    {
      
      if (estado_s3 == 0)
      {
        turnos++;
      }
      if (estado_s3 == 1)
      {
        posicion++;
        turnos++;
        turnos_sem3 = turnos;
      }
    }
    
    
    else if (posicion+1 == distancia_bodega)
    {
      posicion++;
      turnos++;
      turnos_bodega = turnos;
      llego = true;
    }
    else {
      posicion++;
      turnos++;
    }
    printf("Posicion repartidor %i: %i,    Turno: %i\n", id_repartidor, posicion, turnos);
    
  }

  FILE *fp;
  char nombre_archivo[27];
  sprintf(nombre_archivo, "repartidor_%i.txt", id_repartidor);
  fp = fopen( nombre_archivo, "w");
  fprintf(fp, "%i,%i,%i,%i\n", turnos_sem1, turnos_sem2, turnos_sem3, turnos_bodega);
  fclose(fp);
  srand(time(0));
  int randomnumber;
  randomnumber = rand() % 3;
  sleep(randomnumber);
  kill(getppid(), SIGUSR2);
  printf("Termino CORRECTAMENTE repartidor con ID: %i\n", getpid());
  sleep(1);
  exit(0);
  
}