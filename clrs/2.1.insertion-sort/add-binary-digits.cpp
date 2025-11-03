#include <iostream>
#include <vector>

using namespace std;

template<typename T>
void print(const std::vector<T>& xs)
{
  for (auto i: xs)
    cout << i << ' ';
  cout << endl;
}

void print(const int x)
{
  cout << x << endl;
}

vector<bool> add(const vector<bool>& a, const vector<bool>& b)
{
  vector<bool> output(a.size() + 1);
  bool carry = false;
  for (size_t i = 0; i < a.size(); i++) {
    int index = a.size() - 1 - i;
    int sum = (int)a[index] + (int)b[index] + (int)carry;
    output[output.size() - 1 - i] = sum % 2;
    carry = sum / 2;
  }
  if (carry) {
    output[0] = true;
  }
  return output;
}

size_t num(const vector<bool>& x)
{
  size_t output = 0;
  for (size_t i = 0; i < x.size(); i++) {
    size_t bit = (size_t)x[x.size() - 1 - i];
    output += bit<<i;
  }
  return output;
}

int main()
{
  vector<bool> a = { 1, 1, 0 };
  vector<bool> b = { 0, 1, 1 };
  print(num(add(a, b)));
  return 0;
}

