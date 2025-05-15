# Expected value
E(xs) => sum([x * probability(x) for x in xs])

E(xs) is expected value (in general) among xs

# Surprise
P(x) => probability of x (from 0 to 1)

When P(x) is high, we expect the x
If the x occur and P(x) is really high we're not surprised
But if x occur and P(x) is really low then we're surprised
Which means that
The surpise is inverse of P

But it can't just be 1/P because in case P(x) = 1 we want surprise to be 0
And in case where P(x) -> 0 we want surprise -> inf

so

S(x) = log(1 / P(x))

# Entropy
Entropy is Expected value of Surprise <=> E(S(xs)) => E([log(1 / P(x)) for x in xs])

Entropy is average surprise among xs

## What does it mean to take the Entropy of a string (char[])?
- you have a char[]
- this char[] has N unique symbols
- suppose you pick one char from this N symbols and assume that char[0] is this symbol
- ...

- have a event with N states. The states are equally possible
- the possibility of each state is 1/N
- the surprise when we get each state is log(1 / (1/N)) = log(N)

# Cross entropy
H = E(S(xs))

Entropy is calculated from P(x)
But Cross Entropy is calculated from P(x) and Q(x)
Cross Entropy = [P(x) * log(1 / Q(x)) for x in xs]
P(x) is a "true" probability of x
Q(x) is a "real" probability of x

So if P(x) is very high (-> 1) then E(x) is 0
   if P(x) is very high and Q(x) is really low then E(x) = P(x) * log(1 / Q(x)) = high * very high => E(x) explode = really very high
   if P(x) = Q(x) then E(x) = P(x) * log(1 / Q(x)) then E(x) is low