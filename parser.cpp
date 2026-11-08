#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// Inicializa el parser leyendo el primer token
Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

// Si el token actual es del tipo dado, avanza y devuelve true
bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

// Indica si el token actual es del tipo dado, sin avanzar
bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

// Avanza al siguiente token
bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

// Indica si se llegó al final de la entrada
bool Parser::isAtEnd() {
    return current->type == Token::END;
}

// Programa ::= FunDec+
Programa* Parser::parseProgram() {
    Programa* p = new Programa();
    if (!check(Token::FUN)) {
        throw runtime_error("Error sintáctico: el programa debe empezar con una función (fun)");
    }
    while (check(Token::FUN)) {
        p->funciones.push_back(parseFunDec());
    }
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico: se esperaba 'fun' o fin de archivo");
    }

    cout << "Parseo exitoso" << endl;
    return p;
}

// tipo ::= (int | bool) '*'* | stack '<' tipo '>'     (el tipo de los elementos no puede ser otra pila)
string Parser::parseTipo() {
    if (!match(Token::ID) || (previous->text != "int" && previous->text != "bool" && previous->text != "stack")) {
        throw runtime_error("Error sintáctico: se esperaba un tipo (int, bool, int*, int**, stack<int>, ...)");
    }
    string tipo = previous->text;
    if (tipo == "stack") {
        if (!match(Token::LT)) {
            throw runtime_error("Error sintáctico: se esperaba '<' después de stack (por ejemplo stack<int>)");
        }
        string elemento = parseTipo();
        if (elemento.rfind("stack", 0) == 0) {
            throw runtime_error("Error sintáctico: una pila no puede contener otra pila");
        }
        if (!match(Token::GT)) {
            throw runtime_error("Error sintáctico: se esperaba '>' al final de stack<" + elemento);
        }
        return "stack<" + elemento + ">";
    }
    // int** llega como ID seguido de POW, por eso se aceptan '*' y '**'
    while (match(Token::MUL) || match(Token::POW)) {
        tipo += previous->text;
    }
    return tipo;
}

// FunDec ::= fun tipo id '(' [ tipo id (',' tipo id)* ] ')' Body endfun
FunDec* Parser::parseFunDec() {
    match(Token::FUN);
    FunDec* f = new FunDec();
    f->tipo = parseTipo();
    if (!match(Token::ID)) {
        throw runtime_error("Error sintáctico: se esperaba el nombre de la función");
    }
    f->nombre = previous->text;
    if (!match(Token::LPAREN)) {
        throw runtime_error("Error sintáctico: se esperaba '(' después de " + f->nombre);
    }
    if (!check(Token::RPAREN)) {
        do {
            f->parametros_tipos.push_back(parseTipo());
            if (!match(Token::ID)) {
                throw runtime_error("Error sintáctico: se esperaba el nombre del parámetro");
            }
            f->parametros_id.push_back(previous->text);
        } while (match(Token::COMMA));
    }
    if (!match(Token::RPAREN)) {
        throw runtime_error("Error sintáctico: se esperaba ')' en la función " + f->nombre);
    }
    f->cuerpo = parseBody();
    if (!match(Token::ENDFUN)) {
        throw runtime_error("Error sintáctico: se esperaba 'endfun' al final de " + f->nombre);
    }
    return f;
}

// Body ::= VarDec* StmtList
Body* Parser::parseBody() {
    Body* b = new Body();
    while (check(Token::VAR)) {
        b->declaraciones.push_back(parseVarDec());
    }
    for (Stmt* s : parseStmtList()) {
        b->sentencias.push_back(s);
    }
    return b;
}

// VarDec ::= var tipo id (',' id)* ';'
VarDec* Parser::parseVarDec() {
    match(Token::VAR);
    VarDec* d = new VarDec();
    d->tipo = parseTipo();
    do {
        if (!match(Token::ID)) {
            throw runtime_error("Error sintáctico: se esperaba un nombre de variable");
        }
        d->variables.push_back(previous->text);
    } while (match(Token::COMMA));
    if (!match(Token::SEMICOL)) {
        throw runtime_error("Error sintáctico: se esperaba ';' al final de la declaración");
    }
    return d;
}

// StmtList ::= Stmt (';' Stmt)*
list<Stmt*> Parser::parseStmtList() {
    list<Stmt*> slist;
    slist.push_back(parseStmt());
    while (match(Token::SEMICOL)) {
        slist.push_back(parseStmt());
    }
    return slist;
}

// Stmt ::= id '=' CExp | print '(' CExp ')'
//        | id ('[' CExp ']')+ '=' CExp
//        | id sufijos '.' (push '(' CExp ')' | pop '(' ')')
//        | '*' factor '=' CExp
//        | free '(' CExp ')'
//        | if CExp then StmtList [ else StmtList ] endif
//        | while CExp do StmtList enddo
//        | do StmtList while CExp
//        | break
//        | return CExp
Stmt* Parser::parseStmt() {
    Exp* e;
    if (match(Token::ID)) {
        string variable = previous->text;
        if (check(Token::LBRACKET) || check(Token::DOT)) {   // a[i] = e  |  m[i][j] = e  |  s.push(e)
            Exp* destino = parseIndices(new IdExp(variable));
            if (StackOpExp* metodo = dynamic_cast<StackOpExp*>(destino)) {
                if (metodo->op != PUSH_OP && metodo->op != POP_OP) {
                    throw runtime_error("Error sintáctico: ." + metodo->metodo()
                                        + "() no es una sentencia (solo push y pop)");
                }
                return new StackStmt(metodo);
            }
            if (!match(Token::ASSIGN)) {
                throw runtime_error("Error sintáctico: se esperaba '=' después de " + variable + "[...]");
            }
            e = parseCExp();
            return new StoreStmt(destino, e);
        }
        match(Token::ASSIGN);
        e = parseCExp();
        return new AssignStmt(variable, e);
    }
    else if (check(Token::MUL) || check(Token::POW)) {   // *p = e  |  **pp = e
        Exp* destino = parseFactor();
        if (!match(Token::ASSIGN)) {
            throw runtime_error("Error sintáctico: se esperaba '=' después de *puntero");
        }
        e = parseCExp();
        return new StoreStmt(destino, e);
    }
    else if (match(Token::FREE)) {
        if (!match(Token::LPAREN)) throw runtime_error("Error sintáctico: se esperaba '(' tras free");
        e = parseCExp();
        if (!match(Token::RPAREN)) throw runtime_error("Error sintáctico: se esperaba ')' en free");
        return new FreeStmt(e);
    }
    else if (match(Token::PRINT)) {
        match(Token::LPAREN);
        e = parseCExp();
        match(Token::RPAREN);
        return new PrintStmt(e);
    }
    else if (match(Token::IF)) {
        e = parseCExp();
        if (!match(Token::THEN)) {
            throw runtime_error("Error sintáctico: se esperaba 'then'");
        }
        list<Stmt*> thenList = parseStmtList();
        list<Stmt*> elseList;
        if (match(Token::ELSE)) {
            elseList = parseStmtList();
        }
        if (!match(Token::ENDIF)) {
            throw runtime_error("Error sintáctico: se esperaba 'endif'");
        }
        return new IfStmt(e, thenList, elseList);
    }
    else if (match(Token::WHILE)) {
        e = parseCExp();
        if (!match(Token::DO)) {
            throw runtime_error("Error sintáctico: se esperaba 'do'");
        }
        loopDepth++;
        list<Stmt*> body = parseStmtList();
        loopDepth--;
        if (!match(Token::ENDDO)) {
            throw runtime_error("Error sintáctico: se esperaba 'enddo'");
        }
        return new WhileStmt(e, body);
    }
    else if (match(Token::DO)) {
        loopDepth++;
        list<Stmt*> body = parseStmtList();
        loopDepth--;
        if (!match(Token::WHILE)) {
            throw runtime_error("Error sintáctico: se esperaba 'while' al final del do");
        }
        e = parseCExp();
        return new DoWhileStmt(body, e);
    }
    else if (match(Token::RETURN)) {
        ReturnStmt* r = new ReturnStmt();
        r->exp = parseCExp();
        return r;
    }
    else if (match(Token::BREAK)) {
        if (loopDepth == 0) {
            throw runtime_error("Error sintáctico: 'break' fuera de un bucle");
        }
        return new BreakStmt();
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}

// CExp ::= RExp [ ?: CExp ]     (elvis: menor precedencia y asociativo a la derecha)
Exp* Parser::parseCExp() {
    Exp* l = parseRExp();
    if (match(Token::ELVIS)) {
        Exp* r = parseCExp();
        return new ElvisExp(l, r);
    }
    return l;
}

// RExp ::= SExp [ (<= | < | > | >= | == | !=) SExp ]
Exp* Parser::parseRExp() {
    Exp* l = parseSExp();
    BinaryOp op;
    if (match(Token::LE))      op = LE_OP;
    else if (match(Token::LT)) op = LT_OP;
    else if (match(Token::GT)) op = GT_OP;
    else if (match(Token::GE)) op = GE_OP;
    else if (match(Token::EQ)) op = EQ_OP;
    else if (match(Token::NE)) op = NE_OP;
    else return l;
    Exp* r = parseSExp();
    return new BinaryExp(l, r, op);
}

// SExp ::= BExp [ <=> BExp ]     (spaceship: más precedencia que las comparaciones)
Exp* Parser::parseSExp() {
    Exp* l = parseBExp();
    if (match(Token::SPACESHIP)) {
        Exp* r = parseBExp();
        return new SpaceshipExp(l, r);
    }
    return l;
}

// BExp ::= expr { (+ | -) expr }*
Exp* Parser::parseBExp() {
    Exp* l = parseExpr();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS) {
            op = PLUS_OP;
        }
        else {
            op = MINUS_OP;
        }
        Exp* r = parseExpr();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// expr ::= term { (* | /) term }*
Exp* Parser::parseExpr() {
    Exp* l = parseTerm();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL) {
            op = MUL_OP;
        }
        else {
            op = DIV_OP;
        }
        Exp* r = parseTerm();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// term ::= '-' term | factor [ ** factor ]     (-e se arma como 0 - e)
Exp* Parser::parseTerm() {
    if (match(Token::MINUS)) {
        return new BinaryExp(new NumberExp(0), parseTerm(), MINUS_OP);
    }
    Exp* l = parseFactor();
    if (match(Token::POW)) {
        Exp* r = parseFactor();
        l = new BinaryExp(l, r, POW_OP);
    }
    return l;
}

// factor ::= '*' factor | '&' factor | primary ( '[' CExp ']' | '.' metodo '(' [CExp] ')' )*
Exp* Parser::parseFactor() {
    if (match(Token::MUL)) {
        return new DerefExp(parseFactor());
    }
    else if (match(Token::POW)) {                   // **p llega como un solo token
        return new DerefExp(new DerefExp(parseFactor()));
    }
    else if (match(Token::AMP)) {
        return new AddrExp(parseFactor());
    }
    return parseIndices(parsePrimary());
}

// Agrega a base cada sufijo que venga a continuación:
//   índices '[' CExp ']' (m[i][j]) y métodos de pila '.' metodo '(' [CExp] ')' (s.push(1), s.top())
Exp* Parser::parseIndices(Exp* base) {
    while (true) {
        if (match(Token::LBRACKET)) {
            Exp* indice = parseCExp();
            if (!match(Token::RBRACKET)) {
                throw runtime_error("Error sintáctico: se esperaba ']'");
            }
            base = new IndexExp(base, indice);
        }
        else if (match(Token::DOT)) {
            if (!match(Token::ID)) {
                throw runtime_error("Error sintáctico: se esperaba un método después de '.'");
            }
            string metodo = previous->text;
            StackOp op;
            if (metodo == "push")       op = PUSH_OP;
            else if (metodo == "pop")   op = POP_OP;
            else if (metodo == "top")   op = TOP_OP;
            else if (metodo == "empty") op = EMPTY_OP;
            else if (metodo == "size")  op = SIZE_OP;
            else throw runtime_error("Error sintáctico: método desconocido '" + metodo
                                     + "' (una pila tiene push, pop, top, empty y size)");
            if (!match(Token::LPAREN)) {
                throw runtime_error("Error sintáctico: se esperaba '(' después de ." + metodo);
            }
            Exp* valor = nullptr;
            if (op == PUSH_OP) valor = parseCExp();
            if (!match(Token::RPAREN)) {
                throw runtime_error("Error sintáctico: se esperaba ')' en ." + metodo + "(...)");
            }
            base = new StackOpExp(base, op, valor);
        }
        else {
            return base;
        }
    }
}

// primary ::= number | true | false | '(' CExp ')' | id | id '(' [CExp (',' CExp)*] ')'
//           | malloc '(' CExp ')' | sizeof '(' tipo ')'
Exp* Parser::parsePrimary() {
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1, true);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0, true);
    }
    else if (match(Token::LPAREN)) {
        Exp* e = parseCExp();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        string nombre = previous->text;
        if (match(Token::LPAREN)) {                 // llamada: id '(' [CExp (',' CExp)*] ')'
            FCallExp* call = new FCallExp();
            call->nombre = nombre;
            if (!check(Token::RPAREN)) {
                do {
                    call->argumentos.push_back(parseCExp());
                } while (match(Token::COMMA));
            }
            if (!match(Token::RPAREN)) {
                throw runtime_error("Error sintáctico: se esperaba ')' en la llamada a " + nombre);
            }
            return call;
        }
        return new IdExp(nombre);
    }
    else if (match(Token::MALLOC)) {
        if (!match(Token::LPAREN)) throw runtime_error("Error sintáctico: se esperaba '(' tras malloc");
        Exp* bytes = parseCExp();
        if (!match(Token::RPAREN)) throw runtime_error("Error sintáctico: se esperaba ')' en malloc");
        return new MallocExp(bytes);
    }
    else if (match(Token::SIZEOF)) {
        if (!match(Token::LPAREN)) throw runtime_error("Error sintáctico: se esperaba '(' tras sizeof");
        parseTipo();
        if (!match(Token::RPAREN)) throw runtime_error("Error sintáctico: se esperaba ')' en sizeof");
        return new NumberExp(8);                    // int, bool y punteros ocupan 8 bytes
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
