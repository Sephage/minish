#include "lib.h"
#include "input.h"
#include "global.h"

pthread_t parsing;
pthread_t treatment;

pthread_mutex_t mutex;
pthread_cond_t endCmd;
pthread_cond_t endExecuting;


char **commande;
Save save;

int exitProgram = 0;
int endExecute = 1;

int currentSave = 0;
int numberSave = 0;
int isSave = 0;

int c = 0;
int nombreMot = 0;
int nombreLettre = 0;

/*
 * Thread qui execture les commandes qu'on demande
 * TODO
 *	Prise en comtpe de & DONE, voir si ca peut pas creer des problemes
 */
void *execute(void *p){
	char **cmd;
	int k;
	int pid;
	int background = 0;

	while(!exitProgram){
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&endCmd, &mutex);

		if(commande[0][0] != '\0'){
			if(strlen(commande[nombreMot-1]) == 1){
				if(!strncmp(commande[nombreMot-1], "&", 1)){
					nombreMot--;
					background = 1;
					commande[nombreMot][0] = '\0';
				}
			}
			cmd = calloc(nombreMot+1 , sizeof(char*));
			for(k=0;k<nombreMot;k++){
				cmd[k] = calloc(sizeof(commande[k]), sizeof(char));
				strcpy(cmd[k], commande[k]);
			}
			endExecute = 0;
			switch(pid = fork()){
				case -1:
					exitProgram = 1;
					return p;
				case 0:
					//printf("cmd[0] = %s\n",commande[0]);
					if(background)
						printf("pid : %d\n", getpid());
					if(execvp(*cmd,cmd) == -1){
						//	fprintf(stderr,"Erreur lors de execvp\n");
						perror("Commande non trouvée ");
						return p;
					}
			}
			if(!background){
				while(waitpid(pid,0,0) <= 0);
			}
		}

		pthread_cond_signal(&endExecuting);
		endExecute = 1;
		background = 0;

		for(k=0;k<MAXWORD;k++){
			commande[k] = calloc(MAXCHAR, sizeof(char));
		}
		nombreMot = 0;
		pthread_mutex_unlock(&mutex);
	}
	return p;
}

/**
 * Cree les différents threads et attend leur fin
 */
int main(int argc, char **argv){


	save.string = calloc(MAXSAVE, sizeof(char**));
	save.taille = malloc(MAXSAVE * sizeof(int));

	pthread_create(&parsing, 0, readInput, 0);
	pthread_create(&treatment, 0, execute, 0);
	if(pthread_mutex_init(&mutex,0) == -1){
		fprintf(stderr,"Failed to create mutex\n");
		return 1;
	}
	if(pthread_cond_init(&endCmd, 0) == -1){
		fprintf(stderr,"Condition can't be initialized\n");
		pthread_mutex_destroy(&mutex);
		return 2;
	}
	if(pthread_cond_init(&endExecuting, 0) == -1){
		fprintf(stderr,"Condition can't be initialized\n");
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&endCmd);
	}


	pthread_join(parsing, 0);
	pthread_join(treatment,0);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&endCmd);
	pthread_cond_destroy(&endExecuting);

	/*
	for(i=0;i<numberSave;i++){
		for(j = 0;j < save.taille[j];j++)
			free(save.string[i][j]);
	}
	free(save.taille);
	*/
	puts("\t\t**************************************");
	puts("\t\t\tThank you for your use");
	puts("\t\t**************************************");

	return 0;
}

/*void cleanup(){}*/
