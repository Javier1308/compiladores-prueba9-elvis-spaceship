#ifndef INLINE_VISITOR_H
#define INLINE_VISITOR_H

#include <string>
#include <set>
#include <unordered_map>
#include "visitor.h"

// ==================== InlineVisitor ====================

// Visitor que expande en línea las llamadas a funciones de una sola expresión:
//   fun int cuadrado(int x) return x * x endfun
//   print(cuadrado(a) + 1)   ->   print(a * a + 1)
// Una función se puede expandir si su cuerpo es solo "return e" (sin var) y e no usa '&'
// (&x de un parámetro es la dirección de su copia local, no la del argumento).
// Una llamada se expande si todos sus argumentos son números o variables: un parámetro
// puede aparecer varias veces en e, y repetir una llamada o un pop cambiaría el programa.
// No se expande una función dentro de sí misma (recursión directa o indirecta).
// Debe ejecutarse después del TypeChecker y antes del ConstanteVisitor.
class InlineVisitor : public Visitor {
public:
    void expandir(Programa* program);

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
    unordered_map<string, FunDec*> funciones;        // funciones del programa, por nombre
    unordered_map<string, ReturnStmt*> expandibles;  // funciones que se pueden expandir: su único return
    unordered_map<string, Exp*> sustitucion;         // parámetro -> argumento de la llamada que se expande
    set<string> expandiendo;                         // funciones que se están expandiendo (evita recursión)

    Exp* reemplazo = nullptr;       // expresión que queda en lugar de la visitada
    bool esSimple = false;          // la expresión visitada es un número o una variable

    // Primera pasada: se buscan las funciones expandibles sin cambiar nada
    bool buscando = false;
    ReturnStmt* retorno = nullptr;  // la sentencia visitada si es un return, nullptr si no
    bool usaDireccion = false;      // se visitó un '&'

    Exp* reemplazar(Exp* exp);      // expande exp y devuelve la expresión que la reemplaza
};

#endif // INLINE_VISITOR_H
