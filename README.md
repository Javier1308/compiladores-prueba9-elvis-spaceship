# Prueba 9 – ElvisExp y SpaceshipExp

Compilador base de la semana 13 extendido con dos operadores:

| Operador | Sintaxis | Resultado |
|---|---|---|
| Spaceship | `a <=> b` | `-1` si `a < b`, `0` si `a == b`, `1` si `a > b` (int) |
| Elvis | `a ?: b` | `a` si `a != 0`; si no, `b` (evalúa `a` una sola vez y `b` solo si hace falta) |

## Gramática de expresiones (de menor a mayor precedencia)

```
CExp   ::= RExp [ ?: CExp ]                       // elvis, asociativo a la derecha
RExp   ::= SExp [ (<= | < | > | >= | == | !=) SExp ]
SExp   ::= BExp [ <=> BExp ]                      // spaceship
BExp   ::= expr { (+ | -) expr }*
expr   ::= term { (* | /) term }*
term   ::= '-' term | factor [ ** factor ]        // -e se arma como 0 - e
```

## Cambios

- **Scanner / tokens**: tokens `SPACESHIP` (`<=>`) y `ELVIS` (`?:`); los identificadores aceptan `_` (`safe_div`, `clamp_score`).
- **Parser**: niveles `RExp` y `SExp`, elvis encadenable y menos unario (`== -1`).
- **AST / Visitor**: nodos `SpaceshipExp` y `ElvisExp` con `clonar()`.
- **TypeChecker**: `int <=> int -> int`; `a ?: b` con `a` y `b` del mismo tipo.
- **InlineVisitor**: expande dentro de ambos operadores.
- **ConstanteVisitor**: pliega `<=>` con operandos constantes y `k ?: x` cuando `k` es constante.
- **ReduccionFuerzaVisitor**: `k ?: x -> k` (k != 0) y `0 ?: x -> x`.
- **SethiUllmanVisitor**: etiquetas para ambos nodos.
- **GencodeVisitor**: `<=>` con `cmpq` + `setg`/`setl`; `?:` con `cmpq $0` + salto (cortocircuito).

## Uso

```bash
python3 run_all_inputs.py      # compila, corre inputs/ y escribe outputs/ + outputs/reporte.txt
```

- `inputs/`: inputs de la Prueba 9 (semana 13).
- `expected/`: salida esperada de cada input.
- `inputs_base/`: inputs originales del código base (regresión).
