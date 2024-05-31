#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<stdio.h>
#include<stdlib.h>
#include<utils/utils.h>
#include <utils/utils.c>

//file descriptors de memoria y los modulos que se conectaran con ella
int fd_memoria;
int fd_kernel;
int fd_cpu;
int fd_entradasalida;

t_log* memoria_logger; //LOG ADICIONAL A LOS MINIMOS Y OBLIGATORIOS
t_config* memoria_config;
t_list* listProcesos; 

char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;

char* abrir_archivo(char* path, int PC);

bool comparacion(void* elemento, int pid)
{
	PCB* pcb = (PCB*)elemento;
	return pcb->PID == pid;
}

ProcesoMemoria* encontrarDeLista(t_list* lista, uint32_t pid)
{
	ProcesoMemoria* ret;
	t_list_iterator* iterador = list_iterator_create(lista);
	int i = 0;
	while( i < list_size(lista))
	{
		ProcesoMemoria* got = list_iterator_next(iterador);
		if( got->PID == pid )
		{
			ret = got;
		}
	}
	return ret;
}

void memoria_escuchar_cpu (){
		bool control_key = 1;
	while (control_key) {
			int cod_op = recibir_operacion(fd_cpu);
			printf("recibi el código de operacion de cpu\n");
			
			t_newPaquete* paquete = malloc(sizeof(t_newPaquete));
			paquete->buffer = malloc(sizeof(t_newBuffer));
			recv(fd_cpu,&(paquete->buffer->size),sizeof(uint32_t),0);
			paquete->buffer->stream = malloc(paquete->buffer->size);
			recv(fd_cpu,paquete->buffer->stream, paquete->buffer->size,0);
			printf("Pude obtener el paquete\n");
			switch (cod_op) {
			case MENSAJE: 
				//
				break;
			case PROCESO:
				//
				printf("Ejecute este mensaje PROCESO jaja\n");
				//desearializamos lo recibido
				PCB* proceso = deserializar_proceso_cpu(paquete->buffer);
				printf("Los datos recibidos de CPU son pid: %d\n",proceso->PID);
				printf("Los datos recibidos de CPU son pc: %d\n",proceso->PC);
				//abrimos el archivo asociado al proceso
				//ProcesoMemoria* datos = encontrarDeLista(listProcesos,proceso->PID);
				if (list_size(listProcesos) > 0) 
				{
						printf("Lista no vacia\n");
						int tamanioLista = list_size(listProcesos);
						printf("Tiene %d elementos\n", tamanioLista);
				} else {
						printf("Lista vacia\n");
				}
				
				ProcesoMemoria* datos = list_get(listProcesos,0);

				printf("Los datos encontrados son los siguientes pid: %d\n",datos->PID);
				printf("Los datos encontrados son los siguientes path: %s\n",datos->path);
				//leemos la linea indicada por el PC
			    char* instruccion = abrir_archivo(datos->path, proceso->PC); 
				printf("Enviaremos a la cpu: %s\n",instruccion); 
				//enviamos
				enviar_mensaje_cpu_memoria(instruccion,fd_cpu);

				break;
			case PAQUETE:
				//
				break;
			case -1:
				log_error(memoria_logger, "El cliente cpu se desconecto. Terminando servidor");
				control_key = 0;
			default:
				log_warning(memoria_logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}
}

void memoria_escuchar_entradasalida (){
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
				log_error(memoria_logger, "El cliente EntradaSalida se desconecto. Terminando servidor\n");
				control_key = 0;
			default:
				log_warning(memoria_logger,"Operacion desconocida. No quieras meter la pata\n");
				break;
			}
		}
}

void iterator(char* value) 
{
	log_info(memoria_logger,"%s", value);
}

char* leerArchivo(FILE* file)
{
	fseek(file,0,SEEK_END); //

	int tamanioArchivo = ftell(file);

	rewind(file);

	char* contenido = malloc((tamanioArchivo + 1) * sizeof(char) );
	if( contenido == NULL )
	{
		printf("Error al intentar reservar memoria\n");
		return NULL;
	}

	size_t leidos = fread(contenido, sizeof(char), tamanioArchivo, file);
	if( leidos < tamanioArchivo )
	{
		printf("No se pudo leer el contenido del archivo\n");
		free(contenido);
		return NULL;
	}

	contenido[tamanioArchivo] = '\0';
	return contenido;
}

char* abrir_archivo(char* path, int PC)
{
	FILE* file = fopen(path,"r");
	if( file == NULL )
	{
		printf("Error al abrir archivo, sorry\n");
	}
	char* content = leerArchivo(file);
	char** newContent = string_split(content,"\n");

	char* to_ret = malloc(strlen(newContent[PC])+1);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[0]);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[1]);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[2]);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[3]);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[4]);
	printf("Quiero ver qué cosa leyó la memoria: %s\n", newContent[5]);
	printf("El PC que tengo es: %d\n", PC);
	to_ret = newContent[PC];

    free(content);
	fclose(file);
	return to_ret;
}

void memoria_escuchar_kernel (){
		bool control_key = 1;
		
		
			//t_list* lista;
	while (control_key) {
			int cod_op = recibir_operacion(fd_kernel);
			printf("Recibi codigo de operacion\n");

			//debemos extraer el resto, primero el tamaño y luego el contenido
			t_newPaquete* paquete = malloc(sizeof(t_newPaquete));
			paquete->buffer = malloc(sizeof(t_newBuffer));

			recv(fd_kernel,&(paquete->buffer->size),sizeof(uint32_t),0);
			printf("Recibimos tamaño\n");
			
			paquete->buffer->stream = malloc(paquete->buffer->size);
			recv(fd_kernel,paquete->buffer->stream, paquete->buffer->size,0);
			printf("Recibi stream\n");
			
			switch (cod_op) {
			case MENSAJE:
				//
					printf("Ejecute este mensaje MENSAJE jaja\n");
					break;
			
			case PAQUETE:
			//FUNCIONES PARA CUANDO RECIBAMOS PAQUETE
				//lista = recibir_paquete(fd_kernel);
				//log_info(memoria_logger, "Me llegaron los siguientes valores:\n");
				//list_iterate(lista, (void*) iterator);
				// //extraigp el elemento lista y muestro sus parametros
				// ProcesoMemoria* random = list_get(lista,0);
				// printf("El pid recibido es %d", random->PID);
				// printf("El path recibido es %s", random->path);
			//crea conecccion cuando ingresamos INICIAR_PROCESO
			//ABRIR ARCHIVO PSEUDOCODIGO
				ProcesoMemoria* nuevoProceso = deserializar_proceso_memoria(paquete->buffer);
				if(nuevoProceso != NULL){ 
					//list proceso no se aniade correctamente a la lista
					list_add(listProcesos, nuevoProceso);
					printf("El PID que recibi es: %d\n", nuevoProceso->PID);
					printf("El PATH que recibi es: %s\n", nuevoProceso->path);
				} else{
					printf("No se pudo deserializar\n");
				}


				//free(nuevoProceso);
			break;
			case -1:
				log_error(memoria_logger, "El cliente kernel se desconecto. Terminando servidor\n");
				control_key = 0;

			default:
				log_warning(memoria_logger,"Operacion desconocida. No quieras meter la pata\n");
				break;
			}
			//liberar memoria
			free(paquete->buffer->stream);
			free(paquete->buffer);
			free(paquete);
			//cod_op = 0;
		}

}




#endif /* MEMORIA_H_ */