#include "3cc.h"

Node* code[100];
Vector* tokens;
Map* vars;
int pos;
int if_counter = 0;
int while_counter = 0;
int for_counter = 0;

int is_alnum(char c) {
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9') ||
            c == '_');
}

int is_al(char c) {
    return (('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
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

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_IF;
            token->input = p;
            token->tk_ident = if_counter++;
            vec_push(tokens, token);
            p += 2;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_FOR;
            token->input = p;
            token->tk_ident = for_counter++;
            vec_push(tokens, token);
            p += 3;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_ELSE;
            token->input = p;
            vec_push(tokens, token);
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
            Token *token = malloc(sizeof(Token));
            token->ty = TK_WHILE;
            token->input = p;
            token->tk_ident = while_counter++;
            vec_push(tokens, token);
            p += 5;
            continue;
        }

        if (is_al(p[0])) {
            int token_len = 1;
            while (is_alnum(p[token_len])) {
                token_len++;
            }
            Token *token = malloc(sizeof(Token));
            token->ty = TK_IDENT;
            token->input = p;
            token->name = strndup(p, token_len);
            vec_push(tokens, token);
            p += token_len;
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
            || *p == ';' || *p == '='
            || *p == '{' || *p == '}') {
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

Node *new_node_ident(char *name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

Node *new_node_funcall(char *name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FUNCALL;
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

    if (((Token*) tokens->data[pos])->ty == TK_IDENT) {        
        if (((Token*) tokens->data[pos + 1])->ty == '(') { // funcall
            if (((Token*) tokens->data[pos + 2])->ty == ')') {
                int fpos = pos;
                pos = pos + 3;
                return new_node_funcall(((Token*) tokens->data[fpos])->name);
            } else {
                error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
            }
        } else if (!map_get(vars, ((Token*) tokens->data[pos])->name)) { // var
            map_put(vars, ((Token*) tokens->data[pos])->name, new_int(8 * (vars->keys->len + 1)));
        }
        return new_node_ident(((Token*) tokens->data[pos++])->name);
    }
    
    error("項が期待されますが、項ではありません: %s", ((Token*) tokens->data[pos])->input);
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

    if (consume('{')) {
        node = malloc(sizeof(Node));
        node->ty = ND_BLOCK;
        Vector *stmts = new_vector();
        while (!consume('}')) {
            if (consume(TK_EOF))
                error("ブロックの括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
            vec_push(stmts, stmt());
        }
        node->stmts = stmts;
        return node;
    } else if (consume(TK_RETURN)) {
        node = malloc(sizeof(Node));
        node->ty = ND_RETURN;
        node->lhs = expr();
    } else if (consume(TK_IF)) {
        node = malloc(sizeof(Node));
        node->ty = ND_IF;
        node->nd_ident = ((Token*) tokens->data[pos - 1])->tk_ident;
        if (consume('(')) {
            node->lhs = expr();
            if (!consume(')'))
                error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
            Node* then = stmt();
            if (consume(TK_ELSE)) {
                Node* elseNode = malloc(sizeof(Node));
                elseNode->ty = ND_ELSE;
                elseNode->lhs = then;
                elseNode->rhs = stmt();
                node->rhs = elseNode;
            } else {
                node->rhs = then;
            }
            return node;
        } else {
            error("条件部の括弧がありません: %s", ((Token*) tokens->data[pos])->input);
        }
    } else if (consume(TK_FOR)) {
        node = malloc(sizeof(Node));
        node->ty = ND_FOR;
        node->nd_ident = ((Token*) tokens->data[pos - 1])->tk_ident;
        Node* initNode = malloc(sizeof(Node));
        initNode->ty = ND_INIT;
        initNode->lhs = NULL;
        Node* condNode = malloc(sizeof(Node));
        condNode->ty = ND_COND;
        condNode->lhs = NULL;
        condNode->rhs = NULL;
        initNode->rhs = condNode;
        node->lhs = initNode;
        if (!consume('('))
            error("forの括弧がありません: %s", ((Token*) tokens->data[pos])->input);
        if (!consume(';')) { // 初期化節
            initNode->lhs = expr();
            if (!consume(';'))
                error("';'ではないトークンです: %s", ((Token*) tokens->data[pos])->input);
        }
        if (!consume(';')) { // 条件部
            condNode->lhs = expr();
            if (!consume(';'))
                error("';'ではないトークンです: %s", ((Token*) tokens->data[pos])->input);
        }
        if (!consume(')')) { // 更新部
            condNode->rhs = expr();
            if (!consume(')'))
                error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
        }
        node->rhs = stmt();
        return node;
    } else if (consume(TK_WHILE)) {
        node = malloc(sizeof(Node));
        node->ty = ND_WHILE;
        node->nd_ident = ((Token*) tokens->data[pos - 1])->tk_ident;
        if (consume('(')) {
            node->lhs = expr();
            if (!consume(')'))
                error("括弧の対応が取れません: %s", ((Token*) tokens->data[pos])->input);
            node->rhs = stmt();
            return node;
        } else {
            error("条件部の括弧がありません: %s", ((Token*) tokens->data[pos])->input);
        }
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
    vars = new_map();
    pos = 0;
    program();
}
