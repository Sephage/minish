#ifndef __GLOBAL_H
#define __GLOBAL_H
extern pthread_t parsing;
extern pthread_t treatment;

extern pthread_mutex_t mutex;
extern pthread_cond_t endCmd;
extern pthread_cond_t endExecuting;


extern char **commande;
extern Save save;

extern int exitProgram;
extern int endExecute;

extern int currentSave;
extern int numberSave;

extern int c;
extern int nombreMot;
extern int nombreLettre;


#endif
