// key, value
// x = hash(key) <=> any data to int(hash)
// buckets = [bucket]
// ht[key] = value <=> buckets[x % buckets.length].append((key, value))
// ht[key] <=> buckets[x % buckets.length].first { $0.key == key }

#include <iostream>
#include <vector>
#include <span>

namespace bst {

template<typename val_t>
struct node_t {
  node_t* left;
  node_t* right;
  val_t value;
};


template<typename val_t>
void insert(node_t<val_t>*& node, val_t value)
{
  if (!node) {
    node = new node_t<val_t>{nullptr, nullptr, value};
  } else {
    if (node->value > value) {
      insert(node->left, value);
    } else {
      insert(node->right, value);
    }
  }
}


template<typename val_t>
void _binsert(node_t<val_t>*& node, val_t value, bool isLeft)
{
  node_t<val_t>*& child = isLeft ? node->left : node->right;
  node_t<val_t>*& ochild = isLeft ? node->right : node->left;
  if (!child) {
    child = new node_t<val_t>{nullptr, nullptr, value};;
    return;
  }
  if (child->value > value) {
    if (child->left || !isLeft || ochild) {
      return _binsert(child, value, true);
    } else {
      auto child2 = new node_t<val_t>{nullptr, nullptr, value};
      node = new node_t<val_t>{child2, new node_t<val_t>{nullptr, nullptr, node->value}, child->value};
    }
  } else {
    if (child->right || isLeft || ochild) {
      return _binsert(child, value, false);
    } else {
      auto child2 = new node_t<val_t>{nullptr, nullptr, value};
      node = new node_t<val_t>{new node_t<val_t>{nullptr, nullptr, node->value}, child2, child->value};
    }
  }
}

// root -> r0 -> r1 => root <- ro -> r1
// root -> l -> l
template<typename val_t>
void binsert(node_t<val_t>*& node, val_t value)
{
  if (!node) {
    node = new node_t<val_t>{nullptr, nullptr, value};
  } else {
    if (node->value > value) {
      _binsert(node, value, true);
    } else {
      _binsert(node, value, false);
    }
  }
}

template<typename val_t>
node_t<val_t>* search(node_t<val_t>* node, val_t value)
{
  if (!node) return nullptr;
  if (node->value == value) {
    return node;
  }
  if (node->value > value) {
    return search(node->left, value);
  } else {
    return search(node->right, value);
  }
}

template<typename val_t>
void minmaxtraverse(node_t<val_t>* node, std::vector<val_t>& vals)
{
  if (!node) return;
  minmaxtraverse(node->left, vals);
  vals.push_back(node->value);
  minmaxtraverse(node->right, vals);
}

template<typename val_t>
void balanceinsert(node_t<val_t>*& node, const std::span<val_t> vals)
{
  if (vals.empty()) return;
  auto mididx = (vals.size() - (1 - vals.size() % 2)) / 2;
  insert(node, vals[mididx]);

  std::span<val_t> lvals{vals.data(), mididx};
  balanceinsert(node, lvals);
  std::span<val_t> rvals{vals.data() + mididx + 1, vals.size() - mididx - 1};
  balanceinsert(node, rvals);
}

template<typename val_t>
void balance(node_t<val_t>*& node)
{
  if (!node) return;
  std::vector<val_t> vals;
  minmaxtraverse(node, vals);
  node = nullptr;
  balanceinsert(node, {vals.data(), vals.size()});
}


template<typename val_t>
void print(node_t<val_t>* node, int level = 0)
{
  if (!node) return;
  std::string pad(level * 2, ' ');
  print(node->right, level + 1);
  std::cout << pad << node->value << std::endl;
  print(node->left, level + 1);
}

template<typename val_t>
void printnorec(node_t<val_t>* node)
{
  
}

}

int main()
{

  using namespace bst;

  node_t<int>* root = nullptr;

  for (int i = 0; i < 10; i++) {
    binsert(root, i);
  }
  print(root);
  // balance(root);

  std::cout << ((search(root, 15) != nullptr) ? "true" : "false") << std::endl;

  return 0;
}

