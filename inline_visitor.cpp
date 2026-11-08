#include "ast.h"
#include "inline_visitor.h"

using namespace std;

// Expande todo el programa
void InlineVisitor::expandir(Programa* program) {
    program->accept(this);
}

// Visita exp y devuelve la expresión que debe quedar en su lugar.
// Si exp se reemplazó, se libera el nodo que ya no se usa
Exp* InlineVisitor::reemplazar(Exp* exp) {
    reemplazo = exp;
    exp->accept(this);
    Exp* nueva = reemplazo;
    if (nueva != exp) delete exp;
    return nueva;
}

// ---------- Expresiones ----------
// Cada visit deja en reemplazo la expresión que la sustituye (ella misma si no cambia)
// y en esSimple si es un número o una variable

Type InlineVisitor::visit(BinaryExp* exp) {
    exp->left = reemplazar(exp->left);
    exp->right = reemplazar(exp->right);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(NumberExp* exp) {
    reemplazo = exp;
    esSimple = true;
    return Type();
}

// Dentro de una expansión, un parámetro se reemplaza por una copia de su argumento
Type InlineVisitor::visit(IdExp* exp) {
    reemplazo = exp;
    if (sustitucion.count(exp->value)) reemplazo = sustitucion[exp->value]->clonar();
    esSimple = true;
    return Type();
}

// f(a1, ..., an): se expanden los argumentos y, si se puede, la llamada se reemplaza
// por una copia de la expresión del return de f con cada parámetro cambiado por su argumento
Type InlineVisitor::visit(FCallExp* exp) {
    bool argumentosSimples = true;
    for (size_t i = 0; i < exp->argumentos.size(); i++) {
        exp->argumentos[i] = reemplazar(exp->argumentos[i]);
        if (!esSimple) argumentosSimples = false;
    }
    reemplazo = exp;
    esSimple = false;
    if (buscando || !argumentosSimples) return Type();
    if (!expandibles.count(exp->nombre) || expandiendo.count(exp->nombre)) return Type();

    // Se guarda la sustitución actual: la llamada puede estar dentro de otra expansión
    FunDec* f = funciones[exp->nombre];
    unordered_map<string, Exp*> anterior = sustitucion;
    sustitucion.clear();
    for (size_t i = 0; i < f->parametros_id.size(); i++) {
        sustitucion[f->parametros_id[i]] = exp->argumentos[i];
    }
    expandiendo.insert(f->nombre);
    Exp* copia = reemplazar(expandibles[f->nombre]->exp->clonar());   // deja esSimple de la copia
    expandiendo.erase(f->nombre);
    sustitucion = anterior;
    reemplazo = copia;
    return Type();
}

Type InlineVisitor::visit(IndexExp* exp) {
    exp->base = reemplazar(exp->base);
    exp->indice = reemplazar(exp->indice);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(DerefExp* exp) {
    exp->puntero = reemplazar(exp->puntero);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

// &x: una función que lo usa no se puede expandir
Type InlineVisitor::visit(AddrExp* exp) {
    usaDireccion = true;
    exp->lvalue = reemplazar(exp->lvalue);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(MallocExp* exp) {
    exp->bytes = reemplazar(exp->bytes);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(StackOpExp* exp) {
    exp->pila = reemplazar(exp->pila);
    if (exp->valor) exp->valor = reemplazar(exp->valor);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(SpaceshipExp* exp) {
    exp->left = reemplazar(exp->left);
    exp->right = reemplazar(exp->right);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

Type InlineVisitor::visit(ElvisExp* exp) {
    exp->left = reemplazar(exp->left);
    exp->right = reemplazar(exp->right);
    reemplazo = exp;
    esSimple = false;
    return Type();
}

// ---------- Sentencias ----------
// Reemplazan cada expresión por su versión expandida.
// Al terminar dejan retorno en nullptr; solo el return lo apunta a sí mismo

void InlineVisitor::visit(AssignStmt* stm) {
    stm->e = reemplazar(stm->e);
    retorno = nullptr;
}

// El método de pila nunca se reemplaza: solo se expanden sus subexpresiones
void InlineVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
    retorno = nullptr;
}

void InlineVisitor::visit(StoreStmt* stm) {
    stm->destino = reemplazar(stm->destino);
    stm->e = reemplazar(stm->e);
    retorno = nullptr;
}

void InlineVisitor::visit(FreeStmt* stm) {
    stm->e = reemplazar(stm->e);
    retorno = nullptr;
}

void InlineVisitor::visit(PrintStmt* stm) {
    stm->e = reemplazar(stm->e);
    retorno = nullptr;
}

void InlineVisitor::visit(IfStmt* stm) {
    stm->condicion = reemplazar(stm->condicion);
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
    retorno = nullptr;
}

void InlineVisitor::visit(WhileStmt* stm) {
    stm->condicion = reemplazar(stm->condicion);
    for (Stmt* s : stm->body) s->accept(this);
    retorno = nullptr;
}

void InlineVisitor::visit(DoWhileStmt* stm) {
    for (Stmt* s : stm->body) s->accept(this);
    stm->condicion = reemplazar(stm->condicion);
    retorno = nullptr;
}

void InlineVisitor::visit(BreakStmt* stm) {
    retorno = nullptr;
}

void InlineVisitor::visit(ReturnStmt* stm) {
    stm->exp = reemplazar(stm->exp);
    retorno = stm;
}

// ---------- Declaraciones y programa ----------

// Una declaración no tiene expresiones
void InlineVisitor::visit(VarDec* dec) {}

void InlineVisitor::visit(Body* body) {
    for (Stmt* s : body->sentencias) s->accept(this);
}

// Una función no se expande dentro de su propio cuerpo
void InlineVisitor::visit(FunDec* fun) {
    expandiendo.insert(fun->nombre);
    fun->cuerpo->accept(this);
    expandiendo.erase(fun->nombre);
}

// Primero busca las funciones expandibles (cuerpo: solo "return e", sin var ni '&')
// y luego expande las llamadas en todas las funciones
void InlineVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) funciones[f->nombre] = f;

    buscando = true;
    for (FunDec* f : program->funciones) {
        Body* cuerpo = f->cuerpo;
        if (!cuerpo->declaraciones.empty() || cuerpo->sentencias.size() != 1) continue;
        usaDireccion = false;
        cuerpo->sentencias[0]->accept(this);
        if (retorno && !usaDireccion) expandibles[f->nombre] = retorno;
    }
    buscando = false;

    for (FunDec* f : program->funciones) f->accept(this);
}
