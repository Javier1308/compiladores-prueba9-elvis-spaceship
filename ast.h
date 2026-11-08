#ifndef AST_H
#define AST_H

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <ostream>
#include "type.h"

using namespace std;

class Visitor;

// Operadores binarios del lenguaje
enum BinaryOp {
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    POW_OP,
    LE_OP,
    LT_OP,
    GT_OP,
    GE_OP,
    EQ_OP,
    NE_OP
};

// Métodos del tipo stack
enum StackOp {
    PUSH_OP,     // s.push(e): apila e
    POP_OP,      // s.pop():   desapila y devuelve el tope
    TOP_OP,      // s.top():   devuelve el tope sin desapilar
    EMPTY_OP,    // s.empty(): true si la pila no tiene elementos
    SIZE_OP      // s.size():  cantidad de elementos
};

// ==================== Expresiones ====================
// Las expresiones devuelven un Type (tipo y valor)

// Clase base abstracta de todas las expresiones
class Exp {
public:
    // Plegado de constantes (los llena el ConstanteVisitor)
    bool esConstante = false;        // true si el valor se conoce en compilación
    long long valorConstante = 0;    // valor de la expresión si esConstante (un bool vale 1 o 0)

    // Sethi-Ullman: registros que necesita la expresión (lo llena el SethiUllmanVisitor)
    int etiqueta = 0;

    virtual Type accept(Visitor* visitor) = 0;
    virtual Exp* clonar() = 0;       // copia profunda de la expresión (sin los atributos de optimización)
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};

// Expresión binaria: left op right
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~BinaryExp();
};

// Expresión numérica: un literal entero (o true / false)
class NumberExp : public Exp {
public:
    int value;
    bool esBool;    // true si viene de los literales true / false
    NumberExp(int v, bool esBool = false);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~NumberExp();
};

// Expresión identificador: uso de una variable
class IdExp : public Exp {
public:
    string value;
    IdExp(string v);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~IdExp();
};

// Llamada a función: nombre(arg1, arg2, ...)
class FCallExp : public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    FCallExp();
    ~FCallExp();
};

// Acceso a un elemento: base[indice]  (equivale a *(base + indice)).
// Con punteros dobles, m[i][j] es IndexExp(IndexExp(m, i), j)
class IndexExp : public Exp {
public:
    Exp* base;
    Exp* indice;
    IndexExp(Exp* b, Exp* i);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~IndexExp();
};

// Desreferencia: *puntero (el valor guardado en la dirección)
class DerefExp : public Exp {
public:
    Exp* puntero;
    DerefExp(Exp* p);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~DerefExp();
};

// Dirección: &lvalue (lvalue es una variable, un elemento a[i] o un *p)
class AddrExp : public Exp {
public:
    Exp* lvalue;
    AddrExp(Exp* l);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~AddrExp();
};

// Memoria dinámica: malloc(bytes) reserva memoria en el heap y devuelve un void*
class MallocExp : public Exp {
public:
    Exp* bytes;
    MallocExp(Exp* b);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~MallocExp();
};

// Llamada a un método de pila: pila.push(valor) | pila.pop() | pila.top() | pila.empty() | pila.size()
class StackOpExp : public Exp {
public:
    Exp* pila;
    StackOp op;
    Exp* valor;     // solo en push; nullptr en los demás métodos
    StackOpExp(Exp* p, StackOp op, Exp* v = nullptr);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    string metodo() const;   // nombre del método: "push", "pop", ...
    ~StackOpExp();
};

// Operador spaceship: left <=> right vale -1 si left < right, 0 si son iguales y 1 si left > right
class SpaceshipExp : public Exp {
public:
    Exp* left;
    Exp* right;
    SpaceshipExp(Exp* l, Exp* r);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~SpaceshipExp();
};

// Operador elvis: left ?: right vale left si es distinto de 0 y right en otro caso.
// left se evalúa una sola vez y right solo si hace falta
class ElvisExp : public Exp {
public:
    Exp* left;
    Exp* right;
    ElvisExp(Exp* l, Exp* r);
    Type accept(Visitor* visitor) override;
    Exp* clonar() override;
    ~ElvisExp();
};

// ==================== Sentencias ====================
// Las sentencias no producen valor: accept devuelve void

// Clase base abstracta de todas las sentencias
class Stmt {
public:
    virtual void accept(Visitor* visitor) = 0;
    virtual ~Stmt() = 0;
};

// Sentencia de asignación: id = e
class AssignStmt : public Stmt {
public:
    string id;
    Exp* e;
    AssignStmt(string, Exp*);
    void accept(Visitor* visitor) override;
    ~AssignStmt();
};

// Asignación a memoria: destino = e, donde destino es a[i], m[i][j] o *p
class StoreStmt : public Stmt {
public:
    Exp* destino;
    Exp* e;
    StoreStmt(Exp* d, Exp* e);
    void accept(Visitor* visitor) override;
    ~StoreStmt();
};

// Liberación de memoria dinámica: free(e)
class FreeStmt : public Stmt {
public:
    Exp* e;
    FreeStmt(Exp* e);
    void accept(Visitor* visitor) override;
    ~FreeStmt();
};

// Método de pila usado como sentencia: s.push(e) | s.pop()  (el valor de pop se descarta)
class StackStmt : public Stmt {
public:
    StackOpExp* op;
    StackStmt(StackOpExp* op);
    void accept(Visitor* visitor) override;
    ~StackStmt();
};

// Sentencia de impresión: print(e)
class PrintStmt : public Stmt {
public:
    Exp* e;
    PrintStmt(Exp*);
    void accept(Visitor* visitor) override;
    ~PrintStmt();
};

// Sentencia condicional: if CExp then StmtList [else StmtList] endif
class IfStmt : public Stmt {
public:
    Exp* condicion;
    list<Stmt*> thenList;
    list<Stmt*> elseList;
    IfStmt(Exp*, list<Stmt*>, list<Stmt*>);
    void accept(Visitor* visitor) override;
    ~IfStmt();
};

// Sentencia de repetición: while CExp do StmtList enddo
class WhileStmt : public Stmt {
public:
    Exp* condicion;
    list<Stmt*> body;
    WhileStmt(Exp*, list<Stmt*>);
    void accept(Visitor* visitor) override;
    ~WhileStmt();
};

// Sentencia de repetición: do StmtList while CExp (el cuerpo se ejecuta al menos una vez)
class DoWhileStmt : public Stmt {
public:
    list<Stmt*> body;
    Exp* condicion;
    DoWhileStmt(list<Stmt*>, Exp*);
    void accept(Visitor* visitor) override;
    ~DoWhileStmt();
};

// Sentencia break: sale del while / do-while más interno
class BreakStmt : public Stmt {
public:
    BreakStmt();
    void accept(Visitor* visitor) override;
    ~BreakStmt();
};

// Sentencia return: return CExp (sale de la función devolviendo el valor)
class ReturnStmt : public Stmt {
public:
    Exp* exp;
    void accept(Visitor* visitor) override;
    ReturnStmt();
    ~ReturnStmt();
};

// ==================== Declaraciones ====================

// Declaración de variables: var tipo id (, id)* ;   (tipo puede ser int, bool, int*, int**, stack<int>, ...)
class VarDec {
public:
    vector<string> variables;
    string tipo;
    VarDec();
    ~VarDec();
    void accept(Visitor* visitor);
};

// Cuerpo: primero las declaraciones y luego las sentencias
class Body {
public:
    vector<VarDec*> declaraciones;
    vector<Stmt*> sentencias;
    Body();
    ~Body();
    void accept(Visitor* visitor);
};

// Declaración de función: fun tipo nombre(tipo id, ...) Body endfun
class FunDec {
public:
    string nombre;
    string tipo;
    vector<string> parametros_tipos;
    vector<string> parametros_id;
    Body* cuerpo;
    void accept(Visitor* visitor);
    FunDec();
    ~FunDec();
};

// ==================== Programa ====================

// Raíz del AST: la lista de funciones del programa (una debe ser main)
class Programa {
public:
    vector<FunDec*> funciones;
    void accept(Visitor* visitor);
    ~Programa();
    Programa();
};

#endif // AST_H
