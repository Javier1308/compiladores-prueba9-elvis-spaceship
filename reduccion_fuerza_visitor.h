#ifndef REDUCCION_FUERZA_VISITOR_H
#define REDUCCION_FUERZA_VISITOR_H

#include <string>
#include "visitor.h"

// ==================== ReduccionFuerzaVisitor ====================

// Visitor que reemplaza en el AST operaciones por otras equivalentes y más baratas.
//   Simplificación algebraica:  x + 0 -> x    x * 1 -> x    x / 1 -> x
//   Reducción de fuerza:        x * 2 -> x + x    2 * x -> x + x    x ** 2 -> x * x
// La reducción de fuerza solo se aplica si x es una variable: x pasa a evaluarse
// dos veces, y con una llamada o un pop eso cambiaría lo que hace el programa.
// Reconoce el 0, el 1 y el 2 por esConstante / valorConstante, así que debe
// ejecutarse después del ConstanteVisitor.
class ReduccionFuerzaVisitor : public Visitor {
public:
    void reducir(Programa* program);

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
    Exp* reemplazo = nullptr;     // expresión que queda en lugar de la visitada
    string variable;              // nombre de la expresión visitada si es una variable, "" si no
    Exp* reemplazar(Exp* exp);    // reduce exp y devuelve la expresión que la reemplaza
};

#endif // REDUCCION_FUERZA_VISITOR_H
