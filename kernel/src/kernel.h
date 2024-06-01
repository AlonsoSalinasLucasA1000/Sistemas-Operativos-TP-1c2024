#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>
#include <utils/utils.c>

//file descriptors de kernel y los modulos que se conectaran con ella
int fd_kernel;
int fd_cpu_interrupt; 
int fd_cpu_dispach; //luego se dividira en dos fd, un dispach y un interrupt, por ahora nos es suficiente con este
int fd_memoria;
int fd_entradasalida;

//colas y pid
int procesos_en_new = 0;
int procesos_fin = 0;

sem_t sem; //semaforo mutex region critica cola new
sem_t sem_ready; 
sem_t sem_cant; //semaforo cant de elementos en la cola
sem_t sem_cant_ready;

int pid = 0;
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_blocked;

t_log *kernel_logger; // LOG ADICIONAL A LOS MINIMOS Y OBLIGATORIOS
t_config *kernel_config;
t_log *kernel_logs_obligatorios;// LOGS MINIMOS Y OBLIGATORIOS

char *PUERTO_ESCUCHA;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *IP_CPU;
char *PUERTO_CPU_DISPATCH; 
char *PUERTO_CPU_INTERRUPT;
char *ALGORITMO_PLANIFICACION;
char *QUANTUM; //Da segmentation fault si lo defino como int
char* RECURSOS;
char* INSTANCIAS_RECURSOS;
char* GRADO_MULTIPROGRAMACION; //Da segmentation fault si lo defino como int




void kernel_escuchar_cpu ()
{
	bool control_key = 1;
	while (control_key) {
			int cod_op = recibir_operacion(fd_cpu_dispach);
			switch (cod_op) {
			case MENSAJE:
				//
				break;
			case PAQUETE:
				//
				break;
			case -1:
				log_error(kernel_logger, "Desconexion de cpu.");
				control_key = 0;
			default:
				log_warning(kernel_logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}	
}


void kernel_escuchar_entradasalida ()
{
	bool control_key = 1;
	while (control_key) {
			int cod_op = recibir_operacion(fd_entradasalida);
			switch (cod_op) {
			case MENSAJE:
				//
				break;
			case PAQUETE:
				//
				break;
			case -1:
				log_error(kernel_logger, "El cliente EntradaSalida se desconecto. Terminando servidor");
				control_key = 0;
			default:
				log_warning(kernel_logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}	
}

void kernel_escuchar_memoria ()
{
	bool control_key = 1;
	while (control_key) {
			int cod_op = recibir_operacion(fd_memoria);
			switch (cod_op) {
			case MENSAJE:
				//
				break;
			case PAQUETE:
				//
				break;
			case -1:
				log_error(kernel_logger, "Desconexion de memoria.");
				control_key = 0;
			default:
				log_warning(kernel_logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}	
}

void iniciar_proceso(char* path)
{
	PCB* pcb = malloc(sizeof(PCB));
	if ( pcb == NULL )
	{
		printf("Error al crear pcb");
	}

	printf("En INICIAR PROCESO llego el siguiente PATH: %s\n",path);
	//inicializo el PCB del proceso
	pid++;
	pcb->PID = pid;
	pcb->PC = 0;
	pcb->quantum = 0;
	pcb->AX = 123;
	pcb->BX = 0;
	pcb->CX = 0;
	pcb->DX = 0;
	pcb->estado = NEW;
	pcb->path = string_duplicate(path); //consejo

	printf("En INICIAR PROCESO en la PCB se guardo el sigueinte PATH: %s\n",pcb->path);
	//agrego el pcb a la cola new
	
	sem_wait(&sem);
    printf("Entrando en la sección crítica...\n");
	queue_push(cola_new,pcb);
	printf("Saliendo de la sección crítica...\n");
	
    // Señalar (incrementar) el semáforo
    sem_post(&sem);
	sem_post(&sem_cant);

	//procesos_en_new++;
	
	log_info (kernel_logs_obligatorios, "Se crea el proceso %d en NEW, funcion iniciar proceso\n", pcb->PID);


}

void atender_instruccion (char* leido)
{
    char** comando_consola = string_split(leido, " ");
	//printf("%s\n",comando_consola[0]);

    if((strcmp(comando_consola[0], "INICIAR_PROCESO") == 0))
	{ 
        iniciar_proceso(comando_consola[1]);  
    }else if(strcmp(comando_consola [0], "FINALIZAR_PROCESO") == 0){
    }else if(strcmp(comando_consola [0], "DETENER_PLANIFICACION") == 0){
    }else if(strcmp(comando_consola [0], "INICIAR_PLANIFICACION") == 0){
    }else if(strcmp(comando_consola [0], "MULTIPROGRAMACION") == 0){ 
    }else if(strcmp(comando_consola [0], "PROCESO_ESTADO") == 0){
    }else if(strcmp(comando_consola [0], "HELP") == 0){
    }else if(strcmp(comando_consola [0], "PRINT") == 0){
    }else{   
        log_error(kernel_logger, "Comando no reconocido, pero que paso el filtro ???");  
        exit(EXIT_FAILURE);
    }
    string_array_destroy(comando_consola); 
}

void validarFuncionesConsola(char* leido)
{
	 char** valorLeido = string_split(leido, " ");
	 //printf("%s\n",valorLeido[0]);

	 if(strcmp(valorLeido[0], "EJECUTAR_SCRIPT") == 0)
	 {
		printf("Comando válido\n");
	 }
	 else
	 {
		if(strcmp(valorLeido[0], "INICIAR_PROCESO") == 0)
	    {
		    printf("Comando válido\n");
			atender_instruccion(leido);
	    }
		else
		{
			if(strcmp(valorLeido[0], "FINALIZAR_PROCESO") == 0)
	        {  
		         printf("Comando válido\n");
	        }
			else
			{
				if(strcmp(valorLeido[0], "DETENER_PLANIFICACION") == 0)
	            {  
		             printf("Comando válido\n");
	            }
				else
				{
					if(strcmp(valorLeido[0], "INICIAR_PLANIFICACION") == 0)
	                {  
		                printf("Comando válido\n");
	                }
					else
					{
						if(strcmp(valorLeido[0], "MULTIPROGRAMACION") == 0)
	                    {  
		                       printf("Comando válido\n");
	                    }
						else
						{
							if(strcmp(valorLeido[0], "PROCESO_ESTADO") == 0)
	                        {  
		                       printf("Comando válido\n");
	                        }
						}
					}
				}
			}
		}
	 }
	 string_array_destroy(valorLeido);
}

void consolaInteractiva()
{
	char* leido;
	leido = readline("> ");
	
	while( strcmp(leido,"") != 0 )
	{
		validarFuncionesConsola(leido);
		free(leido);
		leido = readline("> ");
	}
}


void enviarProcesoMemoria (ProcesoMemoria* proceso, int socket_servidor)
{
    //Creamos un Buffer
    t_newBuffer* buffer = malloc(sizeof(t_newBuffer));

    //Calculamos su tamaño
    buffer->size = sizeof(uint32_t)*2 + proceso->path_length +1;
    buffer->offset = 0;
    buffer->stream = malloc(buffer->size);
	
    //Movemos los valores al buffer
    memcpy(buffer->stream + buffer->offset, &proceso->PID, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Para el nombre primero mandamos el tamaño y luego el texto en sí:
    memcpy(buffer->stream + buffer->offset, &proceso->path_length, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
    memcpy(buffer->stream + buffer->offset, proceso->path, proceso->path_length);
    
	//Creamos un Paquete
    t_newPaquete* paquete = malloc(sizeof(t_newPaquete));
    //Podemos usar una constante por operación
    paquete->codigo_operacion = PAQUETE;
    paquete->buffer = buffer;

    //Empaquetamos el Buffer
    void* a_enviar = malloc(buffer->size + sizeof(op_code) + sizeof(uint32_t));
    int offset = 0;
    memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_code));
    offset += sizeof(op_code);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
    //Por último enviamos
    send(socket_servidor, a_enviar, buffer->size + sizeof(op_code) + sizeof(uint32_t), 0);

    // No nos olvidamos de liberar la memoria que ya no usaremos
    free(a_enviar);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}



void informar_memoria_nuevo_procesoNEW()
{
	//CREAMOS BUFFER
	//POR EL MOMENTO ESTAMOS HARCODEANDO para intentar mandar algo a memoria
	ProcesoMemoria* proceso = malloc(sizeof(ProcesoMemoria));

	PCB* pcb = queue_pop(cola_new);//
	proceso->path = malloc(strlen(pcb->path)+1);
	proceso->path = pcb->path;
	proceso->PID = pcb->PID;
	proceso->path_length = strlen(pcb->path)+1;
	
	
	enviarProcesoMemoria(proceso,fd_memoria);
	
	sem_wait(&sem_ready);   // mutex hace wait
	queue_push(cola_ready,pcb);	//agrega el proceso a la cola de ready
    sem_post(&sem_ready); 
	sem_post(&sem_cant_ready);  // mutex hace wait
	
}


void finalizar_proceso ()
{
}

void mover_procesos_ready(int grado_multiprogramacion)
{
	//CONSULTA. TENDREMOS QUE USAR SEMAFOROS?
    //Cantidad de procesos en las colas de NEW y READY
    int cantidad_procesos_new = queue_size(cola_new);
    int cantidad_procesos_ready = queue_size(cola_ready);

    //El grado de multiprogramación lo permite?
    if (cantidad_procesos_ready < grado_multiprogramacion)
    {
        // Mover procesos de la cola de NEW a la cola de READY
        while (cantidad_procesos_new > 0 && cantidad_procesos_ready < grado_multiprogramacion)
        {
            // Seleccionar el primer proceso de la cola de NEW y los borramos de la cola
            PCB* proceso_nuevo = queue_pop(cola_new);
           // queue_pop(cola_new);

            // Cambiar el estado del proceso a READY
            proceso_nuevo->estado = READY;

            // Agregar el proceso a la cola de READY
		

			queue_push(cola_ready, proceso_nuevo);
            cantidad_procesos_ready++;
			
		
           

            // Reducir la cantidad de procesos en la cola de NEW
            cantidad_procesos_new--;
        }
    }
    else
    {
        printf("El grado de multiprogramación máximo ha sido alcanzado. %d procesos permanecerán en la cola de NEW.\n",cantidad_procesos_new);
    }
}

void planificador_largo_plazo()
{
    // //Obtenemos el grado de multiprogramación especificado por el archivo de configuración
    // //int grado_multiprogramacion = config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
	// int grado_multiprogramacion = 20;
	// //pregunto constantemente
	// while(1)
	// {
	// 	//hay nuevos procesos en new?
	// 	if( procesos_en_new > 0 )
	// 	{
	// 		//avisar a memoria
	// 		//USAR QUEUE Pop
	while(1)
	{
		//SEMAFOROS (PRODUCTOR-CONSUMIDOR)  consumidor de la cola de NEW

		//if( queue_size(cola_new) > 0)
		//{	
		    sem_wait(&sem_cant);   // mutex hace wait
		    sem_wait(&sem);   // mutex hace wait

			informar_memoria_nuevo_procesoNEW();
			
			sem_post(&sem);   // mutex hace signal

			//sem_post(&sem_cant_ready); //avisamos al planificador que hay un nuevo proceso listo
			
		//}

	}			
	// 	}
	// 	else
	// 	{
	// 		//algun proceso terminó?
	// 		if(procesos_fin > 0 )
	// 		{
	// 			//
	// 			printf("esta parte es la del fin del proceso");
	// 		}
	// 	}
	// 	//luego de esto muevo los procesos a ready mientras pueda
	// 	//mover_procesos_ready(grado_multiprogramacion);
	// 	sleep(1);
	//}	
}

void enviar_pcb_a_cpu()
{
	//Reservo memoria para enviarla
	PCB* to_send = malloc(sizeof(PCB));

	sem_wait(&sem_cant_ready);   // mutex hace wait
	sem_wait(&sem_ready);   // mutex hace wait

	PCB* pcb_cola = queue_pop(cola_ready); //saca el proceso de la cola de ready
			
	sem_post(&sem_ready); // mutex hace signal
	

	to_send->PID = pcb_cola->PID;
	to_send->PC = pcb_cola->PC;
	to_send->quantum = pcb_cola->quantum;
	to_send->estado = EXEC;
	to_send->AX = pcb_cola->AX;
	to_send->BX = pcb_cola->BX;
	to_send->CX = pcb_cola->CX;
	to_send->CX = pcb_cola->CX;
	to_send->path = string_duplicate( pcb_cola->path);

	enviarPCB(to_send, fd_cpu_interrupt);
}

void planificador_corto_plazo()
{

	while(1)
		{
			//usamos semaforo para avisar
			//enviamos la pcb a la cpu
			enviar_pcb_a_cpu();
			//esperamos respuesta de la cpu
			//algun receive
		}
}
	

#endif KERNEL_H_ 
