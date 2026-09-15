#include "Lexer.h"
#include <ctype.h>
#include <string.h>

static struct {
    const char *start;
    const char *current;
    int line;
} scanner;

void initLexer(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static int isAtEnd(void) { return *scanner.current == '\0'; }

static char advance(void) {
    scanner.current++;
    return scanner.current[-1];
}

static char peek(void) { return *scanner.current; }

static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

static Token errorToken(const char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace(void) {
    for (;;) {
        char c = peek();
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case ';':
            while (peek() != '\n' && !isAtEnd())
                advance();
            break;
        default:
            return;
        }
    }
}

static Token number(void) {
    while (isdigit(peek()))
        advance();
    return makeToken(TOKEN_NUMBER);
}

static int isIdentifierChar(char c) {
    return isalnum(c) || c == '_' || c == '+' || c == '-' || c == '*' ||
           c == '/' || c == '<' || c == '>' || c == '=' || c == '?' ||
           c == '!' || c == '&' || c == '|' || c == '.';
}

static int isIdentifierStart(char c) {
    return isalpha(c) || c == '_' || c == '+' || c == '-' || c == '*' ||
           c == '/' || c == '<' || c == '>' || c == '=' || c == '?' ||
           c == '!' || c == '&' || c == '|';
}

static Token identifier(void) {
    while (isIdentifierChar(peek())) {
        advance();
    }
    return makeToken(TOKEN_IDENTIFIER);
}

Token scanToken(void) {
    skipWhitespace();
    scanner.start = scanner.current;

    if (isAtEnd())
        return makeToken(TOKEN_EOF);

    char c = advance();

    if (isdigit(c))
        return number();
    if (isIdentifierStart(c)) {
        return identifier();
    }

    switch (c) {
    case '(':
        return makeToken(TOKEN_LPAREN);
    case ')':
        return makeToken(TOKEN_RPAREN);
    }

    return errorToken("Unexpected character.");
}
