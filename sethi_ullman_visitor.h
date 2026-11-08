#ifndef SETHI_ULLMAN_VISITOR_H
#define SETHI_ULLMAN_VISITOR_H

#include "visitor.h"

// ==================== SethiUllmanVisitor ====================

// Visitor que llena la etiqueta de Sethi-Ullman de cada Exp: la cantidad de
// registros necesaria para evaluarla. Se calcula de las hojas hacia la raíz:
//   hoja (número o variable) -> 1 si es hijo izquierdo, 0 si es hijo derecho
//   left op right -> max(l1, l2) si l1 != l2;  l1 + 1 si l1 == l2
//   otras expresiones -> la mayor etiqueta de sus subexpresiones (mínimo 1)
class SethiUllmanVisitor : public Visitor {
public:
    void etiquetar(Programa* program);

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
    bool hijoDerecho = false;     // true justo al visitar el hijo derecho de un BinaryExp
    void hoja(Exp* exp);          // etiqueta una hoja: 0 si es hijo derecho, 1 si no
};

#endif // SETHI_ULLMAN_VISITOR_H
