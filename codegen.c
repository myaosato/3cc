#include "3cc.h"

void gen_lval(Node *node) {
    if (node->ty != ND_IDENT)
        error("代入の左辺値が変数ではありません");
    int offset = ((Int *) map_get(vars, node->name))->num;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
}

void gen(Node* node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_FUNCALL) {
        printf("  call %s\n", node->name);
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_IF) {
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        if (node->rhs->ty == ND_ELSE) {
            printf("  je  .Lelse%d\n", node->nd_ident);
            gen(node->rhs->lhs);
            printf("  jmp  .LendIF%d\n", node->nd_ident);
            printf(".Lelse%d:\n", node->nd_ident);
            gen(node->rhs->rhs);
        } else {
            printf("  push 0\n");
            printf("  je  .LendIF%d\n", node->nd_ident);
            printf("  pop rax\n");
            gen(node->rhs);
        }
        printf(".LendIF%d:\n", node->nd_ident);
        return;
    }

    if (node->ty == ND_WHILE) {
        printf("  push 0\n");
        printf(".LbeginWHILE%d:\n", node->nd_ident);
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .LendWHILE%d\n", node->nd_ident);
        printf("  pop rax\n");
        gen(node->rhs);
        printf("  jmp  .LbeginWHILE%d\n", node->nd_ident);
        printf(".LendWHILE%d:\n", node->nd_ident);
        return;
    }

    if (node->ty == ND_FOR) {
        printf("  push 0\n");
        if (node->lhs->lhs) {// compile init
            gen(node->lhs->lhs);
            printf("  pop rax\n");
        }
        printf(".LbeginFOR%d:\n", node->nd_ident);
        if (node->lhs->rhs->lhs) {// compile cond
            gen(node->lhs->rhs->lhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .LendFOR%d\n", node->nd_ident);
        }
        printf("  pop rax\n");
        gen(node->rhs);
        if (node->lhs->rhs->rhs) {// compile update
            gen(node->lhs->rhs->rhs);
            printf("  pop rax\n");
        }
        printf("  jmp  .LbeginFOR%d\n", node->nd_ident);
        printf(".LendFOR%d:\n", node->nd_ident);
        return;
    }

    if (node->ty == ND_BLOCK) {
        for (int i = 0; i < node->stmts->len; i++) {
            if (i != 0)
                printf("  pop rax\n");
            gen((Node *)node->stmts->data[i]);
        }
        return;
    }

    if (node->ty == ND_RETURN) {
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    }

    if (node->ty == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
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

void gen_main() {
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", vars->keys->len * 8);
    if (vars->keys->len % 2) {
        printf("  push 0\n");
    }

    for (int i = 0; code[i]; i++) {
        gen(code[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");

    printf("  ret\n");
}