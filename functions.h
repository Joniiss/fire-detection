#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <time.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define TAM 30
#define THR 10

#define NO 'T'
#define FOGO '@'
#define LIVRE '-'
#define QUEIMADO '/'
#define MORTO '!'

extern char mat[TAM][TAM];



typedef struct node
{
	int x,y;
	int borda;
	int cima[6], baixo[6], esquerda[6], direita[6], centro[6];
	int CM, B, E, D, C;
	int id;
	int ativo;
}Node;

extern Node nodes[THR][THR];
extern pthread_t sensor_threads[THR][THR];
extern pthread_mutex_t mutex, mtx_prt;

void criar_floresta(char mat[TAM][TAM]);
void *print_floresta(void *args);
void *fogo(void *args);
void *sensor(void *args);
void *central (void *args);
void enviar_msg(int x, int y);
void prop_msg(int x, int y);
void limpar_msg(int msg[6]);
void apagar_fogo(int x, int y);
int convert_x(int id);
int convert_y(int id);


#endif
