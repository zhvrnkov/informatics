The complexity is f(n) = (n^3)/1000 + 100(n^2) - 100n + 3

What is the algorithmic complexity in big O notation?

It is O(n^3) because of the bigger term

Because as n approaches infinity, the (n^3)/1000 is more close to f(n) than 100(n^2), hence (n^3)/1000 is dominant term, hence complexity is O(n^3)

But interesting fact is that as n -> inf, then (n^2) is more close to f(n) than (n^3), but in that case n^2 is smaller

So with that in mind there is a conflict, but is it? Can term that is smaller than actual time can display the time of an algorithm? I think no because it will be confusing to state that complexity of an algorithm is actually smaller than it is and so event in that case n^3 make more sense
