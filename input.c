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

int currentSave;
int numberSave;
int isSave ;
int exitProgram;
int endExecute;

int c;
int nombreMot;
int nombreLettre;



/*
 * Fonction that parse char when user type it
 * This function doesn't parse when user confirm his command
 */
int parse(){
	if(c == 27){
		struct termios init;
		struct termios config;
		int d;
		tcgetattr(STDIN_FILENO, &init);
		config = init;
		config.c_cc[VMIN] = 0;
		config.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSANOW, &config);
		if((d = getchar()) == 91){
			int k = 0;
			if((d=getchar()) == 65){
			//	for(k=0;k<save.taille[numberSave-1];k++)
			//		printf("%s",save.string[numberSave-1][k]);
				if(currentSave == -1){
					currentSave = numberSave;
				}
				else if(currentSave >= 0 && numberSave > 0){
					nombreMot = save.taille[currentSave];
					commande = calloc(MAXWORD,sizeof(char*));
				//	printf("currentSave = %d ",currentSave);
					for(k=0;k<MAXWORD;k++){
						commande[k] = calloc(MAXCHAR, sizeof(char));
					}
					//	if(save[currentSave][k] != NULL)
					for(k=0;k<save.taille[currentSave];k++){
					//	printf("%s", save.string[currentSave][k]);
						strcpy(commande[k], save.string[currentSave][k]);
					}
					/*\0332K = Erase line \033u = cursor go to the last save*/
					printf("\033[2K \033[u \033[1D\033[1D\033[1D\033[1D$ ");
					for(k=0;k<nombreMot;k++){
						printf("%s ", commande[k]);
					}
					currentSave--;
					nombreLettre = 0;
					isSave = 1;
				}
			}
			else if(d == 66){
				if(currentSave == numberSave+1){
					currentSave = 0;
				}
				else if(currentSave <= numberSave){
					nombreMot = save.taille[currentSave];
					commande = calloc(MAXWORD,sizeof(char*));
					for(k=0;k<MAXWORD;k++){
						commande[k] = calloc(MAXCHAR, sizeof(char));
					}
					for(k=0;k<save.taille[currentSave];k++){
						strcpy(commande[k], save.string[currentSave][k]);
					}
					/*\0332K = Erase line \033u = cursor go to the last save*/
					printf("\033[2K \033[u \033[1D\033[1D\033[1D\033[1D$ ");
					for(k=0;k<nombreMot;k++){
						printf("%s ", commande[k]);
					}
					currentSave++;
					nombreLettre = 0;
					commande[nombreMot][0] = '\0';
					isSave = 1;
				}
			}
			c = 0;
		}
		tcsetattr(STDIN_FILENO, TCSANOW, &init);
	}
	else if(c > 32 && c != 127){//Displayable 
		if(nombreLettre < MAXCHAR){
			printf("%c",(char)c);
			commande[nombreMot][nombreLettre++] = (char)c;
			isSave = 0;
		}else{
			fprintf(stdin,"Taille dépasse la limite autorisée\n");
			c = 10;
		}
	}
	else if(c == 32){ //Space
		printf(" ");
		if(nombreMot < MAXWORD){
			nombreLettre = 0;
			nombreMot++;
			isSave = 0;
		}else{
			c = 10;
			puts("Trop de mots, FIN");
		}

	}
	else if(c == 127){ //BackSpace
		if(nombreLettre > 0){
			nombreLettre--;
			commande[nombreMot][nombreLettre] = '\0';
			printf("\033[1D \033[1D");
		}
		else if(nombreLettre==0 && nombreMot>0){
			nombreMot--;
			nombreLettre = sizeof(commande[nombreMot])-1;
			printf("\033[1D \033[1D");
		}
		isSave = 0;
	}
	return 0;
}


/*
 * Thread qui parse ce que l'on saisi
 * TODO :
 */
void *readInput(void *t){
	int k;
	struct termios initial;
	struct termios config;
	char *dir = "";

	tcgetattr(STDIN_FILENO, &initial);
	config = initial;
	config.c_lflag &= ~ICANON;
	config.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &config);

	pthread_mutex_lock(&mutex);
	commande = malloc(MAXWORD * sizeof(char*));
	for(k=0;k<MAXWORD;k++){
		commande[k] = calloc(MAXCHAR , sizeof(char));
	}
	pthread_mutex_unlock(&mutex);

	while(c != 27){
		pthread_mutex_lock(&mutex);
		if(!endExecute){
			pthread_cond_wait(&endExecuting, &mutex);
		}
		pthread_mutex_unlock(&mutex);

		printf("%s", getcwd(dir, 1024));
		fputs(" $ ", stdout);
		printf("\033[s");
		endExecute = 0;
		do{
			c = getchar();

			pthread_mutex_lock(&mutex);
			parse();
			pthread_mutex_unlock(&mutex);

		}while(c != 27 && c != 10);
		puts(" ");
		if(!isSave)
			nombreMot++;
		
		/*
		 * We save the commande that we type
		 */
		if(numberSave < MAXSAVE){
			int i;
			save.string[numberSave] = calloc(nombreMot+1, sizeof(char*));
			for(i = 0;i < nombreMot;i++){
				save.string[numberSave][i] = calloc(strlen(commande[i]),sizeof(char));
				strcpy(save.string[numberSave][i], commande[i]);
			}
			//save.string[numberSave][i-1][nombreLettre] = '\0';
			save.taille[numberSave] = nombreMot;
			numberSave++;
		}
		pthread_cond_signal(&endCmd);

		nombreLettre = 0;
		currentSave = numberSave-1;
	}
	exitProgram = 1;
	puts(" ");
	tcsetattr(STDIN_FILENO, TCSANOW, &initial);
	return t;
}

