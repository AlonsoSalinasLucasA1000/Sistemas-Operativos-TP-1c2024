#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/string.h>
#include <pthread.h>
#include<readline/readline.h>
#include<commons/collections/queue.h>
#include <semaphore.h>


typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef enum{
	NEW,
	READY,
	BLOCKED,
	EXEC,
	EXIT
} estado_proceso;

typedef struct
{
	int AX;
	int BX;
	int CX;
	int DX;
} RegistrosCPU;

typedef struct
{
	int PID;
	int PC;
	int quantum;
	RegistrosCPU registro;
	estado_proceso estado;
	char* path;
} PCB;

int crear_conexion(char* ip, char* puerto,char* nameServ);
int iniciar_servidor(char* puerto, t_log* un_log, char* msj_server);
int esperar_cliente(int socket_servidor, t_log* un_log, char* msj);
int recibir_operacion(int socket_cliente);

void handshakeClient(int fd_servidor, int32_t handshake);
void handshakeServer(int fd_client);
void liberar_conexion(int socket);
void liberar_logger(t_log* logger);
void liberar_config(t_config* config);

#endif /* UTILS_H_ */