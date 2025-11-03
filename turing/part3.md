# Implication
X -> Y
if X, then Y
X|Y|
-----
0|0|0
0|1|0
1|0|0
1|1|1

если я буду хорошо учиться, то я сдам экзамен
либо я буду плохо учиться, либо я сдам экзамен

хорошо учиться | сдам экзамен | 
0              | 0            | 1
0              | 1            | 1
1              | 0            | 0
1              | 1            | 1

если я сложу 2 и 2, то получу 4
A | B | 
0 | 0 | 1
0 | 1 | 1
1 | 0 | 0
1 | 1 | 1

если человек, то смертен
человек | смертен
0 0 1
0 1 1
1 0 0
1 1 1

# Axioms

# 12. Logic and Computability
propositional logic is restricted to just manipulating T and F
you can apply any transformations to T and F with operations such as OR, AND, NOT and etc
but that's it

first-order predicate logic is applying propositional logic to sets. the predicates and quantifiers are introduced:
- IsPrime(x) is predicate
- "for all", "there exists (for any)" are quantifiers
- you can apply predicates to sets via quantifiers. quantifiers are reduce operations basically, because at the end of eash logic expression you need to get either T or F
- without quantifiers, the expressions can result in [T or F]. quantifiers :: [T or F] -> T or F

then there are a set of axioms from Hilbert's Principia Mathematica:
(a) X or X -> X
(b) X -> X or Y
(c) X or Y -> Y or X
(d) (X -> Y) -> (Z or X -> Z or Y)
(e) all_x(F(X)) -> F(y)
(f) F(y) -> there_exist_x(F(x))

if you can define a pure function such as:
F :: Axioms -> Theorem
which imply that Theorem is derived from Axioms (since F is pure), then this F is a proof of the Theorem. 
it also works in other direction: if you have Formula A and can find such F where F(Axioms) = A, then A is a Theorem with proof F

So to find theorems you have 2 ways:
1. go from top to the bottom: go from axions to the theorem by deriving theorem from axioms: F :: Axioms -> Theorem
2. try to reach top from the bottom: get a formula and try to convert it to set of axioms: F :: Formula -> Axioms?
> Axioms are under ? because Formula can be not provable

with that framework in mind, we face the question:
can we derive ALL theorems from the axioms?
or in other words is there a theorem that cannot be derived from axioms?

Godel prooved that set of first order logic axioms are complete: all theorems can be derived from those axioms.

but if we add basic arithmetic to this first order logic, then system becomes incomplete: there exits a theorem that cannot be derived from axioms.

the general algorithm of prooving any formula is basically this:
```python
def theorems_generator(axioms):
    while True:
        # ... some work
        yield theorem

def is_formula_proovable(F, axioms):
    for theorem in theorems_generator(axioms):
        if F == theorem:
            return True
```
even for first order logic this program might run infinitely many times (if F is not proovable, we don't know when to stop).
but for first order logic + arithmetic, Godel did proove that theorems_generator will not generate all theorems and we got even worse situation:
1. for first order logic the program runs to infinity for all not proovable F
2. for fol + arith the program runs to infinity even for proovable F (cause it will not be yield by theorems_generator due to incompletness)
Godel incompletness theorem strongly imply that such system is undecidable

but Turing wants to proof the decidability problem using Turing Machine. Such Turing Machine K will basically implement above python code if generating all possible theorems from the axioms and find desired F in this theorems to proof that F is a theorem itself.
to do that Turing needs to encode the first order logic axioms and arithmetic rules into its K. so the initial K's tape will have:
[first order logic axioms] + [additional axioms]
the machine itself needs to implement theorems_generator and algorithm to find F in those theorems
more specifically additional axioms includes Peano Axioms 

let alpha be a sequence and G_alpha(x) = alpha[x] == 1
but alpha itself can be defined by G_alpha. G_alpha is just reading alpha at index x

for example we can define alpha = 1010 as G_a(0) & -G_a(1) & G_a(0) & G_a(1) = U
A_n = U & F^n -> G_a(u^n)
B_n = U & F^n -> -G_a(u^n)
A_n 

