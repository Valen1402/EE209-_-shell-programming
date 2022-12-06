/*--------------------------------------------------------------------*/
/* buildin.c                                                              */
/* Full name : Minh Quang Nguyen                                      */
/* Student ID: 20190723															     */
/*--------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 200112L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include "dynarray.h"
#include "lex.h"

int BuildIn(DynArray_T oTokens) {
	int len = DynArray_getLength(oTokens);
   	if(!len) return TRUE;
   	
   	char* ch = ((struct Token*) DynArray_get(oTokens, 0))->pcValue;
   	
   	if(strcmp(ch, "setenv")==0) {
   		if(len==1 || len >= 4 || ((struct Token*) DynArray_get(oTokens, 1))->eType ||
   			(len==3 &&((struct Token*) DynArray_get(oTokens, 2))->eType) )
   		{
   			fprintf(stderr, "./ish: setenv takes one or two parameters\n");
      		return FALSE;
      	}
      	char* ar2, *ar1 = ((struct Token*) DynArray_get(oTokens, 1))->pcValue;
      	if(len==3) ar2 = ((struct Token*) DynArray_get(oTokens, 2))->pcValue;
      	else ar2 = "";
      	if (setenv(ar1, ar2, 1))
      	{
   			perror("./ish");
      		return FALSE;
      	}
   	}
   	
   	else if(strcmp(ch, "unsetenv")==0) {
   		if(len!=2 || ((struct Token*) DynArray_get(oTokens, 1))->eType)
   		{
   			fprintf(stderr, "./ish: unsetenv takes one paremeters\n");
      		return FALSE;
      	}
      	char*ar1 = ((struct Token*) DynArray_get(oTokens, 1))->pcValue;
      	if (unsetenv(ar1))
      	{
   			perror("./ish");
      		return FALSE;
      	}
   	}
   	
   	else if(strcmp(ch, "cd")==0) {
   		if(len>2 || (len==2 && ((struct Token*) DynArray_get(oTokens, 1))->eType))
   		{
   			fprintf(stderr, "./ish: cd takes one paremeters\n");
      		return FALSE;
      	}
      	char* ar1;
      	if(len==1) ar1 = getenv("HOME");
      	else ar1 =  ((struct Token*) DynArray_get(oTokens, 1))->pcValue;
      	if (chdir(ar1))
      	{
   			perror("./ish");
      		return FALSE;
      	}
      }
	return TRUE;
}
