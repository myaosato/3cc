#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(char *fmt, ...);

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void runtest();

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

enum {
    TK_NUM = 256,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_EOF,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

enum {
    ND_NUM = 256,
    ND_EQ,
    ND_NE,
    ND_LE,
};

extern Vector* tokens;
extern int pos;
Node *parse();

void gen(Node* node);
