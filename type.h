#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <ostream>

using namespace std;

// Resultado de visitar un nodo del AST: su tipo y su valor.
// El valor se guarda en el atributo que corresponde al tipo:
//   INT  -> intValue
//   BOOL -> boolValue
//   PTR  -> ninguno (una dirección solo se conoce al ejecutar)
//   VOID -> ninguno (sentencias)
// Un puntero se describe con el tipo base al que apunta y su nivel:
//   int* -> base INT, nivel 1      int** -> base INT, nivel 2
//   void* (lo que devuelve malloc) -> base VOID, nivel 1
// Una pila guarda en base y nivel el tipo de sus elementos:
//   stack<int> -> base INT, nivel 0    stack<int*> -> base INT, nivel 1
class Type {
public:
    enum Kind { VOID, INT, BOOL, PTR, STACK };

    Kind kind;
    int intValue;
    bool boolValue;
    Kind base;                    // PTR: tipo base (INT, BOOL o VOID); STACK: base del elemento
    int nivel;                    // PTR: cantidad de '*'; STACK: '*' del elemento

    Type();                       // VOID
    Type(int v);                  // INT con valor v
    Type(bool b);                 // BOOL con valor b
    static Type puntero(Kind base, int nivel);   // PTR (base, nivel)
    static Type pila(const Type& elemento);      // STACK de elementos de ese tipo
    static Type desdeNombre(const string& nombre); // "int", "bool*", "int**", "stack<int>", ...

    bool isInt() const;
    bool isBool() const;
    bool isVoid() const;
    bool isPtr() const;
    bool isVoidPtr() const;       // void*: puntero genérico de malloc
    bool isStack() const;
    Type elemento() const;        // tipo de los elementos de una pila (stack<int> -> int)
    Type apuntado() const;        // tipo de *p (int** -> int*, int* -> int)
    Type direccion() const;       // tipo de &x (int -> int*, int* -> int**)
    bool mismoTipo(const Type& otro) const;
    bool aceptaValor(const Type& valor) const;   // ¿se puede asignar valor a algo de este tipo?
    string kindName() const;

    friend ostream& operator<<(ostream& outs, const Type& t);
};

#endif // TYPE_H
