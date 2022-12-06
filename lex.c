/*--------------------------------------------------------------------*/
/* lex.c                                                              */
/* Full name : Minh Quang Nguyen                                      */
/* Student ID: 20190723															     */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include "lex.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/
/*
enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};

enum TokenType {TOKEN_WORD, TOKEN_CHAR};
*/
/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */
/*
struct Token
{
   enum TokenType eType;
   char *pcValue;
};
*/
/*--------------------------------------------------------------------*/

void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/
/*
static void printCharToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token*)pvItem;
   if (psToken->eType == TOKEN_CHAR)
      printf("%s\n", psToken->pcValue);
}

static void printWordToken(void *pvItem, void *pvExtra)

{
   struct Token *psToken = (struct Token*)pvItem;
   if (psToken->eType == TOKEN_WORD)
      printf("%s\n", psToken->pcValue);
}
*/
/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

static int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{
	//enum LexState {STATE_START, STATE_IN_WORD, STATE_ERROR, STATE_EXIT };
	enum LexState {STATE_START, STATE_IN_WORD};
   enum LexState eState = STATE_START;

   int iLineIndex = 0;
   int iValueIndex = 0;
   int in_quote = 0;
   char c;
   char acValue[MAX_LINE_SIZE];
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];

      switch (eState)
      {
         case STATE_START:
            if ((c == '\n') || (c == '\0')) {
            	if (in_quote) {
            		fprintf(stderr, "./ish: Could not find quote pair\n");
               	return FALSE;
            	}
               return TRUE;
            }
              
            else if (!in_quote & ((c == '<') || (c == '>') || (c == '|'))) {
            
               /* Create a CHAR token. */
               acValue[0] = c;
               acValue[1] = '\0';
               psToken = makeToken(TOKEN_CHAR, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               //iValueIndex = 0;
               //eState = STATE_START;
            }
            
            else if (c == '"') {
            	in_quote = ~in_quote;
            }

            else if (!isspace(c))
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
            break;

         case STATE_IN_WORD:
            if ((c == '\n') || (c == '\0'))
            {
            	if (in_quote) {
            		fprintf(stderr, "./ish: Could not find quote pair\n");
               	return FALSE;
            	}
            	
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               //iValueIndex = 0;
               return TRUE;
            }
            else if (!in_quote && (isspace(c) ))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               eState = STATE_START;
            }
            else if( c == '"') in_quote = ! in_quote;
            else if( !in_quote && ((c == '<') || (c == '>') || (c == '|'))) {
            	acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               
               acValue[0] = c; acValue[1]='\0';
               psToken = makeToken(TOKEN_CHAR, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "./ish: Cannot allocate memory\n");
                  return FALSE;
               }
               
               iValueIndex = 0;

               eState = STATE_START;
            }
            else
            {	
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
            break;

         default:
            assert(FALSE);
      }
   }
}

/*--------------------------------------------------------------------*/

int SynAna(char* acLine, DynArray_T oTokens)
/* Read a line from stdin, and write to stdout each number and word
   that it contains.  Repeat until EOF.  Return 0 iff successful. */

{
   //char acLine[MAX_LINE_SIZE];
   //DynArray_T oTokens;
   int iSuccessful;

   //printf("------------------------------------\n");
      iSuccessful = lexLine(acLine, oTokens);
      /*if (iSuccessful)
      {
      	 printf("Char:  ");
          DynArray_map(oTokens, printCharToken, NULL);
          printf("\n");
          
          printf("Token:  ");
          DynArray_map(oTokens, printWordToken, NULL);
          printf("\n");
      }*/
      //printf("------------------------------------\n");
   //   enum TokenType eType;    char *pcValue;
 	// (struct Token*) DynArray_get(oTokens, i)) ->eType
 		if(! iSuccessful) return FALSE;
		else if (iSuccessful)
		{
			int i, len = DynArray_getLength(oTokens);
			int in=0, out=0;
			for (i = 0; i<len; i++){
				if(((struct Token*) DynArray_get(oTokens, i))->eType == 1) {
					char* ch = ((struct Token*) DynArray_get(oTokens, i))->pcValue;
					if(i == 0) {
						fprintf (stderr, "./ish: Missing command name\n");
						DynArray_map(oTokens, freeToken, NULL);
      				DynArray_free(oTokens);
      				return FALSE;
					}
					else if((i == len-1) || ((struct Token*) DynArray_get(oTokens, i+1))->eType ==1)
					{	fprintf (stderr, "./ish: Pipe or redirection destination is not specified\n");
						DynArray_map(oTokens, freeToken, NULL);
      				DynArray_free(oTokens);
      				return FALSE;
					}	
					else if(strcmp(ch,"<")==0) {
						if(in) {
							fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
							DynArray_map(oTokens, freeToken, NULL);
      					DynArray_free(oTokens);
      					return FALSE;
      				}
						in = 1;
					}
					else if (strcmp(ch,">")==0)  {
						if(out) {
							fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
							DynArray_map(oTokens, freeToken, NULL);
      					DynArray_free(oTokens);
      					return FALSE;
      				}
						out = 1;
					}
					else if (strcmp(ch,"|")==0)  {
						if(out) {
							fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
							DynArray_map(oTokens, freeToken, NULL);
      					DynArray_free(oTokens);
      					return FALSE;
      				}
						in =1;
					}
				}
			}
		}
   return TRUE;
}
