#ifndef VISITOR_H
#define VISITOR_H

#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include "ast.h"
#include "type.h"

// Tamaño de cada valor: int, bool, punteros y pilas ocupan 8 bytes (en la pila y en el heap)
const int TAM_VARIABLE = 8;

// Una variable stack guarda la dirección de una cabecera en el heap: [tope, tamaño].
// Los elementos forman una lista enlazada de nodos: [valor, siguiente].
// Pasar una pila a una función comparte la misma pila (se copia la dirección).
const int TAM_CABECERA_PILA = 16;
const int TAM_NODO_PILA = 16;

// Interfaz Visitor: una operación visit por cada nodo del AST.
// Las expresiones devuelven Type; las sentencias y declaraciones, void.
class Visitor {
public:
    // Expresiones
    virtual Type visit(BinaryExp* exp) = 0;
    virtual Type visit(NumberExp* exp) = 0;
    virtual Type visit(IdExp* exp) = 0;
    virtual Type visit(FCallExp* exp) = 0;
    virtual Type visit(IndexExp* exp) = 0;
    virtual Type visit(DerefExp* exp) = 0;
    virtual Type visit(AddrExp* exp) = 0;
    virtual Type visit(MallocExp* exp) = 0;
    virtual Type visit(StackOpExp* exp) = 0;
    virtual Type visit(SpaceshipExp* exp) = 0;
    virtual Type visit(ElvisExp* exp) = 0;
    // Sentencias
    virtual void visit(AssignStmt* stm) = 0;
    virtual void visit(StackStmt* stm) = 0;
    virtual void visit(StoreStmt* stm) = 0;
    virtual void visit(FreeStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(IfStmt* stm) = 0;
    virtual void visit(WhileStmt* stm) = 0;
    virtual void visit(DoWhileStmt* stm) = 0;
    virtual void visit(BreakStmt* stm) = 0;
    virtual void visit(ReturnStmt* stm) = 0;
    // Declaraciones y programa
    virtual void visit(VarDec* dec) = 0;
    virtual void visit(Body* body) = 0;
    virtual void visit(FunDec* fun) = 0;
    virtual void visit(Programa* program) = 0;
};

#endif // VISITOR_H
