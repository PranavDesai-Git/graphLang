#include "Parser.h"
#include "Environment.h"
#include "Evaluator.h"
#include "GarbageCollector.h"
#include "Lexer.h"
#include "TreeNode.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern jmp_buf error_jmp;

static Token currentToken;

typedef struct CompilerScope {
    struct CompilerScope *parent;
    Token locals[100];
    int localCount;
} CompilerScope;

static void advance(void) {
    currentToken = scanToken();
    if (currentToken.type == TOKEN_ERROR) {
        printf("Syntax err on line %d: %.*s \n", currentToken.line,
               currentToken.length, currentToken.start);
        longjmp(error_jmp, 1);
    }
}

static void consume(TokenType type, const char *errorMessage) {
    if (currentToken.type == type) {
        advance();
        return;
    }

    printf("Parse Error on line %d: %s\n", currentToken.line, errorMessage);
    longjmp(error_jmp, 1);
}

static Node *resolveVariable(CompilerScope *scope, Token name) {
    int depth = 0;
    CompilerScope *current = scope;
    while (current != NULL) {
        for (int i = current->localCount - 1; i >= 0; i--) {
            Token local = current->locals[i];
            if (local.length == name.length &&
                memcmp(local.start, name.start, name.length) == 0) {
                return createLocalVar(depth, i);
            }
        }
        current = current->parent;
        depth++;
    }

    char *globalName = malloc(name.length + 1);
    memcpy(globalName, name.start, name.length);
    globalName[name.length] = '\0';

    return createGlobalVar(globalName);
}

static Node *parseExpression(CompilerScope *scope) {
    if (currentToken.type == TOKEN_EOF) {
        return NULL;
    }

    if (currentToken.type == TOKEN_NUMBER) {
        int val = atoi(currentToken.start);
        advance();
        return createLiteral(val);
    }

    if (currentToken.type == TOKEN_IDENTIFIER) {
        Node *varNode = resolveVariable(scope, currentToken);
        advance();
        return varNode;
    }

    if (currentToken.type == TOKEN_LPAREN) {
        advance();

        if (currentToken.type == TOKEN_IDENTIFIER && currentToken.length == 6 &&
            memcmp(currentToken.start, "define", 6) == 0) {

            advance();

            Token nameToken = currentToken;
            consume(TOKEN_IDENTIFIER, "Expected function name");

            consume(TOKEN_LPAREN, "Expected '(' before parameters");
            CompilerScope funcScope = {.parent = scope, .localCount = 0};

            Node *paramsList = createCons(0, NULL);
            pushRoot(paramsList);
            Node *currParam = paramsList;

            while (currentToken.type != TOKEN_RPAREN) {
                funcScope.locals[funcScope.localCount++] = currentToken;

                char *pName = malloc(currentToken.length + 1);
                memcpy(pName, currentToken.start, currentToken.length);
                pName[currentToken.length] = '\0';

                setLeft(currParam, createGlobalVar(pName));
                advance();

                if (currentToken.type != TOKEN_RPAREN) {
                    setRight(currParam, createCons(0, NULL));
                    currParam = getRight(currParam);
                }
            }
            consume(TOKEN_RPAREN, "Expected ')' after parameters");

            Node *body = parseExpression(&funcScope);
            pushRoot(body);
            consume(TOKEN_RPAREN, "Expected ')' at end of define");

            char *funcName = malloc(nameToken.length + 1);
            memcpy(funcName, nameToken.start, nameToken.length);
            funcName[nameToken.length] = '\0';

            Node *lambda = createLambda(paramsList, body);
            popRoot(); // body
            pushRoot(lambda);
            Node *defNode = createDefine(funcName, lambda);
            popRoot(); // lambda
            popRoot(); // paramsList
            return defNode;
        }

        if (currentToken.type == TOKEN_IDENTIFIER && currentToken.length == 6 &&
            memcmp(currentToken.start, "lambda", 6) == 0) {

            advance();
            consume(TOKEN_LPAREN, "Expected '(' before parameters");
            CompilerScope funcScope = {.parent = scope, .localCount = 0};

            Node *paramsList = createCons(0, NULL);
            pushRoot(paramsList);
            Node *currParam = paramsList;

            while (currentToken.type != TOKEN_RPAREN) {
                funcScope.locals[funcScope.localCount++] = currentToken;

                char *pName = malloc(currentToken.length + 1);
                memcpy(pName, currentToken.start, currentToken.length);
                pName[currentToken.length] = '\0';

                setLeft(currParam, createGlobalVar(pName));
                advance();

                if (currentToken.type != TOKEN_RPAREN) {
                    setRight(currParam, createCons(0, NULL));
                    currParam = getRight(currParam);
                }
            }
            consume(TOKEN_RPAREN, "Expected ')' after parameters");

            Node *body = parseExpression(&funcScope);
            pushRoot(body);
            consume(TOKEN_RPAREN, "Expected ')' at end of lambda");

            Node *lambda = createLambda(paramsList, body);
            popRoot(); // body
            popRoot(); // paramsList
            return lambda;
        }
        if (currentToken.type == TOKEN_IDENTIFIER && currentToken.length == 3 &&
            memcmp(currentToken.start, "let", 3) == 0) {

            advance();
            consume(TOKEN_LPAREN, "Expected '(' before let bindings");
            CompilerScope funcScope = {.parent = scope, .localCount = 0};

            Node *paramsList = createCons(0, NULL);
            pushRoot(paramsList);
            Node *currParam = paramsList;

            Node *argsList = createCons(0, NULL);
            pushRoot(argsList);
            Node *currArg = argsList;

            while (currentToken.type != TOKEN_RPAREN) {
                consume(TOKEN_LPAREN, "Expected '(' for let binding");
                
                Token varName = currentToken;
                consume(TOKEN_IDENTIFIER, "Expected variable name in let binding");
                
                funcScope.locals[funcScope.localCount++] = varName;
                
                char *pName = malloc(varName.length + 1);
                memcpy(pName, varName.start, varName.length);
                pName[varName.length] = '\0';
                
                setLeft(currParam, createGlobalVar(pName));

                Node *valExpr = parseExpression(scope);
                setLeft(currArg, valExpr);

                consume(TOKEN_RPAREN, "Expected ')' after let binding");

                if (currentToken.type != TOKEN_RPAREN) {
                    setRight(currParam, createCons(0, NULL));
                    currParam = getRight(currParam);

                    setRight(currArg, createCons(0, NULL));
                    currArg = getRight(currArg);
                }
            }
            consume(TOKEN_RPAREN, "Expected ')' after let bindings");

            Node *body = parseExpression(&funcScope);
            pushRoot(body);
            consume(TOKEN_RPAREN, "Expected ')' at end of let");

            Node *lambda = createLambda(paramsList, body);
            popRoot(); // body
            pushRoot(lambda);

            Node *call = createCall(lambda, argsList);
            popRoot(); // lambda
            popRoot(); // argsList
            popRoot(); // paramsList

            return call;
        }
        Node *funcNode = parseExpression(scope);
        pushRoot(funcNode);

        Node *argsList = createCons(0, NULL);
        pushRoot(argsList);
        Node *currArg = argsList;

        while (currentToken.type != TOKEN_RPAREN) {
            setLeft(currArg, parseExpression(scope));

            if (currentToken.type != TOKEN_RPAREN) {
                setRight(currArg, createCons(0, NULL));
                currArg = getRight(currArg);
            }
        }
        consume(TOKEN_RPAREN, "Expected ')' at end of function call");

        Node *result = createCall(funcNode, argsList);
        popRoot();
        popRoot();
        return result;
    }

    printf("Parse Error: Unexpected token! type=%d len=%d text=%.*s\n", 
           currentToken.type, currentToken.length, currentToken.length, currentToken.start);
    longjmp(error_jmp, 1);
}

void parse(const char *source) {
    initLexer(source);
    advance();
    while (currentToken.type != TOKEN_EOF) {
        Node *expr = parseExpression(NULL);
        if (expr != NULL) {
            printf("Evaluating expression...\n");

            // PROTECT THE PARSED AST FROM THE GC!
            pushRoot(expr);
            Node *result = evaluate(expr, NULL);
            popRoot();

            // Execute if it's an IO Action
            if (result != NULL && getNodeType(result) == FOREIGN &&
                getTypeID(result) == 100) {
                result = executeIO(result);
            }

            if (result != NULL && getNodeType(result) == LITERAL) {
                printf("Result: %d\n", getLiteral(result));
            } else if (result != NULL) {
                printf("DEBUG: Result NodeType = %d\n", getNodeType(result));
            }
        }
    }
}
