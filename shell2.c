
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shell-utils.h"

#define INPUT_BUFFER_SIZE 2048
#define MAX_TOKENS 256
// le handler pour ramasser les zombi 
void sigchld_handler (int sig){
	pid_t pid ;
	while ((pid = waitpid(-1 , NULL , WNOHANG))>0){
		printf("ok pid %d terminé continuons\n", pid );
	}
}

int main() {
	signal(SIGCHLD , sigchld_handler);
	/* Une variable pour sotcker les caractères tapés au clavier */
	char line[INPUT_BUFFER_SIZE+1];

	/* Une variable qui pointera vers les données lues par fgets
	 * Sauf problème (erreur, plus de données, ...), elle pointera en
	 * fait vers la variable précédente (line) */
	char* data;

	/* Un tableau où les mots de l'utilisateur seront stockés
	 * indépendamment les uns des autres
	 * Note: un mot est une suite de lettres séparées par une ou plusieurs
	 * espaces des autres mots.  */
	char* tokens[MAX_TOKENS+1];
	/* variables entières pour compter le nombre de token */
	int nb_tokens;
    while (1){

    printf("commande : ");
	/* Récupération des données entrées au clavier */
	data=fgets(line, INPUT_BUFFER_SIZE, stdin);

	if (data==NULL) {
		/* Erreur ou fin de fichier : on quitte tout de suite */
		if (errno) {
			/* Une erreur: on affiche le message correspondant
			 * et on quitte en indiquant une erreur */
			perror("fgets");
			exit(1);
		} else {
			/* Sinon ça doit être une fin de fichier.
			 * On l'indique et on quitte normalement */
			fprintf(stderr, "EOF: exiting\n");
			exit(0);
		}
     
	}

	/* On vérifie que l'utilisateur n'a pas donné une ligne qui soit trop longue */
	if (strlen(data) == INPUT_BUFFER_SIZE-1) {
		fprintf(stderr, "Input line too long: exiting\n");
		exit(1);
       
	}

	/* On découpe la ligne en tokens (mots)
	 * Voir documentation dans shell-utils.h (avec des exemples) */
	nb_tokens=split_tokens(tokens, data, MAX_TOKENS);

	/* S'il y a trop de tokens, on abandonne */
	if (nb_tokens==MAX_TOKENS) {
		fprintf(stderr, "Too many tokens: exiting\n");
		exit(1);
        
	}

	/* S'il n'y a pas de token, c'est que l'utilisateur n'a pas donné de
	 * commande. Il n'y a rien à faire. On arrête tout. */

	if (nb_tokens<=0) {
		fprintf(stderr, "No tokens found: exiting\n");
		exit(1);
	}
	if(strcmp(tokens[0],"exit")==0){
		printf("Bye!\n");
		exit(0);
	}

	/* On exécute la commande donné par l'utilisateur.
	 * Son nom est dans le premier token (le premier mot tapé)
	 * ses arguments (éventuels) seront les tokens suivants */
	pid_t pid;
	int pipe_in = -1;
	char** tokens_courant =tokens;
	int nb_Fils =0;
	int background=test_arriere_plan(nb_tokens,tokens_courant);

	while (tokens_courant != NULL){
         char** tokens_suivant = trouve_tube(tokens_courant, "|");
		 char *file_in = trouve_redirection(tokens_courant, "<");
		 char *file_out= trouve_redirection(tokens_courant, ">");
		 
		 int pipe_out[2];
		 if (tokens_suivant!=NULL){
			pipe(pipe_out);}
		 if ((pid=fork())==0){
			    if (file_in !=NULL){
					int fd = open( file_in,O_RDONLY);
					if (fd==-1){
						perror("open");
						printf("errno= %d : %s\n", errno, strerror(errno));
						exit(1);
					}
					dup2(fd,STDIN_FILENO);
					close(fd);
				}
				else if (pipe_in!=-1){
                    dup2(pipe_in,STDIN_FILENO);//y'avait une commande avant il lit dans le pipe in 
				}
				if (file_out!=NULL){
					int fd = open( file_out,O_WRONLY);
					if (fd==-1){
						perror("open");
						printf("errno= %d : %s\n", errno, strerror(errno));
						exit(1);
					}
					dup2(fd, STDOUT_FILENO);
					close(fd);
				} else if(tokens_suivant!=NULL){
					dup2(pipe_out[1],STDOUT_FILENO); //Il y'a une commande à venir il écrit dans le pipe_out 
				}
				//on ferme tous les fd inutiles ou déjà liés  
				if(pipe_in!=-1) close(pipe_in);
				if (tokens_suivant!=NULL){
					close(pipe_out[0]);
					close(pipe_out[1]);

				}
				
				execvp(tokens_courant[0], tokens_courant);
				perror("execvep");
		        exit(1);
			}
			nb_Fils++;
			//Dans le père on récupère la sortie du tube dans pipe in et on ferme les choses inutiles 
			if (tokens_suivant!=NULL){
				close(pipe_out[1]); // le père n'écrit rien 
				pipe_in = pipe_out[0];
			}else {
				pipe_in = -1; //plus de commandes à exec 
			}

            tokens_courant = tokens_suivant; 
		
	}
	if(background != 1){
	 for(int i =0 ; i<nb_Fils;i++ ){
		wait(NULL);
	 }
	}
}
    

	/* On ne devrait jamais arriver là */

	perror("execvp");
	exit(1);

}

//Note: la commande grep pose problème , ne s'exécute . Problème à identifier .
