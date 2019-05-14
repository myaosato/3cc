#include "3cc.h"

Node* code[100];
Vector* tokens;
int pos;

int is_alnum(char c) {
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            c == '_');
}

void *tokenize(char *p) {
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_RETURN;
            token->input = p;
            vec_push(tokens, token);
            p += 6;
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

        if ('a' <= *p && *p <= 'z') {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_IDENT;
            token->input = p;
            vec_push(tokens, token);
            p++;
            continue;
        }


        if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
            || *p == '(' || *p == ')'
            || *p == '<' || *p == '>'
            || *p == ';' || *p == '=') {
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

Node *new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}


int consume(int ty) {
    if (((Token*) tokens->data[pos])->ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *add();
Node *equality();

Node *term() {
    if (consume('(')) {
        Node *node = equality();
        if (!consume(')'))
            error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
        return node;
    }

    if (((Token*) tokens->data[pos])->ty == TK_NUM)
        return new_node_num(((Token*) tokens->data[pos++])->val);

    if (((Token*) tokens->data[pos])->ty == TK_IDENT)
        return new_node_ident(((Token*) tokens->data[pos++])->input[0]);

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

Node *assign() {
    Node *node = equality();
    if (consume('='))
        node = new_node('=', node, assign());
    return node;
}

Node *expr() {
    return assign();
}

Node *stmt() {
    Node *node;

    if (consume(TK_RETURN)) {
        node = malloc(sizeof(Node));
        node->ty = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }


    if (!consume(';'))
        error("';'ではないトークンです: %s", ((Token*) tokens->data[pos])->input);
    return node;
}

void program() {
    int i = 0;
    while (((Token*) tokens->data[pos])->ty != TK_EOF)
        code[i++] = stmt();
    code[i] = NULL;
}


void parse(char *codestr) {
    tokens = new_vector();
    tokenize(codestr);
    pos = 0;
    program();
}
