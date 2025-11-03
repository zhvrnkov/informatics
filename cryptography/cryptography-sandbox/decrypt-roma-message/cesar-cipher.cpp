#include <__ranges/concepts.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

size_t alphabet_size = 26;

char index2char(int index) {
  if (index == (alphabet_size - 1)) {
    return ' ';
  } else {
    return 'a' + index;
  }
}

int char2index(char c) {
  if (c == ' ') {
    return alphabet_size - 1;
  } else {
    return c - 'a';
  }
}

std::string cesar_encrypt(const std::string &ptext, char key) {
  std::string ctext;
  for (size_t i = 0; i < ptext.size(); i++) {
    size_t index = char2index(ptext[i]);
    index = (index + key + alphabet_size) % alphabet_size;
    ctext.push_back(index2char(index));
  }
  return ctext;
}

std::string cesar_decrypt(const std::string &ctext, char key) {
  std::string ptext;
  for (size_t i = 0; i < ctext.size(); i++) {
    size_t index = char2index(ctext[i]);
    index = (index - key + alphabet_size) % alphabet_size;
    ptext.push_back(index2char(index));
  }
  return ptext;
}

std::map<char, double> str2freqs(const std::string &input) {
  std::map<char, double> freqs;
  double count = 0;
  for (char c : input) {
    count += 1.0;
    freqs[c]++;
  }
  for (auto &[key, value] : freqs) {
    value /= count;
  }
  return freqs;
}

std::map<char, double> english_freq = {
    {' ', 20 / 100.0},   {'e', 12.02 / 100.0}, {'t', 9.10 / 100.0},
    {'a', 8.12 / 100.0}, {'o', 7.68 / 100.0},  {'i', 7.31 / 100.0},
    {'n', 6.95 / 100.0}, {'s', 6.28 / 100.0},  {'r', 6.02 / 100.0},
    {'h', 5.92 / 100.0}, {'d', 4.32 / 100.0},  {'l', 3.98 / 100.0},
    {'u', 2.88 / 100.0}, {'c', 2.71 / 100.0},  {'m', 2.61 / 100.0},
    {'f', 2.30 / 100.0}, {'y', 2.11 / 100.0},  {'w', 2.09 / 100.0},
    {'g', 2.03 / 100.0}, {'p', 1.82 / 100.0},  {'b', 1.49 / 100.0},
    {'v', 1.11 / 100.0}, {'k', 0.69 / 100.0},  {'x', 0.17 / 100.0},
    {'q', 0.11 / 100.0}, {'j', 0.10 / 100.0},  {'z', 0.07 / 100.0},
};

int dst(char x, char y)
{
    return ((char2index(x) - char2index(y)) + alphabet_size) % alphabet_size;
}

void cesar_cipher_brutforce(std::string input) {
  for (char i = 'a'; i < 'z'; i++) {
    std::cout << i << ": ";
    for (char x : input) {
      if (x == ' ') {
        std::cout << x;
      } else {
        auto d = (int)x - (int)input[0];
        char index = (i + ((d + 26) % 26)) % 26;
        std::cout << (char)('a' + index);
      }
    }
    std::cout << std::endl;
  }
}

template <typename K, typename V>
std::vector<std::pair<K, V>> value_sorted(const std::map<K, V> &freqs) {
  std::vector<std::pair<K, V>> output(freqs.size());
  std::copy(freqs.begin(), freqs.end(), output.begin());
  std::sort(output.begin(), output.end(),
            [](auto &left, auto &right) { return left.second > right.second; });
  return output;
}

double index_of_cons(std::string x) 
{
  x = cesar_encrypt(x, 34);
  double n = x.size();
  auto m = alphabet_size;

  std::vector<double> fs(m);
  for (char c : x) {
    fs.at(char2index(c)) += 1;
  }
  // auto fs = str2freqs(x);

  double I = 0;
  for (size_t i = 0; i < m; i++) {
    // I += fs[index2char(i)] * fs[index2char(i)];
    I += (fs.at(i) * (fs.at(i) - 1)) / (n * (n - 1));
  }

  return I;
}

int main() {
  // std::string message =
  //     "the old timer had been very serious in laying down the law that no man "
  //     "must travel alone in the klondike after fifty below well here he was he "
  //     "had had the accident he was alone and he had saved himself those old "
  //     "timers were rather womanish some of them he thought all a man had to do "
  //     "was to keep his head and he was all right any man who was a man could "
  //     "travel alone but it was surprising the rapidity with which his cheeks "
  //     "and nose were freezing and he had not thought his fingers could go "
  //     "lifeless in so short a time lifeless they were for he could scarcely "
  //     "make them move together to grip a twig and they seemed remote from his "
  //     "body and from him when he touched a twig he had to look and see whether "
  //     "or not he had hold of it the wires were pretty well down between him "
  //     "and his finger ends all of which counted for little there was the fire "
  //     "snapping and crackling and promising life with every dancing flame he "
  //     "started to untie his moccasins they were coated with ice the thick "
  //     "german socks were like sheaths of iron half way to the knees and the "
  //     "mocassin strings were like rods of steel all twisted and knotted as by "
  //     "some conflagration";
  // std::cout << message << std::endl;

  // std::cout << index_of_cons(message) << std::endl;
  // return 0;

  // message = cesar_encrypt(message, -127);

  std::string message = "gluhtlishjrvbadvyyplkaohavbyjpwolypzavvdlhrvuuleatlzzhnlzdpajoavcpnlulyljpwolyrlfdvykpzaolopkkluzftivsvmklhaoputfmhcvypalovsilpuluk";
  std::cout << cesar_decrypt(message, -(253 % alphabet_size)) << std::endl;
  return 0;

  {
    auto freqs = value_sorted(str2freqs(message));
    auto english_freq_sorted = value_sorted(english_freq);

    printf("ctext:\n");
    for (auto &[key, value] : freqs) {
      printf("%c = %.4f\n", key, value);
    }

    printf("-------------\n");

    printf("english:\n");
    for (auto &[key, value] : english_freq_sorted) {
      printf("%c = %.4f\n", key, value);
    }

    std::map<int, double> dst_rank;
    for (size_t i = 0; i < freqs.size(); i++) {
      auto &f = freqs.at(i);
      auto &e = english_freq_sorted.at(i);
      // dst_rank[index2char(dst(f.first, e.first))] += (f.second / e.second) *
      // (double)(freqs.size() - i)/(double)freqs.size();
      dst_rank[index2char(dst(f.first, e.first))] += f.second * e.second;
      // distance +=  * f.second;
    }
    auto dst_rank_sorted = value_sorted(dst_rank);
    for (auto& [key, rank] : dst_rank_sorted) {
      printf("key = %c | rank = %.4f\n", key, rank);
    }

    for (size_t i = 0; i < 3; i++) {
      auto key = char2index(dst_rank_sorted[i].first);
      auto dec = cesar_decrypt(message, key);
      printf("%c (%d) => %s\n", index2char(key), key, std::string(dec.data(), 64).data());
    }
  }

  // message = cesar_decrypt(message, key);
  // std::cout << message << std::endl;
  return 0;
}