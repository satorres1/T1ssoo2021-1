#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#include "../file_manager/manager.h"

int name;
int id_fabrica;
int t=0;
int cambios=0;

void handler_sigint_semaforo(int sig)
{
  printf("PROCESO SEMAFORO INTENTANDO TERMINAR POR SIGINT, NO PASA NADA\n");
}

void handler_sigabrt(int sig)
{

  printf("\nfinalizo el semaforo %i, realizando: %i cambios\n", name, cambios);
  char semaforo[20];
  sprintf(semaforo, "semaforo_%i.txt", name);
  FILE *output = fopen(semaforo, "w");

  fprintf(output, "%i", cambios);
  
  fclose(output);

  exit(0);
}

int main(int argc, char const *argv[])
{
  signal(SIGABRT, handler_sigabrt);
  signal(SIGINT, handler_sigint_semaforo);
  printf("I'm the SEMAFORO process and my PID is: %i, id: %s, tiempo: %s, fabrica_id: %s\n", getpid(), argv[0], argv[1], argv[2]);
  id_fabrica = atoi(argv[2]);
  int tiempo = atoi(argv[1]);
  name = atoi(argv[0]);
  int estado = 1;
  
  while(1)
  {
    sleep(tiempo);
    t+= tiempo;
    {
      if (estado == 1)
      {
        estado = 0;
        cambios +=1;
        send_signal_with_int(id_fabrica, name);
      }
      else if (estado == 0)
      {
        estado = 1;
        cambios +=1;
        send_signal_with_int(id_fabrica, name);
      }
    }
  }
}
