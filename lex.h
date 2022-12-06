// lex.h
// Full name: Minh Quang Nguyen
// Student ID: 20190723

enum {MAX_LINE_SIZE = 1024};
enum {FALSE, TRUE};
enum TokenType {TOKEN_WORD, TOKEN_CHAR};

struct Token
{
   enum TokenType eType;
   char *pcValue;
};

void freeToken(void *pvItem, void *pvExtra);
int SynAna(char* acLine, DynArray_T oTokens);
