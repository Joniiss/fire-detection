#include "functions.h"

char mat[TAM][TAM];
Node nodes[THR][THR];
pthread_t sensor_threads[THR][THR];
pthread_mutex_t mutex, mtx_prt; 

int convert_x(int id){
	int x;
	if (id % 10 == 0)
		id--;
	x = 3*(id/10) +1;
	return x;
}

int convert_y(int id){
	int y;
	if (id % 10 == 0){
		y = 28;
		return y;
	}
	y = 3 * (id%10) - 2;
	return y;
}

void criar_floresta(char mat[TAM][TAM]){
	memset(mat, LIVRE, sizeof(mat[0][0])*TAM*TAM);
	int k = 0;
	int l = 0;
	int id = 1;
	for (int i = 1, l = 0; l < THR, i < TAM; l++, i+=3){
	 	for (int j = 1, k = 0; k < THR, j < TAM; k++, j +=3){
			mat[i][j] = NO;
			nodes[l][k].x = i;
			nodes[l][k].y = j;
			nodes[l][k].id = id;
			nodes[l][k].CM = nodes[l][k].B = nodes[l][k].D = nodes[l][k].E = nodes[l][k].C = 0; 
			nodes[l][k].ativo = 1;

			if (l == 0 || l == 9 || k == 0 || k == 9)
				nodes[l][k].borda = 1;
			else
				nodes[l][k].borda = 0;
			id++;
			pthread_create(&sensor_threads[l][k], NULL, sensor, (void *)(intptr_t)id);
	 	}
	}
}


void enviar_msg(int x, int y){
	if (nodes[x][y].borda == 1)
		return;
	
	nodes[x][y].C = 0;

	//	Nó de cima
	memcpy(nodes[x-1][y].baixo, nodes[x][y].centro, sizeof(nodes[x][y].centro));
	nodes[x-1][y].B = 1;
	
	//	Nó de baixo
	memcpy(nodes[x+1][y].cima, nodes[x][y].centro, sizeof(nodes[x][y].centro));
	nodes[x+1][y].CM = 1;
	
	//	Nó da direita
	memcpy(nodes[x][y+1].esquerda, nodes[x][y].centro, sizeof(nodes[x][y].centro));
	nodes[x][y+1].E = 1;
	
	// Nó da esquerda
	memcpy(nodes[x][y-1].direita, nodes[x][y].centro, sizeof(nodes[x][y].centro));
	nodes[x][y-1].D = 1;
}

void prop_msg(int x, int y){
	if (nodes[x][y].borda == 1)
		return;
	int ok = 0;
	size_t size = sizeof(nodes[x][y].centro);
	if (nodes[x][y].CM == 1) //Recebeu de cima, manda para todos os outros
	{
		nodes[x][y].CM = 0;
		memcpy(nodes[x+1][y].cima, nodes[x][y].cima,size); //Nó de baixo
		nodes[x+1][y].CM = 1;
		memcpy(nodes[x][y+1].esquerda, nodes[x][y].cima,size); //Nó da direita
		nodes[x][y+1].E = 1;
		memcpy(nodes[x][y-1].direita, nodes[x][y].cima,size); //Nó da esquerda
		nodes[x][y-1].D = 1;
		ok = 1;
	}
	else if (nodes[x][y].B == 1) //Recebeu de baixo, manda para todos os outros
	{
		nodes[x][y].B = 0;
		memcpy(nodes[x-1][y].baixo, nodes[x][y].baixo,size); //Nó de cima
		nodes[x-1][y].B = 1;
		memcpy(nodes[x][y+1].esquerda, nodes[x][y].baixo,size); //Nó da direita
		nodes[x][y+1].E = 1;
		memcpy(nodes[x][y-1].direita, nodes[x][y].baixo,size); //Nó da esquerda
		nodes[x][y-1].D = 1;
		ok = 1;
	}
	else if (nodes[x][y].E == 1) //Recebeu da esquerda, manda para todos os outros
	{
		nodes[x][y].E = 0;
		memcpy(nodes[x-1][y].baixo, nodes[x][y].esquerda,size); //Nó de cima
		nodes[x-1][y].B = 1;
		memcpy(nodes[x+1][y].cima, nodes[x][y].esquerda,size); //Nó de baixo
		nodes[x+1][y].CM = 1;
		memcpy(nodes[x][y+1].esquerda, nodes[x][y].esquerda,size); //Nó da direita
		nodes[x][y+1].E = 1;
		ok = 1;
	}
	else if (nodes[x][y].D == 1) //Recebeu da direita, manda para todos os outros
	{
		nodes[x][y].D = 0;
		memcpy(nodes[x-1][y].baixo, nodes[x][y].direita,size); //Nó de cima
		nodes[x-1][y].B = 1;
		memcpy(nodes[x+1][y].cima, nodes[x][y].direita,size); //Nó de baixo
		nodes[x+1][y].CM = 1;
		memcpy(nodes[x][y-1].direita, nodes[x][y].direita,size); //Nó da esquerda
		nodes[x][y-1].D = 1;
		ok = 1;
	}
	if(ok)
		sleep(1);
}
void limpar_msg(int msg[6]){
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (memcmp(msg,nodes[i][j].cima,sizeof(msg[0])*6) == 0){
				nodes[i][j].CM = 0;
			}
			else if (memcmp(msg,nodes[i][j].baixo,sizeof(msg[0])*6) == 0){
				nodes[i][j].B = 0;
			}
			else if (memcmp(msg,nodes[i][j].esquerda,sizeof(msg[0])*6) == 0){
				nodes[i][j].E = 0;
			}
			else if (memcmp(msg,nodes[i][j].direita,sizeof(msg[0])*6) == 0){
				nodes[i][j].D = 0;
			}
			else if (memcmp(msg,nodes[i][j].centro,sizeof(msg[0])*6) == 0){
				nodes[i][j].C = 0;
			}
		}
	}

}

void apagar_fogo(int x, int y){
	sleep(1);
	if (mat[x][y] == QUEIMADO || mat[x][y] == FOGO)
		mat[x][y] = LIVRE;
}


void *print_floresta(void *args){
	while(1)
	{
		pthread_mutex_lock(&mtx_prt);
		//printf(BGGRN);
		system("clear");
		int idx, idy;
		int sensor = 0;
		//printf("\t");
		
		for (int i = 0; i < TAM; ++i)
		{
			printf("\t");
			for (int j = 0; j < TAM; ++j)
			{
				
				if (mat[i][j] == '*') 
				{	
					idx = ((i+2)/3)-1;
					idy = ((j+2)/3)-1;
					sensor = 1;
					mat[i][j] = MORTO;
				}

				if (mat[i][j] == QUEIMADO || mat[i][j] == FOGO)
					printf("@  ");
				else if (mat[i][j] == MORTO)
					printf("%c ",MORTO);
				else
					printf("%c  ",mat[i][j]);
				
			}
			printf("\n");
		}
		sensor = 0;
		pthread_mutex_unlock(&mtx_prt);
		sleep(1);
	}
}

void *fogo(void *args){
	FILE *log_thr;
	int x,y;
	int idx, idy;
	while(1){
		sleep(3);
		x = rand()%30;
		y = rand()%30;
		pthread_mutex_lock(&mtx_prt); 
		if (mat[x][y] == NO){
			mat[x][y] = '*';
			idy = ((y+2)/3)-1;
			idx = ((x+2)/3)-1;
			nodes[idx][idy].ativo = 0;
		}
		else if (mat[x][y] == MORTO){}
		else
			mat[x][y] = FOGO;
		pthread_mutex_unlock(&mtx_prt);
	}
}

void *sensor(void *args){
	int id = (intptr_t) args;
	int idx, idy;
	int x, y;
	int sent;
	x = convert_x(id);
	y = convert_y(id);
	idx = ((x+2)/3)-1;
	idy = ((y+2)/3)-1;
	while(1){
		if (nodes[idx][idy].ativo == 1)
		{
			sent = 0;
			pthread_mutex_lock(&mutex);
			for (int i = x-1; i <= x + 1; ++i)
			{
				for (int j = y-1; j <= y + 1; ++j)
				{			
					if (i == x && j == y)
						continue;
					if (mat[i][j] == FOGO)
					{	
						mat[i][j] = QUEIMADO;
						nodes[idx][idy].centro[0] = nodes[idx][idy].id;
						nodes[idx][idy].centro[1] = i;
						nodes[idx][idy].centro[2] = j;
						nodes[idx][idy].C = 1;
						if(nodes[idx][idy].borda == 0){	
							enviar_msg(idx, idy);
							sent = 1;
						}
					}
				}
			}
			if(sent == 0)
				prop_msg(idx,idy);
			
			pthread_mutex_unlock(&mutex);
			sleep(1);
		}
	}
}

void *central(void *args){
	int msg[6];
	size_t size = sizeof(msg);
	FILE *log_fire;
	while(1){
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (nodes[i][j].borda == 0)
					continue;

				if (nodes[i][j].CM == 1){
					nodes[i][j].CM = 0;
					log_fire = fopen("fogo.log", "a");
					memcpy(msg, nodes[i][j].cima, size);
					fprintf(log_fire,"Thread %d apagou fogo em [%d][%d]\n",msg[0],msg[1],msg[2]);
					fclose(log_fire);
					limpar_msg(msg);				
					apagar_fogo(msg[1],msg[2]);
				}
				else if (nodes[i][j].B == 1){
					nodes[i][j].B = 0;
					log_fire = fopen("fogo.log", "a");
					memcpy(msg, nodes[i][j].baixo, size);
					fprintf(log_fire,"Thread %d apagou fogo em [%d][%d]\n",msg[0],msg[1],msg[2]);
					fclose(log_fire);					
					limpar_msg(msg);				
					apagar_fogo(msg[1],msg[2]);
				}
				else if (nodes[i][j].E == 1){
					nodes[i][j].E = 0;
					log_fire = fopen("fogo.log", "a");
					memcpy(msg, nodes[i][j].esquerda, size);
					fprintf(log_fire,"Thread %d apagou fogo em [%d][%d]\n",msg[0],msg[1],msg[2]);
					fclose(log_fire);					
					limpar_msg(msg);				
					apagar_fogo(msg[1],msg[2]);
				}
				else if (nodes[i][j].D == 1){
					nodes[i][j].D = 0;
					log_fire = fopen("fogo.log", "a");
					memcpy(msg, nodes[i][j].direita, size);
					fprintf(log_fire,"Thread %d apagou fogo em [%d][%d]\n",msg[0],msg[1],msg[2]);
					fclose(log_fire);					
					limpar_msg(msg);				
					apagar_fogo(msg[1],msg[2]);
				}
				else if (nodes[i][j].C == 1){
					nodes[i][j].C = 0;
					log_fire = fopen("fogo.log", "a");
					memcpy(msg, nodes[i][j].centro, size);
					fprintf(log_fire,"Thread %d apagou fogo em [%d][%d]\n",msg[0],msg[1],msg[2]);
					fclose(log_fire);					
					limpar_msg(msg);			
					apagar_fogo(msg[1],msg[2]);
				}
			}
		}
		sleep(1);
	}
}
