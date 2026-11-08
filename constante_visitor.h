#ifndef CONSTANTE_VISITOR_H
#define CONSTANTE_VISITOR_H

#include "visitor.h"

// ==================== ConstanteVisitor ====================

// Visitor que marca las expresiones cuyo valor se conoce en compilación.
// Llena esConstante y valorConstante de cada Exp, de las hojas hacia la raíz
// (efecto cascada): un literal es constante, y una operación es constante si
// sus operandos lo son, así (2 + 3) * 4 queda marcada con el valor 20.
// Debe ejecutarse después del TypeChecker (asume un programa bien tipado).
class ConstanteVisitor : public Visitor {
public:
    void marcar(Programa* program);

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
    void constante(Exp* exp, long long valor);   // marca exp como constante con ese valor
    void noConstante(Exp* exp);                  // marca exp como no constante
};

#endif // CONSTANTE_VISITOR_H
