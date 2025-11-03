#include <algorithm>
#include <clocale>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <numeric>
#include <ratio>
#include <sstream>
#include <string>
#include <iostream>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cassert>
#include <filesystem>

const std::string ptext = "the old timer had been very serious in laying down the law that no man must travel alone in the klondike after fifty below well here he was he had had the accident he was alone and he had saved himself those old timers were rather womanish some of them he thought all a man had to do was to keep his head and he was all right any man who was a man could travel alone but it was surprising the rapidity with which his cheeks and nose were freezing and he had not thought his fingers could go lifeless in so short a time lifeless they were for he could scarcely make them move together to grip a twig and they seemed remote from his body and from him when he touched a twig he had to look and see whether or not he had hold of it the wires were pretty well down between him and his finger ends all of which counted for little there was the fire snapping and crackling and promising life with every dancing flame he started to untie his moccasins they were coated with ice the thick german socks were like sheaths of iron half way to the knees and the mocassin strings were like rods of steel all twisted and knotted as by some conflagration";

size_t alphabet_size = 27;

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

std::string vig_encrypt(std::string input, std::string key)
{
    std::string output;
    for (size_t i = 0; i < input.size(); i++) {
        char offset = (char2index(input[i]) + char2index(key[i % key.size()])) % alphabet_size;
        output.push_back(index2char(offset));
    }
    return output;
}

std::string vig_decrypt(const std::string& ciphertext, const std::string& key) {
    std::string plaintext;
    for (size_t i = 0; i < ciphertext.size(); i++) {
        char offset = char2index(ciphertext[i]) - char2index(key[i % key.size()]);
        offset += alphabet_size;
        offset %= alphabet_size;
        plaintext.push_back(index2char(offset));
    }
    return plaintext;
}

using ngrams_t = std::map<std::string, std::vector<size_t>>;

void filter_ngrams(ngrams_t& map)
{
    for (auto it = map.begin(); it != map.end();) {
        if (it->second.size() <= 1) {
            map.erase(it++);
        } else {
            it++;
        }
    }
}

ngrams_t get_ngrams(const std::string& ciphertext, size_t n)
{
    ngrams_t map;
    for(size_t i = 0; i <= ciphertext.size() - n; i++) {
        std::string triplet(ciphertext.data() + i, n);
        map[triplet].push_back(i);
    }
    filter_ngrams(map);
    return map;
}

ngrams_t merge_ngrams(const ngrams_t& xs)
{
    std::unordered_map<size_t, std::string> reversed_xs;
    for (auto &[gram, pos] : xs) {
        for (auto p : pos) {
            assert(reversed_xs.contains(p) == false);
            reversed_xs[p] = gram;
        }
    }
    ngrams_t map;
    for (auto &[gram, pos] : xs) {
        auto n = gram.size();
        for (auto p : pos) {
            if (reversed_xs.contains(p + 1)) {
                auto next_gram = reversed_xs[p + 1];
                auto new_gram = gram;
                new_gram += next_gram.back();
                map[new_gram].push_back(p);
            }
        }
    }
    filter_ngrams(map);
    return map;
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

template <typename K, typename V>
std::vector<std::pair<K, V>> value_sorted(const std::map<K, V> &freqs) {
  std::vector<std::pair<K, V>> output(freqs.size());
  std::copy(freqs.begin(), freqs.end(), output.begin());
  std::sort(output.begin(), output.end(),
            [](auto &left, auto &right) { return left.second > right.second; });
  return output;
}

int dst(char x, char y)
{
    return ((char2index(x) - char2index(y)) + alphabet_size) % alphabet_size;
}

std::vector<std::string> find_keys(const std::string& ctext, size_t keylength)
{
    auto english_freq_sorted = value_sorted(english_freq);
    std::vector<std::string> groups(keylength);
    for (size_t i = 0; i < ctext.size(); i++) {
        groups[i % groups.size()].push_back(ctext[i]);
    }

    std::vector<std::vector<char>> key(groups.size());
    for (size_t i = 0; i < groups.size(); i++) {
        auto &group = groups[i];
        auto freqs = value_sorted(str2freqs(group));
        if (false) {
            std::map<char, double> dst_rank;
            for (size_t i = 0; i < freqs.size(); i++) {
                auto &f = freqs.at(i);
                auto &e = english_freq_sorted.at(i);
                // dst_rank[index2char(dst(f.first, e.first))] += (f.second /
                // e.second) * (double)(freqs.size() - i)/(double)freqs.size();
                dst_rank[index2char(dst(f.first, e.first))] += f.second * e.second;
                // distance +=  * f.second;
            }
            auto dst_rank_sorted = value_sorted(dst_rank);
            printf("%c\n", dst_rank_sorted[0].first);
            key[i].push_back(dst_rank_sorted[0].first);
            // if ((dst_rank_sorted[0].second - dst_rank_sorted[1].second) <
            //     0.03) {
            //     key[i].push_back(dst_rank_sorted[1].first);
            // }
        } else {
            auto most_frequent_in_e = english_freq_sorted.front();
            auto distance = dst(freqs[0].first, most_frequent_in_e.first);
            auto sure =
                (freqs[0].second - freqs[1].second) /
                (english_freq_sorted[0].second - english_freq_sorted[1].second);
            key[i].push_back(index2char(distance));
            printf("%d\n", distance);
            // TODO: can be better
            if (sure < 0.75) {
                key[i].push_back(
                    index2char(dst(freqs[1].first, most_frequent_in_e.first)));
            }
            if (sure < 0.25) {
                key[i].push_back(
                    index2char(dst(freqs[2].first, most_frequent_in_e.first)));
            }
        }
    }
    size_t keys_count = 1;
    for (auto x : key) {
        keys_count *= x.size();
    }
    std::vector<std::string> keys(keys_count);
    size_t count = 1;
    for (size_t i = 0; i < key.size(); i++) {
        auto letters = key.at(i);
        for (size_t j = 0; j < keys_count; j++) {
            keys.at(j) += letters.at(j / count % letters.size());
        }
        count *= letters.size();
    }
    return keys;
}

double index_of_cons(std::string x) 
{
//   x = vig_encrypt(x, "elvira");
  double n = x.size();
  auto m = alphabet_size;

  std::vector<double> fs(m);
  for (char c : x) {
    fs.at(char2index(c)) += 1;
  }
//   auto fs = str2freqs(x);

  double I = 0;
  for (size_t i = 0; i < m; i++) {
    // I += fs[index2char(i)] * fs[index2char(i)];
    I += (fs.at(i) * (fs.at(i) - 1)) / (n * (n - 1));
  }

  return I;
}

double mutual_index_of_cons(const std::string& x, const std::string& y)
{
    size_t m = alphabet_size;

    auto fs = str2freqs(x);
    auto gs = str2freqs(y);

    double MI = 0;
    for (size_t i = 0; i < m; i++) {
        MI += fs[index2char(i)] * gs[index2char(i)];
    }

    return MI;
}

std::vector<std::string> find_keys2(const std::string& ctext, size_t keylength)
{
    std::vector<std::string> groups(keylength);
    for (size_t i = 0; i < ctext.size(); i++) {
        groups[i % groups.size()].push_back(ctext[i]);
    }

    std::vector<int> shifts(keylength);
    for (size_t i = 1; i < groups.size(); i++) {
        auto maxMioc = mutual_index_of_cons(groups[0], groups[i]);
        auto maxS = 0;
        for (size_t s = 1; s < alphabet_size; s++) {
            std::string sgroup = cesar_encrypt(groups[i], s);
            auto mioc = mutual_index_of_cons(groups[0], cesar_encrypt(groups[i], s));
            if (mioc > maxMioc) {
                maxMioc = mioc;
                maxS = s;
            }
        }
        shifts[i] = maxS;
    }

    std::vector<char> first_cs;

    auto english_freq_sorted = value_sorted(english_freq);
    auto freqs = value_sorted(str2freqs(groups[0]));

    auto most_frequent_in_e = english_freq_sorted.front();
    auto distance = dst(freqs[0].first, most_frequent_in_e.first);

    auto sure = (freqs[0].second - freqs[1].second) /
                (english_freq_sorted[0].second - english_freq_sorted[1].second);
    first_cs.push_back(index2char(distance));
    // TODO: can be better
    if (sure < 0.75) {
        first_cs.push_back(
            index2char(dst(freqs[1].first, most_frequent_in_e.first)));
    }
    if (sure < 0.25) {
        first_cs.push_back(
            index2char(dst(freqs[2].first, most_frequent_in_e.first)));
    }

    std::vector<std::string> keys;
    for (auto first_c : first_cs) {
        std::string key;
        for (auto shift : shifts) {
            int index = char2index(first_c);
            index = (index - shift + alphabet_size) % alphabet_size;
            key.push_back(index2char(index));
        }
        keys.push_back(key);
    }

    return keys;
}

void analyze_index_of_cons(const std::string& x, size_t keylength)
{
    std::vector<std::string> groups(keylength);
    for (size_t i = 0; i < x.size(); i++) {
        groups[i % groups.size()].push_back(x[i]);
    }

    printf("ctext ioc = %f\n", index_of_cons(x));
    for (const auto& group : groups) {
        printf("group ioc = %f\n", index_of_cons(group));
    }

    for (size_t i = 1; i < groups.size(); i++) {
        auto maxMioc = mutual_index_of_cons(groups[0], groups[i]);
        auto maxS = 0;
        for (size_t s = 1; s < alphabet_size; s++) {
            std::string sgroup = cesar_encrypt(groups[i], s);
            auto mioc = mutual_index_of_cons(groups[0], cesar_encrypt(groups[i], s));
            if (mioc > maxMioc) {
                maxMioc = mioc;
                maxS = s;
            }
        }
        printf("group #0 to #%lu shift = %d\n", i, maxS);
    }

    // ioc - дает понять:
    // - на каком языке текст
    // - применен к нему шифр сдвига или полиалфавитный
    // ~ioc для английского текста = 0.0644
    // т.к. полиалфавитный шифр ломает все частоты символов в тексте
    // по-сути делая превращая текст в рандомную последовательность символов
    // то ioc для полиалфавитного шифра ~ ioc для рандомной последовательности символов
    // => ioc для полиалфавитного шифра ~ ioc для рандомной последовательности ~ 0,03846
    // также стоит отметить что шифр сдвига не меняет ioc текста
    //
    // mioc - дает понять:
    // - сдвиг между ptext и ctext (если он был зашифрован методом сдвига)
}

size_t find_keylength(const std::string& ctext)
{
    ngrams_t lastn_ngrams = get_ngrams(ctext, 2);
    ngrams_t ngrams = lastn_ngrams;
    for (size_t i = 3; i <= 4; i++) {
        lastn_ngrams = merge_ngrams(lastn_ngrams);
        ngrams.merge(lastn_ngrams);
    }

    ngrams_t deltas;
    for (auto &[key, pos] : ngrams) {
        for (size_t i = 1; i < pos.size(); i++) {
            deltas[key].push_back(pos[i] - pos[i-1]);
        }
    }

    std::map<size_t, size_t> ds;

    for (auto &[gram, delta] : deltas) {
        for (auto d : delta) {
            ds[d]++;
        }
    }
    auto ds_flat = value_sorted(ds);
    std::map<size_t, size_t> gcdc;
    for (auto [x, count] : ds_flat) {
        if (count > 10) {
            for (auto [y, counts] : ds_flat) {
                gcdc[std::gcd(x, y)] += counts;
            }
        }
    }
    auto gcds = value_sorted(gcdc);
    return gcds[0].first;
}

int main()
{
    std::string key = "vlad";
    std::string vig_enc = vig_encrypt(ptext, key);
    std::string xor_enc = ptext;
    for (size_t i = 0; i < xor_enc.size(); i++) {
        xor_enc[i] = index2char(((xor_enc[i] ^ key[i % key.size()]) + alphabet_size) % alphabet_size);
    }
    std::cout << vig_enc << std::endl << std::endl;
    std::cout << xor_enc << std::endl << std::endl;

    return 0;
}
