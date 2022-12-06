/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Full name : Minh Quang Nguyen                                      */
/* Student ID: 20190723															     */
/*--------------------------------------------------------------------*/

#define _GNU_SOURCE

#include "dynarray.h"
#include "lex.h"
#include "processhandler.h"
#include "buildin.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#define FILENAME "/.ishrc"

int proc_line(char* acLine){
	DynArray_T oTokens;
   int iSuccessful;
   
	oTokens = DynArray_new(0);
	if (oTokens == NULL)
      {
         fprintf(stderr, "./ish: Cannot allocate memory\n");
         return(FALSE);
      }
   iSuccessful = SynAna(acLine, oTokens);
   
   if(iSuccessful) {
   	if(DynArray_getLength(oTokens) == 0) return TRUE;  	
   	char* ch = ((struct Token*) DynArray_get(oTokens, 0))->pcValue;
   	//if (ch == '\0') return TRUE;
      if(!strcmp(ch, "setenv") || !strcmp(ch, "unsetenv") ||
      				!strcmp(ch, "cd") || !strcmp(ch, "exit")) 
      	BuildIn(oTokens);
      //else if( alias
		else ProcessHandler(oTokens);
		
		DynArray_map(oTokens, freeToken, NULL);
		DynArray_free(oTokens);
		return TRUE;
   }
	else return FALSE;
}

int main(void) 
{
   char acLine[MAX_LINE_SIZE];
   char filename[MAX_LINE_SIZE] = "";
   sprintf(filename, "%s%s", getenv("HOME"), FILENAME);
	FILE *fp;
	fp = fopen(filename, "r");
	if(!fp) {
		fprintf(stderr, "./ish: Can't open file\n");
		return EXIT_FAILURE;
	}
	const char c = '%';

	void (*pfRet) (int);
	pfRet = signal(SIGINT, INT);
	assert(pfRet != SIG_ERR);
/*	
	pfRet = signal(SIGQUIT, QUIT);
	assert(pfRet != SIG_ERR);
	alarm(5);
	alarm(0);
	pfRet = signal(SIGQUIT, QUITALL);
	assert(pfRet != SIG_ERR);
*/	
 	while (fgets(acLine, MAX_LINE_SIZE, fp))
   {	
   	printf("%c %s", c, acLine);
		proc_line(acLine);
   }
   printf("%c ", c);
   for (int i=0; i< 1024; i++) acLine[i] = '\0';

	while (fgets(acLine, MAX_LINE_SIZE, stdin))
   {
   	proc_line(acLine);
   	printf("%c ",c);
   }
   return EXIT_SUCCESS;
}
