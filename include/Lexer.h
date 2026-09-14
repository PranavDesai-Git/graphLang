#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

void initLexer(const char *source);
Token scanToken(void);

#endif
