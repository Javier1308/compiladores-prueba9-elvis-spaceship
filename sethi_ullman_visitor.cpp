#include <algorithm>
#include "ast.h"
#include "sethi_ullman_visitor.h"

using namespace std;

// Etiqueta todas las expresiones del programa
void SethiUllmanVisitor::etiquetar(Programa* program) {
    program->accept(this);
}

// Una hoja (número o variable) vale 0 si es el hijo derecho de un BinaryExp
// (se usa directo como operando, sin cargarla en un registro) y 1 en otro caso
void SethiUllmanVisitor::hoja(Exp* exp) {
    exp->etiqueta = 1;
    if (hijoDerecho) exp->etiqueta = 0;
    hijoDerecho = false;
}

// ---------- Expresiones ----------
// El Type que devuelven no se usa: el resultado queda en la etiqueta de cada Exp.
// Cada nodo que no es hoja apaga hijoDerecho antes de visitar a sus hijos

// left op right: max(l1, l2) si l1 != l2;  l1 + 1 si l1 == l2
Type SethiUllmanVisitor::visit(BinaryExp* exp) {
    hijoDerecho = false;
    exp->left->accept(this);
    hijoDerecho = true;
    exp->right->accept(this);
    int l1 = exp->left->etiqueta;
    int l2 = exp->right->etiqueta;
    if (l1 != l2) exp->etiqueta = max(l1, l2);
    else exp->etiqueta = l1 + 1;
    return Type();
}

// Un literal es una hoja
Type SethiUllmanVisitor::visit(NumberExp* exp) {
    hoja(exp);
    return Type();
}

// Una variable es una hoja
Type SethiUllmanVisitor::visit(IdExp* exp) {
    hoja(exp);
    return Type();
}

// f(a1, ..., an): la mayor etiqueta de los argumentos (el resultado vuelve en un registro)
Type SethiUllmanVisitor::visit(FCallExp* exp) {
    hijoDerecho = false;
    exp->etiqueta = 1;
    for (Exp* arg : exp->argumentos) {
        arg->accept(this);
        exp->etiqueta = max(exp->etiqueta, arg->etiqueta);
    }
    return Type();
}

// a[i]: la mayor etiqueta entre la base y el índice
Type SethiUllmanVisitor::visit(IndexExp* exp) {
    hijoDerecho = false;
    exp->base->accept(this);
    exp->indice->accept(this);
    exp->etiqueta = max(exp->base->etiqueta, exp->indice->etiqueta);
    return Type();
}

// *p: la etiqueta del puntero
Type SethiUllmanVisitor::visit(DerefExp* exp) {
    hijoDerecho = false;
    exp->puntero->accept(this);
    exp->etiqueta = exp->puntero->etiqueta;
    return Type();
}

// &x: la etiqueta del lvalue
Type SethiUllmanVisitor::visit(AddrExp* exp) {
    hijoDerecho = false;
    exp->lvalue->accept(this);
    exp->etiqueta = exp->lvalue->etiqueta;
    return Type();
}

// malloc(n): la etiqueta de n
Type SethiUllmanVisitor::visit(MallocExp* exp) {
    hijoDerecho = false;
    exp->bytes->accept(this);
    exp->etiqueta = exp->bytes->etiqueta;
    return Type();
}

// s.metodo(...): la mayor etiqueta entre la pila y el valor de push
Type SethiUllmanVisitor::visit(StackOpExp* exp) {
    hijoDerecho = false;
    exp->pila->accept(this);
    exp->etiqueta = exp->pila->etiqueta;
    if (exp->valor) {
        exp->valor->accept(this);
        exp->etiqueta = max(exp->etiqueta, exp->valor->etiqueta);
    }
    return Type();
}

// left <=> right: igual que un BinaryExp
Type SethiUllmanVisitor::visit(SpaceshipExp* exp) {
    hijoDerecho = false;
    exp->left->accept(this);
    hijoDerecho = true;
    exp->right->accept(this);
    int l1 = exp->left->etiqueta;
    int l2 = exp->right->etiqueta;
    if (l1 != l2) exp->etiqueta = max(l1, l2);
    else exp->etiqueta = l1 + 1;
    return Type();
}

// left ?: right: cada operando se evalúa por separado en %rax, así que vale la mayor etiqueta
Type SethiUllmanVisitor::visit(ElvisExp* exp) {
    hijoDerecho = false;
    exp->left->accept(this);
    hijoDerecho = false;
    exp->right->accept(this);
    exp->etiqueta = max(exp->left->etiqueta, exp->right->etiqueta);
    return Type();
}

// ---------- Sentencias ----------
// Solo recorren sus expresiones

void SethiUllmanVisitor::visit(AssignStmt* stm) {
    stm->e->accept(this);
}

void SethiUllmanVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
}

void SethiUllmanVisitor::visit(StoreStmt* stm) {
    stm->destino->accept(this);
    stm->e->accept(this);
}

void SethiUllmanVisitor::visit(FreeStmt* stm) {
    stm->e->accept(this);
}

void SethiUllmanVisitor::visit(PrintStmt* stm) {
    stm->e->accept(this);
}

void SethiUllmanVisitor::visit(IfStmt* stm) {
    stm->condicion->accept(this);
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
}

void SethiUllmanVisitor::visit(WhileStmt* stm) {
    stm->condicion->accept(this);
    for (Stmt* s : stm->body) s->accept(this);
}

void SethiUllmanVisitor::visit(DoWhileStmt* stm) {
    for (Stmt* s : stm->body) s->accept(this);
    stm->condicion->accept(this);
}

void SethiUllmanVisitor::visit(BreakStmt* stm) {}

void SethiUllmanVisitor::visit(ReturnStmt* stm) {
    stm->exp->accept(this);
}

// ---------- Declaraciones y programa ----------

// Una declaración no tiene expresiones
void SethiUllmanVisitor::visit(VarDec* dec) {}

void SethiUllmanVisitor::visit(Body* body) {
    for (Stmt* s : body->sentencias) s->accept(this);
}

void SethiUllmanVisitor::visit(FunDec* fun) {
    fun->cuerpo->accept(this);
}

void SethiUllmanVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) f->accept(this);
}
