/*
 *	Programme sh
 *	1 Thread pour prendre le texte
 *	1 qui copie la commande et qui créé des processus 
 *	exitProgramme = 1 => Arrêt de tous les threads
 *	TODO
 *	  history avec sauvegarde
 *	  Remove \n of history live
 *	  pipe
 *	  redirection
 *	  define les différentes touches (pour rendre le code POSIX)
 *	  free when we leave, create function cleanup
 *	  
 *	  getcwd pour connaitre sa position absolue
 * */

#ifndef __LIB_H
#define __LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <termios.h>
#include <pthread.h>

#define MAXWORD 100
#define MAXCHAR 200
#define MAXSAVE 50

typedef struct{
	char ***string;
	int *taille;
}Save;


#endif
