#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(char *fmt, ...);

typedef struct {
    int num;
} Int;

Int *new_int(int in);

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void test_vec();

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void test_map();

typedef struct {
    int ty;
    int val;
    char *name;
    char *input;
    int tk_ident;
} Token;

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_RETURN,
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
    char *name;
    int nd_ident;
    Vector *stmts;
    Vector *params;
} Node;

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_RETURN,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
    ND_FOR,
    ND_INIT,
    ND_COND,
    ND_BLOCK,
    ND_FUNCALL,
    ND_EQ,
    ND_NE,
    ND_LE,
};

extern Vector* tokens;
extern int pos;
extern Node *code[];
extern Map* vars;
void parse();


void gen_main();
