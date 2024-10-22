#include "functions.h"

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	srand(time(NULL));
	pthread_t p, f;
	pthread_t hora, ctrl;
	int prt, fr, hr, cnt;
	criar_floresta(mat);
	
	FILE *log_fire, *log_thr;
	log_fire = fopen("fogo.log", "w");
	fclose(log_fire);

	prt = pthread_create(&p, NULL, print_floresta, NULL); 
	if (prt){
		printf("Erro ao criar thread print_floresta. Return Code %d\n",prt);
		exit(-1);
	}
	fr = pthread_create(&f, NULL, fogo, NULL);
	if (fr){
		printf("Erro ao criar thread fogo. Return Code %d\n",fr);
		exit(-1);
	}
	cnt = pthread_create(&ctrl, NULL, central, NULL);
	if (cnt){
		printf("Erro ao criar thread central. Return Code %d\n",cnt);
		exit(-1);
	}

	pthread_join(f,NULL);
	pthread_mutex_destroy(&mutex); 
	return 0;
}