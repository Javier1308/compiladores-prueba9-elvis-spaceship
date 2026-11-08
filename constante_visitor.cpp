#include <climits>
#include "ast.h"
#include "constante_visitor.h"

using namespace std;

// base ** exponente como lo calcula el código generado: 1 si el exponente es <= 0
static long long potencia(long long base, long long exponente) {
    long long resultado = 1;
    long long b = base;
    while (exponente > 0) {
        if (exponente & 1) resultado *= b;
        b *= b;
        exponente >>= 1;
    }
    return resultado;
}

// Marca la expresión como constante con el valor dado
void ConstanteVisitor::constante(Exp* exp, long long valor) {
    exp->esConstante = true;
    exp->valorConstante = valor;
}

// Marca la expresión como no constante (su valor solo se conoce al ejecutar)
void ConstanteVisitor::noConstante(Exp* exp) {
    exp->esConstante = false;
    exp->valorConstante = 0;
}

// Marca las expresiones de todo el programa
void ConstanteVisitor::marcar(Programa* program) {
    program->accept(this);
}

// ---------- Expresiones ----------
// El Type que devuelven no se usa: el resultado queda en los atributos de cada Exp

// left op right es constante si ambos operandos lo son.
// No se pliega una división entre 0 (ni LLONG_MIN / -1): se deja que falle al ejecutar
Type ConstanteVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    exp->right->accept(this);
    noConstante(exp);
    if (!exp->left->esConstante || !exp->right->esConstante) return Type();
    long long a = exp->left->valorConstante;
    long long b = exp->right->valorConstante;
    switch (exp->op) {
        case PLUS_OP:  constante(exp, a + b); break;
        case MINUS_OP: constante(exp, a - b); break;
        case MUL_OP:   constante(exp, a * b); break;
        case DIV_OP:
            if (b != 0 && !(a == LLONG_MIN && b == -1)) constante(exp, a / b);
            break;
        case POW_OP:   constante(exp, potencia(a, b)); break;
        case LE_OP:    constante(exp, a <= b); break;
        case LT_OP:    constante(exp, a < b);  break;
        case GT_OP:    constante(exp, a > b);  break;
        case GE_OP:    constante(exp, a >= b); break;
        case EQ_OP:    constante(exp, a == b); break;
        case NE_OP:    constante(exp, a != b); break;
        default:       break;
    }
    return Type();
}

// Un literal (número, true o false) siempre es constante
Type ConstanteVisitor::visit(NumberExp* exp) {
    constante(exp, exp->value);
    return Type();
}

// Una variable no es constante: su valor depende del flujo del programa
Type ConstanteVisitor::visit(IdExp* exp) {
    noConstante(exp);
    return Type();
}

// Una llamada no es constante, pero sus argumentos pueden serlo
Type ConstanteVisitor::visit(FCallExp* exp) {
    for (Exp* arg : exp->argumentos) arg->accept(this);
    noConstante(exp);
    return Type();
}

// a[i]: lee memoria, no es constante (el índice sí puede serlo)
Type ConstanteVisitor::visit(IndexExp* exp) {
    exp->base->accept(this);
    exp->indice->accept(this);
    noConstante(exp);
    return Type();
}

// *p: lee memoria, no es constante
Type ConstanteVisitor::visit(DerefExp* exp) {
    exp->puntero->accept(this);
    noConstante(exp);
    return Type();
}

// &x: una dirección solo se conoce al ejecutar
Type ConstanteVisitor::visit(AddrExp* exp) {
    exp->lvalue->accept(this);
    noConstante(exp);
    return Type();
}

// malloc(n): la dirección no es constante (n sí puede serlo)
Type ConstanteVisitor::visit(MallocExp* exp) {
    exp->bytes->accept(this);
    noConstante(exp);
    return Type();
}

// Métodos de pila: dependen del contenido de la pila, no son constantes
Type ConstanteVisitor::visit(StackOpExp* exp) {
    exp->pila->accept(this);
    if (exp->valor) exp->valor->accept(this);
    noConstante(exp);
    return Type();
}

Type ConstanteVisitor::visit(SpaceshipExp* exp) {
    exp->left->accept(this);
    exp->right->accept(this);
    noConstante(exp);
    if (!exp->left->esConstante || !exp->right->esConstante) return Type();
    long long a = exp->left->valorConstante;
    long long b = exp->right->valorConstante;
    constante(exp, (a > b) - (a < b));
    return Type();
}

Type ConstanteVisitor::visit(ElvisExp* exp) {
    exp->left->accept(this);
    exp->right->accept(this);
    noConstante(exp);
    if (!exp->left->esConstante) return Type();
    if (exp->left->valorConstante != 0) constante(exp, exp->left->valorConstante);
    else if (exp->right->esConstante) constante(exp, exp->right->valorConstante);
    return Type();
}

// ---------- Sentencias ----------
// Solo recorren sus expresiones

void ConstanteVisitor::visit(AssignStmt* stm) {
    stm->e->accept(this);
}

void ConstanteVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
}

void ConstanteVisitor::visit(StoreStmt* stm) {
    stm->destino->accept(this);
    stm->e->accept(this);
}

void ConstanteVisitor::visit(FreeStmt* stm) {
    stm->e->accept(this);
}

void ConstanteVisitor::visit(PrintStmt* stm) {
    stm->e->accept(this);
}

void ConstanteVisitor::visit(IfStmt* stm) {
    stm->condicion->accept(this);
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
}

void ConstanteVisitor::visit(WhileStmt* stm) {
    stm->condicion->accept(this);
    for (Stmt* s : stm->body) s->accept(this);
}

void ConstanteVisitor::visit(DoWhileStmt* stm) {
    for (Stmt* s : stm->body) s->accept(this);
    stm->condicion->accept(this);
}

void ConstanteVisitor::visit(BreakStmt* stm) {}

void ConstanteVisitor::visit(ReturnStmt* stm) {
    stm->exp->accept(this);
}

// ---------- Declaraciones y programa ----------

// Una declaración no tiene expresiones
void ConstanteVisitor::visit(VarDec* dec) {}

void ConstanteVisitor::visit(Body* body) {
    for (Stmt* s : body->sentencias) s->accept(this);
}

void ConstanteVisitor::visit(FunDec* fun) {
    fun->cuerpo->accept(this);
}

void ConstanteVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) f->accept(this);
}
