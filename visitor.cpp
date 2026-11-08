#include "ast.h"
#include "visitor.h"

// =====================================================
// ====================== accept =======================
// =====================================================

Type BinaryExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type NumberExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type IdExp::accept(Visitor* visitor)      { return visitor->visit(this); }
Type FCallExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type IndexExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type DerefExp::accept(Visitor* visitor)   { return visitor->visit(this); }
Type AddrExp::accept(Visitor* visitor)    { return visitor->visit(this); }
Type MallocExp::accept(Visitor* visitor)  { return visitor->visit(this); }
Type StackOpExp::accept(Visitor* visitor) { return visitor->visit(this); }
Type SpaceshipExp::accept(Visitor* visitor) { return visitor->visit(this); }
Type ElvisExp::accept(Visitor* visitor)   { return visitor->visit(this); }

void AssignStmt::accept(Visitor* visitor)  { visitor->visit(this); }
void StackStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void StoreStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void FreeStmt::accept(Visitor* visitor)    { visitor->visit(this); }
void PrintStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void IfStmt::accept(Visitor* visitor)      { visitor->visit(this); }
void WhileStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void DoWhileStmt::accept(Visitor* visitor) { visitor->visit(this); }
void BreakStmt::accept(Visitor* visitor)   { visitor->visit(this); }
void ReturnStmt::accept(Visitor* visitor)  { visitor->visit(this); }

void VarDec::accept(Visitor* visitor)   { visitor->visit(this); }
void Body::accept(Visitor* visitor)     { visitor->visit(this); }
void FunDec::accept(Visitor* visitor)   { visitor->visit(this); }
void Programa::accept(Visitor* visitor) { visitor->visit(this); }
