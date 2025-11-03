#include <iostream>
#include <vector>
#include <span>

using namespace std;

void print(const std::vector<int>& xs)
{
  for (auto i: xs)
    cout << i << ' ';
  cout << endl;
}

void print(const int x)
{
  cout << x << endl;
}

// best-case  => O(n)
// worst-case => O(n^2)
// TODO: derive the formula of arithmetic progression
// {
// 1 + 2 + 3 + 4 + ... + (i - 1)
// n + (n - 1) + (n - 2) + (n - 3) + ... + (n - n)
// A(0) = 0
// A(n) = A(n - 1) + n
// }
void insertionSort(vector<int>& xs, bool inc)
{
  size_t n = xs.size();
  for (size_t i = 1; i < n; i++) {
    int key = xs[i];
    int j = i - 1;
    while (j >= 0 && (inc ? key < xs[j] : key >= xs[j])) {
      xs[j + 1] = xs[j];
      j -= 1;
    }
    xs[j + 1] = key;
  }
}

void selectionSort(vector<int>& xs)
{
  for (size_t i = 0; i < xs.size() - 1; i++) {
    size_t minIndex = i;
    size_t j = i + 1;
    for (; j < xs.size(); j++) {
      if (xs[j] < xs[minIndex]) {
        minIndex = j;
      }
    }
    if (minIndex != i) {
      auto tmp = xs[i];
      xs[i] = xs[minIndex];
      xs[minIndex] = tmp;
    }
  }
}

void bubbleSort(vector<int>& xs)
{
  for (size_t i = 0; i < xs.size(); i++) {
    for (size_t j = 0; j < (xs.size() - i - 1); j++) {
      if (xs.at(j) > xs.at(j + 1)) {
        auto tmp = xs[j + 1];
        xs[j + 1] = xs[j];
        xs[j] = tmp;
      }
    }
  }
}

void mySort(span<int> xs)
{
  if (xs.size() <= 2) {
    if (xs.size() == 2 && xs[0] > xs[1]) {
      std::swap(xs[0], xs[1]);
    }
    return;
  } else {
    int sum = 0;
    vector<int> copy;
    for (size_t i = 0; i < xs.size(); i++) {
      sum += xs[i];
      copy.push_back(xs[i]);
    }
    int mid = sum / xs.size();

    size_t i = 0;
    for (auto x : copy) {
      if (x < mid) {
        xs[i++] = x;
      } 
    }
    size_t midI = i;
    for (auto x : copy) {
      if (x >= mid) {
        xs[i++] = x;
      } 
    }
    mySort(xs.first(midI - 1));
    mySort(xs.last(xs.size() - midI - 1));
  }
}

int main()
{
  vector<int> xs = { 6, 7, 1, 4, 8, 2, 9, 0, 3, 5 };
  span<int> xss { xs };
  print(xs);
  mySort(xss);
  print(xs);
  return 0;
}
