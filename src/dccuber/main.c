#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <math.h>

#include "../file_manager/manager.h"

// guardamos en varaibles los diferentes valores



int pid_repartidores[500];
int pid_semaforos[3];

int rep_creados = 0;
int crear = 0;

int verde_s1 = 1;
int verde_s2 = 1;
int verde_s3 = 1;

int fabricapid;
int semaforopid;

int repartidores_necesarios;
int tiempo_de_repartidores;
int repartidores_completados = 0;


void crear_repartidor(){
  crear = 1;
};

void handler_sigabrt_fabrica(int sig)
{

  printf("FINALIZANDO LA FABRICA \n");
  printf("PROPAGANDO LA SEÑAL A LOS REPARTIDORES \n");
  for (int i = 0; i < rep_creados; i++)
  {
    kill(pid_repartidores[i], SIGABRT); 
  }


  //BORRRRAAAAAAAR
  //exit(0);



  while (repartidores_completados < rep_creados) {
    
  }

  printf("FABRICA FINALIZADA \n");
  exit(0);
  
}

void handler_sigint_principal(int sig)
{
  printf("PROCESO PRINCIPAL TERMINANDO A LA FUERZA\n");
  kill(getpid(), SIGABRT);
  
}

void handler_sigint_fabrica(int sig)
{
  printf("PROCESO FABRICA INTENTANDO TERMINAR POR SIGINT, NO PASA NADA\n");
}

void handler_sigausr2_fabrica(int sig)
{
  repartidores_completados+=1;
  printf("PROCESO DE REPARTIDOR COMPLETADO O INTERRUMPIDO\n");
  printf("repartidores completados: %i y repartidores creados: %i\n", repartidores_completados, rep_creados);
}

void handler_sigabrt_principal(int sig)
{

  printf("FINALIZANDO PROCESO PRINCIPAL \n");

  printf("ESPERANDO QUE TERMINE PROCESO FABRICA\n");
  kill(fabricapid, SIGABRT);
  int status;
  waitpid(fabricapid, &status, 0);

  for (int i = 0; i < 3; i++)
  {
    kill(pid_semaforos[i], SIGABRT); 
  }

  int n = 3;
  while (n > 0) {
    wait(NULL);
    printf("ESPERANDO QUE TERMINE PROCESO SEMAFORO.\n");
    --n;  // TODO(pts): Remove pid from the pids array.
  }

  printf("PROCESO PRINCIPAL FINALIZADO \n");
  exit(0);
 
}


void avisar_repartidor(int sig, siginfo_t *siginfo, void *ucontext){
  int semaforo = siginfo -> si_value.sival_int;
  // cambiamos el estado de los semaforos para los repartidores que van a ser creados

  if (semaforo == 1)
  {
    if (verde_s1 == 1)
    {
      verde_s1 = 0;
    }
    else if (verde_s1 == 0)
    {
      verde_s1 = 1;
    }
  }

  if (semaforo == 2)
  {
    if (verde_s2 == 1)
    {
      verde_s2 = 0;
    }
    else if (verde_s2 == 0)
    {
      verde_s2 = 1;
    }
  }

  if (semaforo == 3)
  {
    if (verde_s3 == 1)
    {
      verde_s3 = 0;
    }
    else if (verde_s3 == 0)
    {
      verde_s3 = 1;
    }
  }

  for (int i = 0; i < rep_creados; i++)
  { 
    send_signal_with_int(pid_repartidores[i], semaforo);
  }
};


int main(int argc, char const *argv[])
{

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

  char* distancia_semaforo_1 = data_in->lines[0][0];
  char* distancia_semaforo_2 = data_in->lines[0][1];
  char* distancia_semaforo_3 = data_in->lines[0][2];
  char* distancia_bodega = data_in->lines[0][3];

  char* tiempo_creacion = data_in->lines[1][0];
  char* envios_necesarios = data_in->lines[1][1];
  char* tiempo_semaforo_1 = data_in->lines[1][2];
  char* tiempo_semaforo_2 = data_in->lines[1][3];
  char* tiempo_semaforo_3 = data_in->lines[1][4];

  char** tiempos_semaforos = malloc(3 * sizeof(char*));

  tiempos_semaforos[0] = tiempo_semaforo_1;
  tiempos_semaforos[1] = tiempo_semaforo_2;
  tiempos_semaforos[2] = tiempo_semaforo_3;

  repartidores_necesarios = atoi(envios_necesarios);
  tiempo_de_repartidores = atoi(tiempo_creacion);


  


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


      signal(SIGALRM, crear_repartidor);
      signal(SIGABRT, handler_sigabrt_fabrica);
      alarm(tiempo_de_repartidores); 
      connect_sigaction(SIGUSR1, avisar_repartidor);
      signal(SIGUSR2, handler_sigausr2_fabrica);
      signal(SIGINT, handler_sigint_fabrica);

      while (1)
    {
      if (crear == 1)
      {
        crear = 0;
        rep_creados +=1 ;

        
        int repartidor_id = fork();

        if (repartidor_id == 0)
        {
          
          signal(SIGUSR1, SIG_IGN);
          char repartidores_creados_str[3];
          sprintf(repartidores_creados_str, "%i", rep_creados);

          //printf("repartidores creados hasta ahoraaaaa: %s", repartidores_creados_str);
          //printf("aaaaaaaaaaaaaaah");

          char* estado_s1_str;
          char* estado_s2_str;
          char* estado_s3_str;

          if (verde_s1)
          {
            estado_s1_str = "1";
          }
          else
          {
            estado_s1_str = "0";
          }

          if (verde_s2)
          {
            estado_s2_str = "1";
          }
          else
          {
            estado_s2_str = "0";
          }

          if (verde_s3)
          {
            estado_s3_str = "1";
          }
          else
          {
            estado_s3_str = "0";
          }

          //char *args[]={"./repartidor", distancia_semaforo_1, distancia_semaforo_2, distancia_semaforo_3, distancia_bodega, repartidores_creados_str, estado_s1_str, estado_s2_str, estado_s3_str, NULL};
          //execvp(args[0],args);

          execlp("./repartidor", distancia_semaforo_1, distancia_semaforo_2, distancia_semaforo_3, distancia_bodega, repartidores_creados_str, estado_s1_str, estado_s2_str, estado_s3_str, NULL);

          

        
          

          // execlp("./repartidor", distancia_s1, distancia_s2, distancia_s3, distancia_b, repartidores_creados_str, NULL);
        }
        pid_repartidores[rep_creados - 1] = repartidor_id;
        printf("REPARTIDOR CREADO ID: %i\n", repartidor_id);
        if (rep_creados < repartidores_necesarios)
        {
          printf("REPARTIDORES CREADOS HASTA EL MOMENTO: %i \n", rep_creados);
          alarm(tiempo_de_repartidores);
        }
        else{
          break;
        }
      }
    }

    //pid_t wpid;
    //int status;
    //while ((wpid = wait(&status)) > 0);
    while (repartidores_completados < rep_creados) {
      // mantenerse aqui hasta que todos los hijos terminen
    }
    printf("TERMINANDO PROCESO FABRICA\n");
    exit(0);

  }  

  else
  
  {
    signal(SIGABRT, handler_sigabrt_principal);
    signal(SIGINT, handler_sigint_principal);
    for (int i = 0; i < 3; i++)
    {
      semaforopid = fork();
      if (semaforopid == 0) {
        
        int id_semaforo = i + 1;
        char id_semaforo_str[2];
        sprintf(id_semaforo_str, "%i", id_semaforo);
        char fabrica_id_str[6];
        sprintf(fabrica_id_str, "%i", fabricapid);
        execlp("./semaforo", id_semaforo_str, tiempos_semaforos[i], fabrica_id_str, NULL);
      }
      pid_semaforos[i] = semaforopid;
    
    }
    
  int status;
  waitpid(fabricapid, &status, 0);
  printf("FABRICA YA TERMINÓ \n");

  printf("FINALIZANDO PROCESO PRINCIPAL \n");

  for (int i = 0; i < 3; i++)
  {
    kill(pid_semaforos[i], SIGABRT); 
  }

  int n = 3;
  while (n > 0) {
    wait(0);
    printf("ESPERANDO QUE TERMINE PROCESO SEMAFORO.\n");
    --n;
  }

  free(tiempos_semaforos);
  printf("PROCESO PRINCIPAL FINALIZADO \n");
  exit(0);
  


  
  }
}

}


    
      