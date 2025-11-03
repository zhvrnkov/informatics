# UM basic idea
- UM can simulate any M (just Turing machine)
- UM tape starts with M description and "::" at the end

UM tape:
[ M_description... :: ]

- each M has a complete configuration at every step
- complete configuration is: new state, next scanned symbol, snapshot of a tape
- for example this machine M completes configurations:
```raw
q1 * [P0, R] q2
q2 * [P1, R] q1

complete configs:
[0] = [], q1, .  # next state is q1, next scanned symbol is empty, snapshot of empty tape
[1] = [0], q2, . # next state is q2, ...
[0] = [0, 1], q1, .
...
```
- to execute a step of M you need to know its state and its symbol (to choose the proper state) and the tape
> but we'd rather need new head position then next scanned symbol because
- each complete configuration can be encoded using S.D. (Standard Description):
```raw
[0] = DAD
[1] = DCDAAD
[0] = DCDCCDAD
DA = q1
DAA = q2
DAAA = q3
...
D = .
DC = 0
DCC = 1
DCCC = ...

A for states
C for symbols
```
- !!! complete configurations endoing is [EVERYTHING LEFT FROM SCANNED SYMBOL] [STATE] [SCANNED SYMBOL] [EVERYTHING RIGHT FROM SCANNED SYMBOL]. with that sceheme you don't need to explicitly encode head position
- if UM is simulating this M, then initial tape of a UM will look like this:
[ee;DADDCRDAA;DAADDCCRDA::]
> this tape has encoded state of M:
```raw
q1 * [P0, R] q2
q2 * [P1, R] q1
```
- ee for determining beginning of the tape
- each state is beginning with a ";"
- the main working idea of UM is that it can determine every complete configuration of M and determining c.c. is basically executing M
- so after first 2 execution steps of UM the tape will look like this:
```raw
[...:::cc[0]:cc[1]]
```
- each cc is separated by a ":"
- cc is basically next state + next symbol + tape snapsot
- each state of an M is basically state + symbol + ops + next state
- so based on a cc we can easily determine next cc:
 1. we got a cc[i]
 2. we find a M's state based on cc.next_state and cc.nex_symbol (thats pretty easy, just compare strings)
 3. execute M's state ops on cc.tape and get a new tape + new head position
 4. cc[i+1].next_state = M's next state, tape = new tape, next_symbol = new_tape[new_head_position]

# UM implementation
- so initials of UM is this:
    tape = [{M_states_description}::]
    state = begin
- begin state of UM looks like this:
```raw
begin * find(begin1, begin1, ::)
begin1 * [R, R, P:, R, R, PD, R, R, PA] anf

> note that :: is a single symbol
```
- double shifts to R is because we write to F-squares, E-squares are for private calculations
- so after begin1 next state is anf and tape is:
    tape = [{M_states_description}:::DA] (ignoring E-squares)
- DA make sense as initial configuration for any M. DA means that next configuration is q1 (begin) and there is no scanned symbol and tape is empty (any M starts from this configuration)
- so cc[0] is just "q1"



==========================
```raw
ee;.D.A.D.DuCuRuDyAyAy;.D.A.A.D.D.C.C.R.D.A.|.:.D.A.D.:.0.:.D.C.D.A.A.D.:.1.:.
[0.] + DXR + [] => 0Xq.
[0.] + DXN + [] => 0qX.
[0.] + DXL + [] => q0X.

[01.] + DXR => 01Xq.
[01.] + DXN => 01qX
[01.] + DXL => 0q1X

[01] q [a12] + PXR => 01
[01] q [a12] + PXN =>
[01] q [a12] + PXL =>


:.DvCvDxD.A.D.C.DwDwCw:
  DC  D DA  DC  D DC
  0   . q1  0   . 0
[0.q10.0              ]
y = new state
v = left symbols[0..<N]
x = left symbols[N]
u = symbol from P
w = right symbols w/o current

PN = v x y u w
PL = v y x u w
PR = v x u y w

```


