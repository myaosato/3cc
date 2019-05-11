#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}

int expect(int line, int expected, int actual) {
    if (expected == actual)
        return 1;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void runtest() {
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);
    
    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (long)vec->data[0]);
    expect(__LINE__, 50, (long)vec->data[50]);
    expect(__LINE__, 99, (long)vec->data[99]);

    printf("OK\n");
}

enum {
    TK_NUM = 256,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Vector* tokens;
int pos;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void *tokenize(char *p) {
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_EQ;
            token->input = p;
            vec_push(tokens, token);
            p++;
            p++;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_NE;
            token->input = p;
            vec_push(tokens, token);
            p++;
            p++;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_LE;
            token->input = p;
            vec_push(tokens, token);
            p++;
            p++;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_GE;
            token->input = p;
            vec_push(tokens, token);
            p++;
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
            || *p == '(' || *p == ')'
            || *p == '<' || *p == '>') {
            Token *token = malloc(sizeof(Token));
            token->ty = *p;
            token->input = p;
            vec_push(tokens, token);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_NUM;
            token->input = p;
            token->val = strtol(p, &p, 10);
            vec_push(tokens, token);
            continue;
        }

        error("トークナイズできません: %s", p);
        exit(1);
    }
    Token *token = malloc(sizeof(Token));
    token->ty = TK_EOF;
    token->input = p;
    vec_push(tokens, token);
}

enum {
    ND_NUM = 256,
    ND_EQ,
    ND_NE,
    ND_LE,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

int consume(int ty) {
    if (((Token*) tokens->data[pos])->ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *add();

Node *term() {
    if (consume('(')) {
        Node *node = add();
        if (!consume(')'))
            error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
        return node;
    }

    if (((Token*) tokens->data[pos])->ty == TK_NUM)
        return new_node_num(((Token*) tokens->data[pos++])->val);
    error("数値が期待されますが、数値ではありません: %s", ((Token*) tokens->data[pos])->input);
}

Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

Node *mul() {
    Node *node = unary();

    for(;;) {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for(;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for(;;) {
        if (consume('<'))
            node = new_node('<', node, add());
        if (consume('>'))
            node = new_node('<', add(), node);
        if (consume(TK_LE))
            node = new_node(ND_LE, node, add());
        if (consume(TK_GE))
            node = new_node(ND_LE, add(), node);
        else
            return node;
    }
}

Node *equality() {
    Node *node = relational();
    for(;;) {
        if (consume(TK_EQ))
            node = new_node(ND_EQ, node, relational());
        if (consume(TK_NE))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *parse() {
    return equality();
}

void gen(Node* node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
        break;
    case '<':
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    if (strncmp(argv[1], "-test", 5) == 0) {
        runtest();
        return 0;
    }

    tokens = new_vector();
    pos = 0;

    tokenize(argv[1]);
    Node *node = parse();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
