#include <iostream>
#include "ast.h"

using namespace std;

// Destructor de Exp
Exp::~Exp() {}

// Devuelve el símbolo de un operador binario
string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case POW_OP:   return "**";
        case LE_OP:    return "<=";
        case LT_OP:    return "<";
        case GT_OP:    return ">";
        case GE_OP:    return ">=";
        case EQ_OP:    return "==";
        case NE_OP:    return "!=";
        default:       return "?";
    }
}

// Crea una expresión binaria con sus dos operandos
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

// Libera los dos operandos
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

// Copia la expresión binaria con copias de sus operandos
Exp* BinaryExp::clonar() {
    return new BinaryExp(left->clonar(), right->clonar(), op);
}

SpaceshipExp::SpaceshipExp(Exp* l, Exp* r) : left(l), right(r) {}

SpaceshipExp::~SpaceshipExp() {
    delete left;
    delete right;
}

Exp* SpaceshipExp::clonar() {
    return new SpaceshipExp(left->clonar(), right->clonar());
}

ElvisExp::ElvisExp(Exp* l, Exp* r) : left(l), right(r) {}

ElvisExp::~ElvisExp() {
    delete left;
    delete right;
}

Exp* ElvisExp::clonar() {
    return new ElvisExp(left->clonar(), right->clonar());
}

// Crea un literal entero
NumberExp::NumberExp(int v, bool b) : value(v), esBool(b) {}

// Destructor de NumberExp
NumberExp::~NumberExp() {}

// Copia el literal
Exp* NumberExp::clonar() {
    return new NumberExp(value, esBool);
}

// Crea una referencia a una variable
IdExp::IdExp(string v) : value(v) {}

// Destructor de IdExp
IdExp::~IdExp() {}

// Copia la referencia a la variable
Exp* IdExp::clonar() {
    return new IdExp(value);
}

// Crea un acceso base[indice]
IndexExp::IndexExp(Exp* b, Exp* i) : base(b), indice(i) {}

// Libera la base y el índice
IndexExp::~IndexExp() {
    delete base;
    delete indice;
}

// Copia el acceso con copias de la base y el índice
Exp* IndexExp::clonar() {
    return new IndexExp(base->clonar(), indice->clonar());
}

// Crea una desreferencia *p
DerefExp::DerefExp(Exp* p) : puntero(p) {}

// Libera la expresión puntero
DerefExp::~DerefExp() {
    delete puntero;
}

// Copia la desreferencia
Exp* DerefExp::clonar() {
    return new DerefExp(puntero->clonar());
}

// Crea una expresión &lvalue
AddrExp::AddrExp(Exp* l) : lvalue(l) {}

// Libera el lvalue
AddrExp::~AddrExp() {
    delete lvalue;
}

// Copia la expresión &lvalue
Exp* AddrExp::clonar() {
    return new AddrExp(lvalue->clonar());
}

// Crea un malloc(bytes)
MallocExp::MallocExp(Exp* b) : bytes(b) {}

// Libera la expresión con la cantidad de bytes
MallocExp::~MallocExp() {
    delete bytes;
}

// Copia el malloc
Exp* MallocExp::clonar() {
    return new MallocExp(bytes->clonar());
}

// Crea una llamada a un método de pila
StackOpExp::StackOpExp(Exp* p, StackOp o, Exp* v) : pila(p), op(o), valor(v) {}

// Devuelve el nombre del método de pila
string StackOpExp::metodo() const {
    switch (op) {
        case PUSH_OP:  return "push";
        case POP_OP:   return "pop";
        case TOP_OP:   return "top";
        case EMPTY_OP: return "empty";
        case SIZE_OP:  return "size";
        default:       return "?";
    }
}

// Libera la pila y el valor
StackOpExp::~StackOpExp() {
    delete pila;
    delete valor;
}

// Copia el método de pila (valor solo existe en push)
Exp* StackOpExp::clonar() {
    Exp* v = nullptr;
    if (valor) v = valor->clonar();
    return new StackOpExp(pila->clonar(), op, v);
}

// Destructor de Stmt
Stmt::~Stmt() {}

// Crea un método de pila usado como sentencia
StackStmt::StackStmt(StackOpExp* o) : op(o) {}

// Libera la llamada al método
StackStmt::~StackStmt() {
    delete op;
}

// Crea una asignación a memoria destino = e
StoreStmt::StoreStmt(Exp* d, Exp* expresion) : destino(d), e(expresion) {}

// Libera el destino y la expresión
StoreStmt::~StoreStmt() {
    delete destino;
    delete e;
}

// Crea un free(e)
FreeStmt::FreeStmt(Exp* expresion) : e(expresion) {}

// Libera la expresión
FreeStmt::~FreeStmt() {
    delete e;
}

// Crea una asignación de la expresión a la variable
AssignStmt::AssignStmt(string variable, Exp* expresion) {
    id = variable;
    e = expresion;
}

// Destructor de AssignStmt
AssignStmt::~AssignStmt() {}

// Crea una sentencia print de la expresión
PrintStmt::PrintStmt(Exp* expresion) {
    e = expresion;
}

// Destructor de PrintStmt
PrintStmt::~PrintStmt() {}

// Crea un if con su condición, rama then y rama else (puede estar vacía)
IfStmt::IfStmt(Exp* c, list<Stmt*> t, list<Stmt*> e)
    : condicion(c), thenList(t), elseList(e) {}

// Libera la condición y las sentencias de ambas ramas
IfStmt::~IfStmt() {
    delete condicion;
    for (Stmt* s : thenList) delete s;
    for (Stmt* s : elseList) delete s;
}

// Crea un while con su condición y cuerpo
WhileStmt::WhileStmt(Exp* c, list<Stmt*> b)
    : condicion(c), body(b) {}

// Libera la condición y las sentencias del cuerpo
WhileStmt::~WhileStmt() {
    delete condicion;
    for (Stmt* s : body) delete s;
}

// Crea un do-while con su cuerpo y condición
DoWhileStmt::DoWhileStmt(list<Stmt*> b, Exp* c)
    : body(b), condicion(c) {}

// Libera las sentencias del cuerpo y la condición
DoWhileStmt::~DoWhileStmt() {
    for (Stmt* s : body) delete s;
    delete condicion;
}

// Crea una sentencia break
BreakStmt::BreakStmt() {}

// Destructor de BreakStmt
BreakStmt::~BreakStmt() {}

// Crea una declaración vacía (el parser llena tipo y variables)
VarDec::VarDec() {}

// Destructor de VarDec
VarDec::~VarDec() {}

// Crea un cuerpo vacío
Body::Body() {}

// Libera las declaraciones y las sentencias
Body::~Body() {
    for (VarDec* d : declaraciones) delete d;
    for (Stmt* s : sentencias) delete s;
}

// Crea una llamada sin argumentos (el parser los agrega)
FCallExp::FCallExp() {}

// Libera los argumentos
FCallExp::~FCallExp() {
    for (Exp* e : argumentos) delete e;
}

// Copia la llamada con copias de sus argumentos
Exp* FCallExp::clonar() {
    FCallExp* copia = new FCallExp();
    copia->nombre = nombre;
    for (Exp* e : argumentos) copia->argumentos.push_back(e->clonar());
    return copia;
}

// Crea un return vacío (el parser asigna la expresión)
ReturnStmt::ReturnStmt() : exp(nullptr) {}

// Libera la expresión devuelta
ReturnStmt::~ReturnStmt() {
    delete exp;
}

// Crea una función vacía (el parser llena sus datos)
FunDec::FunDec() : cuerpo(nullptr) {}

// Libera el cuerpo de la función
FunDec::~FunDec() {
    delete cuerpo;
}

// Crea un programa sin funciones
Programa::Programa() {}

// Libera todas las funciones
Programa::~Programa() {
    for (FunDec* f : funciones) delete f;
}
