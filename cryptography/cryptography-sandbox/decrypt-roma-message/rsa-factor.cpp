#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

namespace mp = boost::multiprecision;
using big_int = mp::cpp_int;

bool is_perfect_square(const big_int& x)
{
    auto sx = sqrt(x);
    return sx * sx == x;
}

big_int mod_inverse(big_int e, big_int phi) {
    big_int a = e, b = phi;
    big_int x = 0, y = 1, u = 1, v = 0;
    
    while (a != 0) {
        big_int q = b / a;
        big_int r = b % a;
        big_int m = x - u * q;
        big_int n = y - v * q;
        b = a;
        a = r;
        x = u;
        y = v;
        u = m;
        v = n;
    }
    
    // Make sure x is positive
    if (x < 0) {
        x += phi;
    }
    
    return x;
}

// its imporant that it uses Fermat Factorization alogrithm
// which is a technique to factor odd number N
// its based on property that odd numbers can be written as product of
// N = (a + b) * (a - b) = a*a - b*b
// and its a property of odd numbers because to distance between p and q should be even number
// (p - q) % 2 == 0
// and that's true only if
// p and q are both even numbers (but such p * q == even) (and in that case N % 4 == 0)
// or p and q are both odd numbers (and p - q == even)
// otherwise p - q == odd
// and it make sense for rsa crack
// if p or q is even (basically only 2 is even prime number) then N / 2 is automatically gives us second prime number
// if p and q are odd then we can use this property and use Fermat Factorization technique
void factorize(const big_int& N)
{
    big_int a = mp::sqrt(N) + 1;
    big_int b_squared = a * a - N;

    while (!is_perfect_square(b_squared)) {
        a += 1;
        b_squared = a * a - N;
    }

    auto b = sqrt(b_squared);
    auto p = a + b;
    auto q = a - b;

    std::cout << "GOT P = " << p << std::endl;
    std::cout << "GOT Q = " << q << std::endl;

    big_int e = 10445627;

    auto phi = (p - 1) * (q - 1);
    auto d = mod_inverse(e, phi);

    std::cout << "GOT D = " << d << std::endl;
    std::cout << "GOT PHI = " << phi << std::endl;
}

int main()
{
    big_int N("91859820443963317207975816217818538045326991275048210332218870264223530952579150504197523271635639573977886516800530364642539605592814590229261818986430613350748518542792908442318723457739823006932480165473784058530562807755149563083316160808082533476368610059943636352143325022541194871397779753084365191879794824922014517916515863376000161576999713389375799064005581067193587863472201846508597698821596922540900895346278505942043981314086809185289486014070614082740155749816771166018424883015528669744905988532062665063869800205282186250388075617540739346039247236996244640194547406546054081075301259907963387369753");
    factorize(N);

    return 1;
}