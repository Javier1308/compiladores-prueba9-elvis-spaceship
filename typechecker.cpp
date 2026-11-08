#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "ast.h"
#include "typechecker.h"

using namespace std;

// Tipo (sin valor concreto) que corresponde a un nombre de tipo: int, bool, int*, int**, ...
static Type tipoDe(const string& nombre) {
    return Type::desdeNombre(nombre);
}

// Indica si la expresión designa una posición de memoria: x, a[i] o *p
static bool esLvalue(Exp* e) {
    return dynamic_cast<IdExp*>(e) || dynamic_cast<IndexExp*>(e) || dynamic_cast<DerefExp*>(e);
}

// =====================================================
// ==================== TypeChecker ====================
// =====================================================

// Lanza un error de tipos con el mensaje dado
static void error(const string& msg) {
    throw runtime_error(msg);
}

// Verifica todo el programa
void TypeChecker::check(Programa* program) {
    program->accept(this);
}

// Bytes que debe reservar la función en la pila: 8 por variable (int o bool),
// redondeado a múltiplo de 16 para mantener la pila alineada
int TypeChecker::espacioPila(const string& funcion) {
    int bytes = numVariables[funcion] * TAM_VARIABLE;
    return ((bytes + 15) / 16) * 16;
}

// Verifica que la condición de una sentencia sea bool
void TypeChecker::esperarBool(Exp* condicion, const string& donde) {
    Type c = condicion->accept(this);
    if (!c.isBool()) {
        error("la condición del " + donde + " debe ser bool, pero es " + c.kindName());
    }
}

// ---------- Expresiones ----------

// Operadores aritméticos: int op int -> int
// Orden: int (<= | < | > | >=) int -> bool
// Igualdad: a (== | !=) b con a y b del mismo tipo (int, bool o puntero) -> bool
// Aritmética de punteros: puntero + int y puntero - int -> puntero del mismo tipo
Type TypeChecker::visit(BinaryExp* exp) {
    Type l = exp->left->accept(this);
    Type r = exp->right->accept(this);
    string op = Exp::binopToChar(exp->op);
    if (exp->op == EQ_OP || exp->op == NE_OP) {
        if (l.isVoid() || (!l.aceptaValor(r) && !r.aceptaValor(l))) {
            error("el operador '" + op + "' necesita dos operandos del mismo tipo, pero recibió "
                  + l.kindName() + " " + op + " " + r.kindName());
        }
        return Type(false);                      // bool
    }
    if (l.isPtr() && r.isInt() && (exp->op == PLUS_OP || exp->op == MINUS_OP)) {
        return l;
    }
    if (!l.isInt() || !r.isInt()) {
        error("el operador '" + op + "' necesita operandos int, pero recibió "
              + l.kindName() + " " + op + " " + r.kindName());
    }
    switch (exp->op) {
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
            return Type(false);                  // bool
        default:
            return Type(0);                      // int
    }
}

// Un número es int; true / false son bool
Type TypeChecker::visit(NumberExp* exp) {
    if (exp->esBool) return Type(false);
    return Type(0);
}

// Una variable tiene el tipo con el que fue declarada
Type TypeChecker::visit(IdExp* exp) {
    if (!tipos.check(exp->value)) {
        error("variable no declarada: " + exp->value);
    }
    return tipos.lookup(exp->value);
}

// f(a1, ..., an): la función existe, recibe n argumentos y cada uno tiene el tipo de su parámetro
Type TypeChecker::visit(FCallExp* exp) {
    if (!funciones.count(exp->nombre)) {
        error("función no declarada: " + exp->nombre);
    }
    FunDec* f = funciones[exp->nombre];
    if (exp->argumentos.size() != f->parametros_id.size()) {
        error("la función " + f->nombre + " espera " + to_string(f->parametros_id.size())
              + " argumento(s), pero recibió " + to_string(exp->argumentos.size()));
    }
    for (size_t i = 0; i < exp->argumentos.size(); i++) {
        Type t = exp->argumentos[i]->accept(this);
        if (!tipoDe(f->parametros_tipos[i]).aceptaValor(t)) {
            error("el argumento " + to_string(i + 1) + " de " + f->nombre + " debe ser "
                  + f->parametros_tipos[i] + ", pero es " + t.kindName());
        }
    }
    return tipoDe(f->tipo);
}

// a[i]: a es un puntero (int*, int**, ...) e i un int; el resultado es el tipo apuntado
Type TypeChecker::visit(IndexExp* exp) {
    Type b = exp->base->accept(this);
    Type i = exp->indice->accept(this);
    if (!b.isPtr()) {
        error("solo se puede indexar un puntero, pero se indexó un " + b.kindName());
    }
    if (b.isVoidPtr()) {
        error("no se puede indexar un void*: guarda el malloc en una variable con tipo");
    }
    if (!i.isInt()) {
        error("el índice de un arreglo debe ser int, pero es " + i.kindName());
    }
    return b.apuntado();
}

// *p: p es un puntero; el resultado es el tipo apuntado
Type TypeChecker::visit(DerefExp* exp) {
    Type p = exp->puntero->accept(this);
    if (!p.isPtr()) {
        error("solo se puede desreferenciar (*) un puntero, pero se recibió " + p.kindName());
    }
    if (p.isVoidPtr()) {
        error("no se puede desreferenciar un void*");
    }
    return p.apuntado();
}

// &x: x debe tener dirección (variable, a[i] o *p); el resultado es un puntero a su tipo
Type TypeChecker::visit(AddrExp* exp) {
    if (!esLvalue(exp->lvalue)) {
        error("'&' necesita una variable, un elemento a[i] o un *p");
    }
    Type t = exp->lvalue->accept(this);
    if (t.isStack()) {
        error("no se puede tomar la dirección (&) de una pila");
    }
    return t.direccion();
}

// malloc(n): n es la cantidad de bytes (int); devuelve un void* asignable a cualquier puntero
Type TypeChecker::visit(MallocExp* exp) {
    Type b = exp->bytes->accept(this);
    if (!b.isInt()) {
        error("malloc necesita la cantidad de bytes (int), pero recibió " + b.kindName());
    }
    return Type::puntero(Type::VOID, 1);
}

// s.metodo(...): s debe ser una pila.
//   push(e): e del tipo de los elementos; no devuelve valor
//   pop() / top(): devuelven un elemento    empty(): bool    size(): int
Type TypeChecker::visit(StackOpExp* exp) {
    Type p = exp->pila->accept(this);
    if (!p.isStack()) {
        error("." + exp->metodo() + "() solo se puede usar sobre una pila, pero se usó sobre un " + p.kindName());
    }
    switch (exp->op) {
        case PUSH_OP: {
            Type v = exp->valor->accept(this);
            if (!p.elemento().aceptaValor(v)) {
                error("no se puede apilar " + v.kindName() + " en una " + p.kindName());
            }
            return Type();                       // void: push no se puede usar como valor
        }
        case POP_OP:
        case TOP_OP:
            return p.elemento();
        case EMPTY_OP:
            return Type(false);                  // bool
        default:
            return Type(0);                      // size: int
    }
}

Type TypeChecker::visit(SpaceshipExp* exp) {
    Type l = exp->left->accept(this);
    Type r = exp->right->accept(this);
    if (!l.isInt() || !r.isInt()) {
        error("el operador '<=>' necesita operandos int, pero recibió "
              + l.kindName() + " <=> " + r.kindName());
    }
    return Type(0);
}

Type TypeChecker::visit(ElvisExp* exp) {
    Type l = exp->left->accept(this);
    Type r = exp->right->accept(this);
    if (l.isVoid() || l.isStack() || (!l.aceptaValor(r) && !r.aceptaValor(l))) {
        error("el operador '?:' necesita dos operandos del mismo tipo (int, bool o puntero), pero recibió "
              + l.kindName() + " ?: " + r.kindName());
    }
    if (l.aceptaValor(r)) return l;
    return r;
}

// ---------- Sentencias ----------

// x = e: x declarada y e del mismo tipo
void TypeChecker::visit(AssignStmt* stm) {
    if (!tipos.check(stm->id)) {
        error("variable no declarada: " + stm->id);
    }
    Type declarado = tipos.lookup(stm->id);
    Type t = stm->e->accept(this);
    if (!declarado.aceptaValor(t)) {
        error("no se puede asignar " + t.kindName() + " a la variable "
              + declarado.kindName() + " " + stm->id);
    }
}

// s.push(e) | s.pop(): se verifica el método
void TypeChecker::visit(StackStmt* stm) {
    stm->op->accept(this);
}

// a[i] = e  |  *p = e: el valor debe tener el tipo del elemento
void TypeChecker::visit(StoreStmt* stm) {
    Type destino = stm->destino->accept(this);
    Type t = stm->e->accept(this);
    if (!destino.aceptaValor(t)) {
        error("no se puede asignar " + t.kindName() + " a un elemento de tipo " + destino.kindName());
    }
}

// free(p): p debe ser un puntero
void TypeChecker::visit(FreeStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isPtr()) {
        error("free necesita un puntero, pero recibió " + t.kindName());
    }
}

// print(e): e debe ser int o bool (una dirección no se imprime)
void TypeChecker::visit(PrintStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isInt() && !t.isBool()) {
        error("print necesita una expresión int o bool, pero recibió " + t.kindName());
    }
}

// if: condición bool y se verifican ambas ramas
void TypeChecker::visit(IfStmt* stm) {
    esperarBool(stm->condicion, "if");
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
}

// while: condición bool y se verifica el cuerpo
void TypeChecker::visit(WhileStmt* stm) {
    esperarBool(stm->condicion, "while");
    bucles++;
    for (Stmt* s : stm->body) s->accept(this);
    bucles--;
}

// do-while: se verifica el cuerpo y la condición debe ser bool
void TypeChecker::visit(DoWhileStmt* stm) {
    bucles++;
    for (Stmt* s : stm->body) s->accept(this);
    bucles--;
    esperarBool(stm->condicion, "do-while");
}

// break: solo dentro de un bucle
void TypeChecker::visit(BreakStmt* stm) {
    if (bucles == 0) {
        error("'break' fuera de un bucle");
    }
}

// return e: e debe tener el tipo de la función
void TypeChecker::visit(ReturnStmt* stm) {
    Type t = stm->exp->accept(this);
    if (!tipoDe(funcionActual->tipo).aceptaValor(t)) {
        error("la función " + funcionActual->nombre + " debe devolver "
              + funcionActual->tipo + ", pero return devuelve " + t.kindName());
    }
}

// ---------- Declaraciones y programa ----------

// var tipo ids: tipo válido y sin variables repetidas
void TypeChecker::visit(VarDec* dec) {
    string base = dec->tipo.substr(0, dec->tipo.find_first_of("*<"));
    if (base != "int" && base != "bool" && base != "stack") {
        error("tipo desconocido: " + dec->tipo);
    }
    for (const string& v : dec->variables) {
        if (tipos.check(v)) {
            error("variable declarada dos veces: " + v);
        }
        tipos.add_var(v, tipoDe(dec->tipo));
        numVariables[funcionActual->nombre]++;      // una variable local más
    }
}

// Primero las declaraciones, luego las sentencias
void TypeChecker::visit(Body* body) {
    for (VarDec* d : body->declaraciones) d->accept(this);
    for (Stmt* s : body->sentencias) s->accept(this);
}

// Una función abre su propio nivel con sus parámetros y variables locales
void TypeChecker::visit(FunDec* fun) {
    funcionActual = fun;
    numFunciones++;
    numVariables[fun->nombre] = fun->parametros_id.size();   // los parámetros también ocupan pila
    tipos.add_level();
    for (size_t i = 0; i < fun->parametros_id.size(); i++) {
        const string& p = fun->parametros_id[i];
        if (tipos.check(p)) {
            error("parámetro repetido en " + fun->nombre + ": " + p);
        }
        tipos.add_var(p, tipoDe(fun->parametros_tipos[i]));
    }
    fun->cuerpo->accept(this);
    tipos.remove_level();
    funcionActual = nullptr;
}

// Registra todas las funciones (para permitir recursión y llamadas a funciones
// definidas más abajo) y luego verifica cada una
void TypeChecker::visit(Programa* program) {
    for (FunDec* f : program->funciones) {
        if (funciones.count(f->nombre)) {
            error("función declarada dos veces: " + f->nombre);
        }
        if (f->parametros_id.size() > 6) {
            error("la función " + f->nombre + " tiene más de 6 parámetros");
        }
        funciones[f->nombre] = f;
    }
    if (!funciones.count("main")) {
        error("falta la función main");
    }
    FunDec* m = funciones["main"];
    if (m->tipo != "int" || !m->parametros_id.empty()) {
        error("main debe declararse como: fun int main()");
    }
    for (FunDec* f : program->funciones) f->accept(this);
}
