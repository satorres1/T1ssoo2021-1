#include <stdio.h>
#include <unistd.h>
#include "../file_manager/manager.h"

int main(int argc, char const *argv[])
{

  int estado = 1;

  sleep(2);
  printf("I'm the SEMAFORO process and my PID is: %i\n", getpid());
  printf("tiempo: %s y mi id es %s\n", argv[1], argv[2]);
  printf("pid_fabrica: %s\n", argv[3]);
  
}
