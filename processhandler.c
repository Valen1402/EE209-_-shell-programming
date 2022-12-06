/*--------------------------------------------------------------------*/
/*                                                               */
/* Full name : Minh Quang Nguyen                                      */
/* Student ID: 20190723															     */
/*--------------------------------------------------------------------*/
#define _GNU_SOURCE

#include "dynarray.h"
#include "lex.h"
#include "processhandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <fcntl.h>

pid_t child_pid = (pid_t) -1;
char c = 92;

void INT(int iSig){
	kill(child_pid, SIGINT);
}
/*
void QUIT(int iSig) {
	kill(child_pid, SIGQUIT);
	printf("Type Ctrl-%c again within 5 seconds to exit.\n", c);
}

void QUITALL(int iSig) {
		kill(getpid(), SIGQUIT);
}
*/

int ProcessHandler(DynArray_T oTokens){
	int len = DynArray_getLength(oTokens);
	int count, i =-1, status;
	int pipefd[2], type;
	pipe(pipefd);
	char* pgm = ((struct Token*) (DynArray_get(oTokens, 0)))->pcValue;
	char* argv[len+1];// *argv2[len+1];
	int fd;
	
	for(count =0; count < len; count++){
		i++;
		type = (((struct Token*) DynArray_get(oTokens, count)))->eType;
		argv[i] = (((struct Token*) DynArray_get(oTokens, count)))->pcValue;
		if(type) //REDIRECTION CHAR
		{
		pgm = argv[0];
			//magic. dont touch.
			if(strcmp(argv[i], "|") ==0) {
				argv[i] = NULL;
				fflush(NULL);
				pid_t pid = fork();
				if(pid < 0) perror("./ish");
				else if( pid == 0){
					// in child
					dup2(pipefd[1], 1);
					close(pipefd[0]);
					if(execvp(pgm,  argv))
					{
						perror("./ish");
						return FALSE;
					}
				}
				//in parent
				else {
					child_pid = pid;
					pid = wait(NULL);
					close(pipefd[1]);
					dup2(pipefd[0], 0);
					for(int j =0; j<=i; j++) argv[j]=NULL;
					i=-1;
				}
				close(pipefd[0]); close(pipefd[1]);
			}
			
			else if(strcmp(argv[i], "<") ==0) {
				argv[i] = NULL;
				argv[i+1] = (((struct Token*) DynArray_get(oTokens, count)))->pcValue;
				if ((fd = open(argv[i+1], O_RDONLY)));
				else {
					fprintf(stderr, "./ish: No such file or directory\n");
					return FALSE;
				}
				fflush(NULL);
				pid_t pid = fork();
				if(pid < 0) perror("./ish");
				else if( pid == 0){
					// in child
					close(0);
					dup(fd);
					if(execvp(pgm,  argv))
					{
						perror("./ish");
						return FALSE;
					}
					exit(0);
				}
				//in parent
				else {
					child_pid = pid;
					pid = wait(NULL);
					close(fd);
					dup2(0,0);
					for(int j =0; j<=i+1; j++) argv[j]=NULL;
					count++;
					i=-1;
				}
			}
		}
	}
	argv[i+1] = NULL;
	pgm = argv[0];
	fflush(NULL);
	pid_t pid = fork();
	//if(pid < 0) perror("./ish");
	if( pid == 0){
		//printf("in child\n\n");
		if(execvp(pgm,  argv))
		{
			perror("./ish");
			return FALSE;
		}
	}
	else if(pid >0) {
		//printf("in parent");
		child_pid = pid;
		wait(&status);
	}
	return TRUE;
}
