#ifndef GENCODE_VISITOR_H
#define GENCODE_VISITOR_H

#include <vector>
#include <string>
#include <unordered_map>
#include "visitor.h"
#include "environment.h"
#include "typechecker.h"

// ==================== GencodeVisitor ====================

// Visitor que genera código ensamblador x86-64 a partir del AST.
// Convención de llamada (System V): los argumentos van en
// %rdi, %rsi, %rdx, %rcx, %r8, %r9 y el resultado vuelve en %rax.
class GencodeVisitor : public Visitor {
public:
    ostream& out;
    TypeChecker* checker;                    // ya ejecutado: dice cuántas variables tiene cada función
    int contador = 1;                        // siguiente posición libre en la pila de la función actual
    Environment<int> posicion;               // posición en la pila de cada variable / parámetro
    Environment<Type> tipos;                 // tipo de cada variable (y su último valor)
    int etiquetas = 0;                       // contador para generar etiquetas únicas
    vector<string> finDeBucle;               // etiqueta de salida de cada bucle abierto (para break)
    unordered_map<string, FunDec*> funciones; // funciones del programa, por nombre
    string funcionActual;                    // función que se está generando (para return)
    int pila = 0;                            // pushq pendientes (para alinear la pila en cada call)
    bool usaPilaVacia = false;               // true si hay pop/top: se genera la rutina de error

    GencodeVisitor(ostream& out, TypeChecker* checker);
    void gencode(Programa* program);

    Type visit(BinaryExp* exp) override;
    Type visit(NumberExp* exp) override;
    Type visit(IdExp* exp) override;
    Type visit(FCallExp* exp) override;
    Type visit(IndexExp* exp) override;
    Type visit(DerefExp* exp) override;
    Type visit(AddrExp* exp) override;
    Type visit(MallocExp* exp) override;
    Type visit(StackOpExp* exp) override;
    Type visit(SpaceshipExp* exp) override;
    Type visit(ElvisExp* exp) override;
    void visit(AssignStmt* stm) override;
    void visit(StackStmt* stm) override;
    void visit(StoreStmt* stm) override;
    void visit(FreeStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(IfStmt* stm) override;
    void visit(WhileStmt* stm) override;
    void visit(DoWhileStmt* stm) override;
    void visit(BreakStmt* stm) override;
    void visit(ReturnStmt* stm) override;
    void visit(VarDec* dec) override;
    void visit(Body* body) override;
    void visit(FunDec* fun) override;
    void visit(Programa* program) override;

private:
    void operar(BinaryExp* exp, Type l);     // %rax = %rax op %rcx (left en %rax, right en %rcx)
    Type direccion(Exp* lvalue);             // deja en %rax la dirección de x, a[i] o *p
    void llamar(const string& funcion);      // call con la pila alineada a 16 bytes
};

#endif // GENCODE_VISITOR_H
