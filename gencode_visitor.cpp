#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "ast.h"
#include "gencode_visitor.h"

using namespace std;

// Registros donde van los argumentos de una llamada (convención System V)
static const char* REG_ARGS[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

// Tipo (sin valor concreto) que corresponde a un nombre de tipo: int, bool, int*, int**, ...
static Type tipoDe(const string& nombre) {
    return Type::desdeNombre(nombre);
}

// Instrucción set que materializa el resultado de cada comparación (con signo)
static string instruccionSet(BinaryOp op) {
    switch (op) {
        case LE_OP: return "setle";
        case LT_OP: return "setl";
        case GT_OP: return "setg";
        case GE_OP: return "setge";
        case EQ_OP: return "sete";
        default:    return "setne";
    }
}

// =====================================================
// ================== GencodeVisitor ===================
// =====================================================

// Crea el generador de código, que escribe el ensamblador en out.
// Necesita el TypeChecker ya ejecutado para saber cuánta pila reservar en cada función.
GencodeVisitor::GencodeVisitor(ostream& out, TypeChecker* checker) : out(out), checker(checker) {}

// Genera el código de todo el programa
void GencodeVisitor::gencode(Programa* program) {
    program->accept(this);
}

// Indica si el operador es una comparación (su resultado es bool)
static bool esComparacion(BinaryOp op) {
    return op == LE_OP || op == LT_OP || op == GT_OP || op == GE_OP || op == EQ_OP || op == NE_OP;
}

// Indica si left op right es aritmética de punteros: p + i y p - i avanzan i elementos
static bool esAritmeticaPunteros(BinaryOp op, Type l) {
    return l.isPtr() && (op == PLUS_OP || op == MINUS_OP);
}

// Tipo (y valor) del resultado de left op right a partir de los tipos de sus operandos
static Type tipoResultado(BinaryOp op, Type l, Type r) {
    if (esAritmeticaPunteros(op, l)) return l;     // p + i y p - i tienen el tipo de p
    // Valor de cada operando visto como entero (un bool vale 1 o 0)
    int a = l.intValue;
    if (l.isBool()) a = l.boolValue;
    int b = r.intValue;
    if (r.isBool()) b = r.boolValue;
    switch (op) {
        case PLUS_OP:  return Type(a + b);
        case MINUS_OP: return Type(a - b);
        case MUL_OP:   return Type(a * b);
        case DIV_OP:
            if (b == 0) return Type(0);
            return Type(a / b);
        case POW_OP: {
            int p = 1;
            for (int i = 0; i < b; i++) p *= a;
            return Type(p);
        }
        case LE_OP:    return Type(a <= b);
        case LT_OP:    return Type(a < b);
        case GT_OP:    return Type(a > b);
        case GE_OP:    return Type(a >= b);
        case EQ_OP:    return Type(a == b);
        case NE_OP:    return Type(a != b);
        default:       return Type();
    }
}

// Genera el código de left op right y deja el resultado en %rax:
//   - plegado: si es constante, se carga su valor con un solo movq
//   - Sethi-Ullman: si la derecha tiene etiqueta 0 es una hoja y cargarla solo usa %rax,
//     así que la izquierda espera en %rcx en vez de guardarse en la pila
//   - si no, la izquierda se guarda con pushq mientras se evalúa la derecha
Type GencodeVisitor::visit(BinaryExp* exp) {
    if (exp->esConstante) {
        out << "movq $" << exp->valorConstante << ", %rax" << endl;
        if (esComparacion(exp->op)) return Type(exp->valorConstante != 0);
        return Type((int)exp->valorConstante);
    }
    Type l, r;
    if (exp->right->etiqueta == 0) {
        l = exp->left->accept(this);
        out << "movq %rax, %rcx" << endl;
        r = exp->right->accept(this);
        // Quedó left en %rcx y right en %rax: se intercambian salvo que el orden no importe
        bool conmutativa = exp->op == MUL_OP || exp->op == EQ_OP || exp->op == NE_OP
                        || (exp->op == PLUS_OP && !l.isPtr());
        if (!conmutativa) out << "xchgq %rax, %rcx" << endl;
    }
    else {
        l = exp->left->accept(this);
        out << "pushq %rax" << endl;
        pila++;
        r = exp->right->accept(this);
        out << "movq %rax, %rcx" << endl;
        out << "popq %rax" << endl;
        pila--;
    }
    operar(exp, l);
    return tipoResultado(exp->op, l, r);
}

// %rax = %rax op %rcx (left en %rax, right en %rcx)
void GencodeVisitor::operar(BinaryExp* exp, Type l) {
    // Aritmética de punteros: p + i avanza i elementos, es decir, i * 8 bytes
    if (esAritmeticaPunteros(exp->op, l)) {
        out << "imulq $" << TAM_VARIABLE << ", %rcx" << endl;
    }
    switch (exp->op) {
        case PLUS_OP:
            out << "addq %rcx, %rax" << endl;
            break;
        case MINUS_OP:
            out << "subq %rcx, %rax" << endl;
            break;
        case MUL_OP:
            out << "imulq %rcx, %rax" << endl;
            break;
        case DIV_OP:
            out << "cqto" << endl;                 // extiende el signo de %rax a %rdx
            out << "idivq %rcx" << endl;           // cociente en %rax
            break;
        case POW_OP: {
            // %rax = base ** exponente, multiplicando %rcx veces
            int id = etiquetas++;
            out << "movq %rax, %rdx" << endl;
            out << "movq $1, %rax" << endl;
            out << "pow_loop_" << id << ":" << endl;
            out << "cmpq $0, %rcx" << endl;
            out << "jle pow_end_" << id << endl;
            out << "imulq %rdx, %rax" << endl;
            out << "decq %rcx" << endl;
            out << "jmp pow_loop_" << id << endl;
            out << "pow_end_" << id << ":" << endl;
            break;
        }
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
        case EQ_OP:
        case NE_OP:
            // Compara y deja 1 (true) o 0 (false) en %rax
            out << "cmpq %rcx, %rax" << endl;
            out << instruccionSet(exp->op) << " %al" << endl;
            out << "movzbq %al, %rax" << endl;
            break;
        default:
            break;
    }
}

// Carga el literal en %rax
Type GencodeVisitor::visit(NumberExp* exp) {
    out << "movq $" << exp->value << ", %rax" << endl;
    if (exp->esBool) return Type(exp->value != 0);   // true / false
    return Type(exp->value);
}

// Carga el valor de la variable desde la pila en %rax
Type GencodeVisitor::visit(IdExp* exp) {
    out << "movq " << posicion.lookup(exp->value) * -TAM_VARIABLE << "(%rbp), %rax" << endl;
    return tipos.lookup(exp->value);
}

// Llamada: evalúa los argumentos, los pasa en registros y llama a la función.
// El resultado queda en %rax.
Type GencodeVisitor::visit(FCallExp* exp) {
    for (Exp* arg : exp->argumentos) {
        arg->accept(this);
        out << "pushq %rax" << endl;
        pila++;
    }
    for (int i = (int)exp->argumentos.size() - 1; i >= 0; i--) {
        out << "popq " << REG_ARGS[i] << endl;
        pila--;
    }
    llamar(exp->nombre);
    return tipoDe(funciones[exp->nombre]->tipo);
}

// a[i]: calcula la dirección del elemento y carga su valor en %rax
Type GencodeVisitor::visit(IndexExp* exp) {
    Type t = direccion(exp);
    out << "movq (%rax), %rax" << endl;
    return t;
}

// *p: evalúa el puntero y carga el valor guardado en esa dirección
Type GencodeVisitor::visit(DerefExp* exp) {
    Type t = direccion(exp);
    out << "movq (%rax), %rax" << endl;
    return t;
}

// &x: deja en %rax la dirección del lvalue
Type GencodeVisitor::visit(AddrExp* exp) {
    return direccion(exp->lvalue).direccion();
}

// malloc(bytes): pide memoria al heap; la dirección reservada vuelve en %rax
Type GencodeVisitor::visit(MallocExp* exp) {
    exp->bytes->accept(this);
    out << "movq %rax, %rdi" << endl;
    llamar("malloc@PLT");
    return Type::puntero(Type::VOID, 1);
}

// Métodos de pila. %rax tiene la dirección de la cabecera [tope, tamaño];
// cada nodo es [valor, siguiente]
Type GencodeVisitor::visit(StackOpExp* exp) {
    Type p = exp->pila->accept(this);
    switch (exp->op) {
        case PUSH_OP:
            // nodo = malloc(16); nodo->valor = e; nodo->siguiente = tope; tope = nodo; tamaño++
            out << "pushq %rax" << endl;
            pila++;
            exp->valor->accept(this);
            out << "pushq %rax" << endl;
            pila++;
            out << "movq $" << TAM_NODO_PILA << ", %rdi" << endl;
            llamar("malloc@PLT");
            out << "popq %rcx" << endl;            // valor
            out << "popq %rdx" << endl;            // cabecera
            pila -= 2;
            out << "movq %rcx, (%rax)" << endl;
            out << "movq (%rdx), %rcx" << endl;
            out << "movq %rcx, 8(%rax)" << endl;
            out << "movq %rax, (%rdx)" << endl;
            out << "incq 8(%rdx)" << endl;
            return Type();
        case POP_OP:
            // nodo = tope (error si es nulo); tope = nodo->siguiente; tamaño--; free(nodo)
            usaPilaVacia = true;
            out << "movq (%rax), %rdi" << endl;
            out << "cmpq $0, %rdi" << endl;
            out << "je .Lpila_vacia" << endl;
            out << "movq 8(%rdi), %rcx" << endl;
            out << "movq %rcx, (%rax)" << endl;
            out << "decq 8(%rax)" << endl;
            out << "pushq (%rdi)" << endl;         // guarda el valor mientras se libera el nodo
            pila++;
            llamar("free@PLT");
            out << "popq %rax" << endl;
            pila--;
            return p.elemento();
        case TOP_OP:
            usaPilaVacia = true;
            out << "movq (%rax), %rax" << endl;
            out << "cmpq $0, %rax" << endl;
            out << "je .Lpila_vacia" << endl;
            out << "movq (%rax), %rax" << endl;
            return p.elemento();
        case EMPTY_OP:
            out << "cmpq $0, 8(%rax)" << endl;
            out << "sete %al" << endl;
            out << "movzbq %al, %rax" << endl;
            return Type(false);
        case SIZE_OP:
            out << "movq 8(%rax), %rax" << endl;
            return Type(0);
        default:
            return Type();
    }
}

// left <=> right: deja -1, 0 o 1 en %rax. Los operandos se cargan como en un BinaryExp
// (left en %rax, right en %rcx) y el resultado es (left > right) - (left < right)
Type GencodeVisitor::visit(SpaceshipExp* exp) {
    if (exp->esConstante) {
        out << "movq $" << exp->valorConstante << ", %rax" << endl;
        return Type((int)exp->valorConstante);
    }
    if (exp->right->etiqueta == 0) {
        exp->left->accept(this);
        out << "movq %rax, %rcx" << endl;
        exp->right->accept(this);
        out << "xchgq %rax, %rcx" << endl;
    }
    else {
        exp->left->accept(this);
        out << "pushq %rax" << endl;
        pila++;
        exp->right->accept(this);
        out << "movq %rax, %rcx" << endl;
        out << "popq %rax" << endl;
        pila--;
    }
    out << "cmpq %rcx, %rax" << endl;
    out << "setg %dl" << endl;
    out << "setl %al" << endl;
    out << "subb %al, %dl" << endl;
    out << "movsbq %dl, %rax" << endl;
    return Type(0);
}

// left ?: right: evalúa left una sola vez; si es distinto de 0 queda en %rax,
// si no se evalúa right en %rax
Type GencodeVisitor::visit(ElvisExp* exp) {
    if (exp->esConstante) {
        out << "movq $" << exp->valorConstante << ", %rax" << endl;
        return Type((int)exp->valorConstante);
    }
    int id = etiquetas++;
    Type l = exp->left->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "jne elvis_end_" << id << endl;
    exp->right->accept(this);
    out << "elvis_end_" << id << ":" << endl;
    if (l.isInt()) return Type(0);
    if (l.isBool()) return Type(false);
    return l;
}

// Deja en %rax la dirección de un lvalue y devuelve el tipo del valor guardado ahí:
//   x    -> posición de x en la pila
//   a[i] -> a + i * 8 (a es la dirección del primer elemento)
//   *p   -> el valor de p
Type GencodeVisitor::direccion(Exp* lvalue) {
    if (IdExp* id = dynamic_cast<IdExp*>(lvalue)) {
        out << "leaq " << posicion.lookup(id->value) * -TAM_VARIABLE << "(%rbp), %rax" << endl;
        return tipos.lookup(id->value);
    }
    if (IndexExp* acceso = dynamic_cast<IndexExp*>(lvalue)) {
        Type base = acceso->base->accept(this);
        out << "pushq %rax" << endl;
        pila++;
        acceso->indice->accept(this);
        out << "movq %rax, %rcx" << endl;
        out << "popq %rax" << endl;
        pila--;
        out << "leaq (%rax,%rcx," << TAM_VARIABLE << "), %rax" << endl;
        return base.apuntado();
    }
    if (DerefExp* deref = dynamic_cast<DerefExp*>(lvalue)) {
        return deref->puntero->accept(this).apuntado();
    }
    throw runtime_error("la expresión no tiene dirección");
}

// Llama a la función con la pila alineada a 16 bytes, como exige System V
void GencodeVisitor::llamar(const string& funcion) {
    bool alinear = pila % 2 != 0;
    if (alinear) out << "subq $8, %rsp" << endl;
    out << "call " << funcion << endl;
    if (alinear) out << "addq $8, %rsp" << endl;
}

// Evalúa la expresión y la guarda en la posición de la variable
void GencodeVisitor::visit(AssignStmt* stm) {
    Type t = stm->e->accept(this);
    if (!t.isPtr()) tipos.update(stm->id, t);      // un puntero conserva su tipo declarado (malloc da void*)
    out << "movq %rax, " << posicion.lookup(stm->id) * -TAM_VARIABLE << "(%rbp)" << endl;
}

// s.push(e) | s.pop(): genera el método; el valor de pop se descarta
void GencodeVisitor::visit(StackStmt* stm) {
    stm->op->accept(this);
}

// a[i] = e  |  *p = e: calcula la dirección destino, evalúa e y la guarda en memoria
void GencodeVisitor::visit(StoreStmt* stm) {
    direccion(stm->destino);
    out << "pushq %rax" << endl;
    pila++;
    stm->e->accept(this);
    out << "popq %rcx" << endl;
    pila--;
    out << "movq %rax, (%rcx)" << endl;
}

// free(p): devuelve al heap la memoria reservada con malloc
void GencodeVisitor::visit(FreeStmt* stm) {
    stm->e->accept(this);
    out << "movq %rax, %rdi" << endl;
    llamar("free@PLT");
}

// Evalúa la expresión y la imprime con printf
void GencodeVisitor::visit(PrintStmt* stm) {
    stm->e->accept(this);
    out << "movq %rax, %rsi" << endl;
    out << "leaq print_fmt(%rip), %rdi" << endl;
    out << "movl $0, %eax" << endl;
    out << "call printf@PLT" << endl;
}

// Genera el código del if: si la condición es 0 salta a la rama else
void GencodeVisitor::visit(IfStmt* stm) {
    int id = etiquetas++;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je else_" << id << endl;
    for (Stmt* s : stm->thenList) s->accept(this);
    out << "jmp endif_" << id << endl;
    out << "else_" << id << ":" << endl;
    for (Stmt* s : stm->elseList) s->accept(this);
    out << "endif_" << id << ":" << endl;
}

// Genera el código del while: evalúa la condición al inicio de cada vuelta
void GencodeVisitor::visit(WhileStmt* stm) {
    int id = etiquetas++;
    string fin = "endwhile_" + to_string(id);
    out << "while_" << id << ":" << endl;
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "je " << fin << endl;
    finDeBucle.push_back(fin);
    for (Stmt* s : stm->body) s->accept(this);
    finDeBucle.pop_back();
    out << "jmp while_" << id << endl;
    out << fin << ":" << endl;
}

// Genera el código del do-while: ejecuta el cuerpo y repite mientras la condición no sea 0
void GencodeVisitor::visit(DoWhileStmt* stm) {
    int id = etiquetas++;
    string fin = "enddowhile_" + to_string(id);
    out << "dowhile_" << id << ":" << endl;
    finDeBucle.push_back(fin);
    for (Stmt* s : stm->body) s->accept(this);
    finDeBucle.pop_back();
    stm->condicion->accept(this);
    out << "cmpq $0, %rax" << endl;
    out << "jne dowhile_" << id << endl;
    out << fin << ":" << endl;
}

// Genera el código del break: salta a la salida del bucle más interno
void GencodeVisitor::visit(BreakStmt* stm) {
    out << "jmp " << finDeBucle.back() << endl;
}

// Genera el código del return: deja el valor en %rax y salta al epílogo de la función
void GencodeVisitor::visit(ReturnStmt* stm) {
    stm->exp->accept(this);
    out << "jmp .Lfin_" << funcionActual << endl;
}

// Declara cada variable en el nivel actual: le asigna una posición en la pila y su tipo.
// Una pila nace vacía: se reserva su cabecera en el heap con tope = 0 y tamaño = 0
void GencodeVisitor::visit(VarDec* dec) {
    for (const string& v : dec->variables) {
        posicion.add_var(v, contador++);
        tipos.add_var(v, tipoDe(dec->tipo));
        int offset = posicion.lookup(v) * -TAM_VARIABLE;
        if (tipoDe(dec->tipo).isStack()) {
            out << "movq $" << TAM_CABECERA_PILA << ", %rdi" << endl;
            llamar("malloc@PLT");
            out << "movq $0, (%rax)" << endl;
            out << "movq $0, 8(%rax)" << endl;
            out << "movq %rax, " << offset << "(%rbp)" << endl;
        }
        else {
            out << "movq $0, " << offset << "(%rbp)" << endl;   // inicia en 0 / false
        }
    }
}

// Procesa primero las declaraciones y luego las sentencias
void GencodeVisitor::visit(Body* body) {
    for (VarDec* d : body->declaraciones) d->accept(this);
    for (Stmt* s : body->sentencias) s->accept(this);
}

// Genera una función: prólogo, copia de parámetros a la pila, cuerpo y epílogo
void GencodeVisitor::visit(FunDec* fun) {
    funcionActual = fun->nombre;
    contador = 1;
    posicion.add_level();
    tipos.add_level();

    // Espacio en la pila según lo que contó el TypeChecker
    int espacio = checker->espacioPila(fun->nombre);

    out << fun->nombre << ":" << endl;
    out << "pushq %rbp" << endl;
    out << "movq %rsp, %rbp" << endl;
    if (espacio > 0) out << "subq $" << espacio << ", %rsp" << endl;

    // Los parámetros llegan en registros: se guardan en la pila como variables locales
    for (size_t i = 0; i < fun->parametros_id.size(); i++) {
        const string& p = fun->parametros_id[i];
        posicion.add_var(p, contador++);
        tipos.add_var(p, tipoDe(fun->parametros_tipos[i]));
        out << "movq " << REG_ARGS[i] << ", " << posicion.lookup(p) * -TAM_VARIABLE << "(%rbp)" << endl;
    }

    fun->cuerpo->accept(this);

    out << "movq $0, %rax" << endl;            // si no hubo return, devuelve 0
    out << ".Lfin_" << fun->nombre << ":" << endl;
    if (fun->nombre == "main") {
        out << "movq $0, %rax" << endl;        // main siempre termina con código 0
    }
    out << "leave" << endl;
    out << "ret" << endl;

    posicion.remove_level();
    tipos.remove_level();
}

// Genera el programa completo: sección de datos y el código de cada función
void GencodeVisitor::visit(Programa* program) {
    for (FunDec* f : program->funciones) funciones[f->nombre] = f;

    out << ".data" << endl;
    out << "print_fmt:.string \"%ld\\n\" " << endl;
    out << ".text" << endl;
    out << ".globl main" << endl;
    for (FunDec* f : program->funciones) f->accept(this);
    if (usaPilaVacia) {
        // pop / top sobre una pila vacía: mensaje en stderr y exit(1)
        out << ".Lpila_vacia:" << endl;
        out << "andq $-16, %rsp" << endl;
        out << "movq stderr@GOTPCREL(%rip), %rax" << endl;
        out << "movq (%rax), %rdi" << endl;
        out << "leaq pila_vacia_msg(%rip), %rsi" << endl;
        out << "movl $0, %eax" << endl;
        out << "call fprintf@PLT" << endl;
        out << "movl $1, %edi" << endl;
        out << "call exit@PLT" << endl;
        out << ".data" << endl;
        out << "pila_vacia_msg:.string \"Error: pop/top sobre una pila vacia\\n\"" << endl;
    }
    out << ".section .note.GNU-stack,\"\",@progbits" << endl;
}
