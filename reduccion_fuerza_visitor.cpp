#include "ast.h"
#include "reduccion_fuerza_visitor.h"

using namespace std;

// Indica si la expresión es la constante v
static bool esValor(Exp* e, long long v) {
    return e->esConstante && e->valorConstante == v;
}

// Reduce todo el programa
void ReduccionFuerzaVisitor::reducir(Programa* program) {
    program->accept(this);
}

// Visita exp y devuelve la expresión que debe quedar en su lugar.
// Si exp se reemplazó, se libera el nodo que ya no se usa
Exp* ReduccionFuerzaVisitor::reemplazar(Exp* exp) {
    reemplazo = exp;
    exp->accept(this);
    Exp* nueva = reemplazo;
    if (nueva != exp) delete exp;
    return nueva;
}

// ---------- Expresiones ----------
// Cada visit deja en reemplazo la expresión que la sustituye (ella misma si no cambia)
// y en variable su nombre si es una variable ("" si no)

// Simplificación algebraica y reducción de fuerza de left op right
Type ReduccionFuerzaVisitor::visit(BinaryExp* exp) {
    exp->left = reemplazar(exp->left);
    string izquierda = variable;
    exp->right = reemplazar(exp->right);
    string derecha = variable;
    reemplazo = exp;
    variable = "";

    // x + 0, x * 1 y x / 1 -> x
    bool identidad = (exp->op == PLUS_OP && esValor(exp->right, 0))
                  || (exp->op == MUL_OP && esValor(exp->right, 1))
                  || (exp->op == DIV_OP && esValor(exp->right, 1));
    if (identidad) {
        reemplazo = exp->left;
        exp->left = nullptr;          // x sigue en el árbol: no se libera junto con el nodo
        variable = izquierda;
    }
    // x * 2 -> x + x
    else if (exp->op == MUL_OP && esValor(exp->right, 2) && izquierda != "") {
        delete exp->right;
        exp->op = PLUS_OP;
        exp->right = new IdExp(izquierda);
    }
    // 2 * x -> x + x
    else if (exp->op == MUL_OP && esValor(exp->left, 2) && derecha != "") {
        delete exp->left;
        exp->op = PLUS_OP;
        exp->left = new IdExp(derecha);
    }
    // x ** 2 -> x * x
    else if (exp->op == POW_OP && esValor(exp->right, 2) && izquierda != "") {
        delete exp->right;
        exp->op = MUL_OP;
        exp->right = new IdExp(izquierda);
    }
    return Type();
}

// Un literal no se reduce
Type ReduccionFuerzaVisitor::visit(NumberExp* exp) {
    reemplazo = exp;
    variable = "";
    return Type();
}

// Una variable no se reduce, pero avisa su nombre al padre
Type ReduccionFuerzaVisitor::visit(IdExp* exp) {
    reemplazo = exp;
    variable = exp->value;
    return Type();
}

// f(a1, ..., an): se reducen los argumentos
Type ReduccionFuerzaVisitor::visit(FCallExp* exp) {
    for (size_t i = 0; i < exp->argumentos.size(); i++) {
        exp->argumentos[i] = reemplazar(exp->argumentos[i]);
    }
    reemplazo = exp;
    variable = "";
    return Type();
}

// a[i]: se reducen la base y el índice
Type ReduccionFuerzaVisitor::visit(IndexExp* exp) {
    exp->base = reemplazar(exp->base);
    exp->indice = reemplazar(exp->indice);
    reemplazo = exp;
    variable = "";
    return Type();
}

// *p: se reduce el puntero
Type ReduccionFuerzaVisitor::visit(DerefExp* exp) {
    exp->puntero = reemplazar(exp->puntero);
    reemplazo = exp;
    variable = "";
    return Type();
}

// &x: se reduce el lvalue
Type ReduccionFuerzaVisitor::visit(AddrExp* exp) {
    exp->lvalue = reemplazar(exp->lvalue);
    reemplazo = exp;
    variable = "";
    return Type();
}

// malloc(n): se reduce n
Type ReduccionFuerzaVisitor::visit(MallocExp* exp) {
    exp->bytes = reemplazar(exp->bytes);
    reemplazo = exp;
    variable = "";
    return Type();
}

// s.metodo(...): se reducen la pila y el valor de push
Type ReduccionFuerzaVisitor::visit(StackOpExp* exp) {
    exp->pila = reemplazar(exp->pila);
    if (exp->valor) exp->valor = reemplazar(exp->valor);
    reemplazo = exp;
    variable = "";
    return Type();
}

// left <=> right: se reducen los dos operandos
Type ReduccionFuerzaVisitor::visit(SpaceshipExp* exp) {
    exp->left = reemplazar(exp->left);
    exp->right = reemplazar(exp->right);
    reemplazo = exp;
    variable = "";
    return Type();
}

// left ?: right con left constante:  k ?: x -> k (k != 0)    0 ?: x -> x
Type ReduccionFuerzaVisitor::visit(ElvisExp* exp) {
    exp->left = reemplazar(exp->left);
    string izquierda = variable;
    exp->right = reemplazar(exp->right);
    string derecha = variable;
    reemplazo = exp;
    variable = "";
    if (!exp->left->esConstante) return Type();
    if (exp->left->valorConstante != 0) {
        reemplazo = exp->left;
        exp->left = nullptr;          // k sigue en el árbol: no se libera junto con el nodo
        variable = izquierda;
    }
    else {
        reemplazo = exp->right;
        exp->right = nullptr;         // x sigue en el árbol: no se libera junto con el nodo
        variable = derecha;
    }
    return Type();
}

// ---------- Sentencias ----------
// Reemplazan cada expresión por su versión reducida

void ReduccionFuerzaVisitor::visit(AssignStmt* stm) {
    stm->e = reemplazar(stm->e);
}

// El método de pila nunca se reemplaza: solo se reducen sus subexpresiones
void ReduccionFuerzaVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
}

void ReduccionFuerzaVisitor::visit(StoreStmt* stm) {
    stm->destino = reemplazar(stm->destino);
    stm->e = reemplazar(stm->e);
}

void ReduccionFuerzaVisitor::visit(FreeStmt* stm) {
    stm->e = reemplazar(stm->e);
}

void ReduccionFuerzaVisitor::visit(PrintStmt* stm) {
    stm->e = reemplazar(stm->e);
}

void ReduccionFuerzaVisitor::visit(IfStmt* stm) {
    stm->condicion = reemplazar(stm->condicion);
    for (Stmt* s : stm->thenList) s->accept(this);
    for (Stmt* s : stm->elseList) s->accept(this);
}

void ReduccionFuerzaVisitor::visit(WhileStmt* stm) {
    stm->condicion = reemplazar(stm->condicion);
    for (Stmt* s : stm->body) s->accept(this);
}

void ReduccionFuerzaVisitor::visit(DoWhileStmt* stm) {
    for (Stmt* s : stm->body) s->accept(this);
    stm->condicion = reemplazar(stm->condicion);
}

void ReduccionFuerzaVisitor::visit(BreakStmt* stm) {}

void ReduccionFuerzaVisitor::visit(ReturnStmt* stm) {
    stm->exp = reemplazar(stm->exp);
}

// ---------- Declaraciones y programa ----------

// Una declaración no tiene expresiones
void ReduccionFuerzaVisitor::visit(VarDec* dec) {}

void ReduccionFuerzaVisitor::visit(Body* body) {
    for (Stmt* s : body->sentencias) s->accept(this);
}

void ReduccionFuerzaVisitor::visit(FunDec* fun) {
    fun->cuerpo->accept(this);
}

void ReduccionFuerzaVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) f->accept(this);
}
