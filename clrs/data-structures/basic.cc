#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <cassert>
#include <ctime>
#include <span>
#include <string>
#include <utility>
#include <optional>

namespace basic {

template<typename val_t>
struct stack_t {
  stack_t(size_t size = 256) : size(size), count(0) {
    data = new val_t[size];
  };
  
  ~stack_t() {
    delete[] data;
  }
  
  val_t pop() {
    if (count == 0) {
      throw std::exception{};
    }
    return data[--count];
  }

  size_t getHead() {
    return count - 1;
  }

  val_t* peek(size_t idx) {
    if (idx >= count) return nullptr;

    return &data[idx];
  }
  
  bool push(val_t value) {
    if (count >= size) {
      return false;
    }
    data[count++] = value;
    return true;
  }

  bool empty() {
    return count == 0;
  }
  
  private:
  val_t* data;
  size_t size;
  size_t count;
};

template<typename val_t>
struct queue_t {

  queue_t(size_t size = 256) : size(size), head(0), tail(0), count(0) {
    data = new val_t[size];
  };
  
  ~queue_t() {
    delete[] data;
  }

  val_t dequeue() {
    if (count <= 0) throw std::exception{};
    auto h = head;
    head = (head + 1) % size;
    count--;
    return data[h];
  }

  bool enqueue(val_t value) {
    if (count == size) return false;

    data[tail++] = value;
    count++;
    tail %= size;
    return true;
  }

  bool empty() {
    return count == 0;
  }

  private:
    val_t* data;
    size_t size;
    size_t head;
    size_t tail;
    size_t count;
};

template<typename val_t>
struct deque_t {
  deque_t(size_t size = 256) : size(size), count(0), head(0), tail(0) {
    data = new val_t[size];
  };
  
  ~deque_t() {
    delete[] data;
  }

  val_t dequeueFront() {
    if (count <= 0) {
      throw std::exception{};
    }
    auto h = head;
    head = (head + 1) % size;
    count--;
    return data[h];
  }

  val_t dequeueBack() {
    if (count <= 0) {
      throw std::exception{};
    }
    if (tail == 0) tail = size - 1;
    else tail = (tail - 1) % size;
    count--;
    return data[tail];
  }

  bool enqueueFront(val_t value) {
    if (count >= size) return false;
    if (head == 0) head = size - 1;
    else head = (head - 1) % size;
    data[head] = value;
    count++;
    return true;
  }

  bool enqueueBack(val_t value) {
    if (count >= size) return false;

    data[tail++] = value;
    tail %= size;
    count++;
    return true;
  }

  bool empty() {
    return count == 0;
  }

  private:
    val_t* data;
    size_t size;
    size_t count;

    size_t head;
    size_t tail;
};

template<typename val_t>
struct slistnode_t {
  slistnode_t<val_t>* next;
  val_t value;

  void remove(slistnode_t<val_t>* x) {
    slistnode_t<val_t>* xparent = nullptr;
    auto current = this;
    while (current) {
      if (current->next == x) {
        xparent = current;
        break;
      }
      current = current->next;
    }
    if (xparent) {
      xparent->next = x->next;
      x->next = nullptr;
      delete x;
    }
  }

  void insert(slistnode_t<val_t>* x) {
    x->next = next;
    next = x;
  }

  slistnode_t<val_t>* insert(val_t value) {
    auto x = new slistnode_t<val_t>{nullptr, value};
    x->next = next;
    next = x;
    return x;
  }

  slistnode_t<val_t>* search(val_t value) {
    auto current = this;
    while (current && current->value != value) {
      current = current->next;
    }
    return current;
  }

  void print() {
    std::cout << value;
    if (next) {
      std::cout << " -> ";
      next->print();
    } else {
      std::cout << std::endl;
    }
  }
};

template<typename val_t>
struct dlistnode_t {
  val_t value;
  dlistnode_t *prev;
  dlistnode_t *next;

  static dlistnode_t<val_t>* sentinel() {
    dlistnode_t<val_t>* sentinel = new dlistnode_t<val_t>{{}, nullptr, nullptr};
    sentinel->next = sentinel;
    sentinel->prev = sentinel;
    return sentinel;
  }

  // p <-> x <-> n
  // p <-> x <-> node <-> n
  dlistnode_t<val_t>* insert(val_t value) {
    auto node = new dlistnode_t<val_t>{value, nullptr, nullptr};
    insert(node);
    return node;
  }

  // s <-> 0
  // s <-> 0 <-> 1
  void insert(dlistnode_t<val_t>* node) {
    node->prev = this;
    node->next = next;
    if (next) next->prev = node;
    next = node;
  }

  // p <-> x <-> n
  // p <-> n
  void remove() {
    if (prev) {
      prev->next = next;
    }
    if (next) {
      next->prev = prev;
    }
    next = nullptr;
    prev = nullptr;
    delete this;
  }

  dlistnode_t<val_t>* search(val_t value) {
    auto current = this;
    while (current && current->value != value) {
      current = current->next;
    }
    return current;
  }

  dlistnode_t<val_t>* search(dlistnode_t<val_t>* sentinel, val_t value) {
    auto current = this == sentinel ? next : this;
    while (current != sentinel && current->value != value) {
      current = current->next;
    }
    return current == sentinel ? nullptr : current;
  }

  dlistnode_t<val_t>* search(dlistnode_t<val_t>* sentinel, std::function<bool(val_t)> pred) {
    auto current = this == sentinel ? next : this;
    while (current != sentinel && !pred(current->value)) {
      current = current->next;
    }
    return current == sentinel ? nullptr : current;
  }

  void print(dlistnode_t<val_t>* sentinel) {
    std::cout << value;
    if (next != sentinel) {
      std::cout << " -> ";
      next->print(sentinel);
    } else {
      std::cout << std::endl;
    }
  }

  void print() {
    std::cout << value;
    if (next && this != next) {
      std::cout << " -> ";
      next->print();
    } else {
      std::cout << std::endl;
    }
  }
};

template<typename val_t>
struct dlistxornode_t {
  val_t value;
  uint64_t np;

  dlistxornode_t<val_t>* insert(dlistxornode_t<val_t>* prev, val_t value) {
    // p <-> s <-> n
    // p <-> s <-> x -> n
    //
    // null <-> s <-> x
    if (prev) {
      auto next = (dlistxornode_t<val_t>*)(np ^ (uint64_t)prev);
      auto x = new dlistxornode_t<val_t>{value, ((uint64_t)this) ^ ((uint64_t)next)};
      if (next) {
        next->np = ((next->np) ^ (uint64_t)this) ^ (uint64_t)x;
      }
      np = (uint64_t)prev ^ (uint64_t)x;
      return x;
    } else {
      auto x = new dlistxornode_t<val_t>{value, ((uint64_t)this) ^ ((uint64_t)this)};
      np = (uint64_t)x ^ (uint64_t)x;
      return x;
    }
  }

  // for empty node or node with only previos or only next!
  dlistxornode_t<val_t>* add(val_t value) {
    auto x = new dlistxornode_t<val_t>{value, (uint64_t)this};
    np ^= (uint64_t)x;
    return x;
  }

  void remove(dlistxornode_t<val_t>* previous) {
    dlistxornode_t<val_t>* next = (dlistxornode_t<val_t>*)(np ^ (uint64_t)previous);
    assert((np ^ (uint64_t)next) == (uint64_t)previous);
    previous->np = (previous->np ^ (uint64_t)this) ^ (uint64_t)next;
    next->np = (next->np ^ (uint64_t)this) ^ (uint64_t)previous;
  }

  dlistxornode_t<val_t>* search(val_t value) {
    uint64_t prev = 0;
    auto current = this;
    while (current && current->value != value) {
     dlistxornode_t<val_t>* next  = (dlistxornode_t<val_t>*)(current->np ^ prev);
     prev = (uint64_t)current;
     current = next;
    }
    return current;
  }

  void print(dlistxornode_t<val_t>* prev) {
    uint64_t prev64 = (uint64_t)prev;
    auto current = this;
    while (current != prev) {
      std::cout << current->value;
      dlistxornode_t<val_t>* next  = (dlistxornode_t<val_t>*)(current->np ^ prev64);
      prev64 = (uint64_t)current;
      current = next;
      if (current != prev) {
        std::cout << " -> ";
      }
    }
    std::cout << std::endl;
  }
};

std::pair<uint32_t, uint32_t> dwordDecay(uint64_t n) 
{
  return std::make_pair((n >> 32) & 0xFFFFFFFF, n & 0xFFFFFFFF);
}

uint64_t dwordBound(uint32_t l, uint32_t r) 
{
  return ((uint64_t)(l) << 32 | (r & 0xFFFFFFFF));
}

uint64_t dwordSwapped(uint64_t n)
{
  auto [l, r] = dwordDecay(n);
  return dwordBound(r, l);
}


template<typename k_t>
static uint32_t hash_muls(k_t x, uint64_t power) {
  constexpr uint64_t a = 2654435769;
  return ((x * a) % (1ul << 32)) >> (32 - power);
}

static uint32_t hash_muls(std::string x, uint64_t power) {
  auto h = 0;
  for (auto c : x) {
    h ^= hash_muls((uint32_t)c, power);
  }
  return h;
}

template<typename key_t, typename val_t>
struct chain_hash_table_t {
  using entry_t = std::pair<key_t, val_t>;

  const size_t M = 256;
  const size_t M_POW = 8;

  chain_hash_table_t() {
    storage = new dlistnode_t<entry_t>*[M]{};
  }

  ~chain_hash_table_t() {
    delete[] storage;
  }

  void set(key_t key, val_t value) {
    auto hash = hash_muls(key, M_POW);
    if (storage[hash]) {
      auto node = storage[hash]->search(storage[hash], [&key](auto entry) { return entry.first == key; });
      if (node) {
        node->value.second = value;
      } else {
        storage[hash]->insert(std::make_pair(key, value));
      }
    } else {
      storage[hash] = dlistnode_t<entry_t>::sentinel();
      storage[hash]->insert(std::make_pair(key, value));
    }
  }

  std::optional<val_t> get(key_t key) {
    auto node = getnode(key);
    if (node) {
      return node->value.second;
    } else {
      return std::nullopt;
    }
  }

  void remove(key_t key) {
    auto node = getnode(key);
    if (node) {
      node->remove();
    }
  }

  private:

  dlistnode_t<entry_t>* getnode(key_t key) {
    auto hash = hash_muls(key, M_POW);
    if (storage[hash]) {
      return storage[hash]->search(storage[hash], [&key](auto entry) { return entry.first == key; });
    } else {
      return nullptr;
    }
  }

  dlistnode_t<entry_t>** storage;
};

template<typename key_t, typename val_t>
struct oa_hash_table_t {
  struct entry_t {
    key_t key;
    val_t value;
    bool notNil;
  };

  const size_t M_POW = 12;
  const size_t M = 1 << M_POW;

  oa_hash_table_t() {
    storage = new entry_t[M]{};
  }

  ~oa_hash_table_t() {
    delete[] storage;
  }

  void set(key_t key, val_t value) {
    for (uint32_t i = 0; i < M; i++) {
      auto h = hash(key, i);
      if (!storage[h].notNil || storage[h].key == key) {
        storage[h].notNil = true;
        storage[h].key = key;
        storage[h].value = value;
        break;
      }
    }
  }

  std::optional<val_t> get(key_t key) {
    auto entry = getEntry(key);
    if (entry) {
      return entry->value;
    } else {
      return std::nullopt;
    }
  }

  void remove(key_t key) {
    auto g = [&](key_t k, uint32_t h) { return  (h - hash(k, 0)) % M; };

    uint32_t q = 0;
    for (uint32_t i = 0; i < M; i++) {
      q = hash(key, i);
      if (!storage[q].notNil) {
        return;
      } 
      else if (storage[q].key == key) {
        break;
      }
    }
    storage[q].notNil = false;
    auto qi = g(storage[q].key, q);
    auto tq = q + 1;

    // std::cout << storage[q].value << " = " << qi << std::endl;

    while (storage[tq].notNil) {
      auto tqi = g(storage[tq].key, tq);
      // std::cout << storage[tq].value << " = " << tqi << std::endl;
      if (tqi > qi) {
        storage[q] = storage[tq];
        storage[tq].notNil = false;
        q = tq;
        qi = tqi;
      }
      tq += 1;
    }
  }

  void dump() {
    for (auto i = 0; i < (1 << M_POW); i++) {
      if (storage[i].notNil) {
        std::cout << storage[i].value << " ";
      } else {
        std::cout << "x ";
      }
    }
    std::cout << std::endl;
  }
  
  private:
  entry_t* getEntry(key_t key) {
    entry_t* output = nullptr;
    for (uint32_t i = 0; i < M; i++) {
      auto h = hash(key, i);
      if (!storage[h].notNil) {
        break;
      } 
      else if (storage[h].key == key) {
        output = &storage[h];
        break;
      }
    }
    return output;
  }

  uint32_t hash(key_t key, uint32_t i) {
    auto h = hash_muls(key, M_POW);
    // linear probbing
    return (h + i) % M;

    // double hashing
    // return (h + i * (h + (1 - h % 2))) % M;
  }

  entry_t* storage;
};

void printxs(const std::vector<uint8_t>& xs)
{
  for (auto& x : xs) {
    std::cout << (uint32_t)x << ", ";
  }
  std::cout << std::endl;
}

  template<typename T>
void printxs(const T& xs)
{
  for (auto& x : xs) {
    std::cout << x << ", ";
  }
  std::cout << std::endl;
}
}

namespace basic::bst {

template<typename val_t>
struct node_t {
  node_t* parent;
  node_t* left;
  node_t* right;
  val_t value;

  ~node_t() = default;
};

template<typename val_t>
node_t<val_t>* insertiter(node_t<val_t>*& node, val_t value)
{
  node_t<val_t>** current = &node;
  auto parent = (*current) != nullptr ? (*current)->parent : nullptr;
  while (*current) {
    parent = *current;
    if ((*current)->value > value) {
      current = &(*current)->left;
    } else {
      current = &(*current)->right;
    }
  }
  *current = new node_t<val_t>{parent, nullptr, nullptr, value};
  return *current;
}

template<typename val_t>
node_t<val_t>* insert(node_t<val_t>*& node, val_t value, node_t<val_t>* parent = nullptr)
{
  if (!node) {
    node = new node_t<val_t>{parent, nullptr, nullptr, value};
    return node;
  } else {
    if (node->value > value) {
      return insert(node->left, value, node);
    } else {
      return insert(node->right, value, node);
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
  if (!node || node->value == value) {
    return node;
  }
  if (node->value > value) {
    return search(node->left, value);
  } else {
    return search(node->right, value);
  }
}

template<typename val_t>
node_t<val_t>* searchiter(node_t<val_t>* node, val_t value)
{
  while (node) {
    if (node->value == value) {
      return node;
    }
    else if (node->value > value) {
      node = node->left;
    } else {
      node = node->right;
    }
  }
  return nullptr;
}

template<typename val_t>
node_t<val_t>* searchmin(node_t<val_t>* node)
{
  if (!node->left) {
    return node;
  } else {
    return searchmin(node->left);
  }
}

template<typename val_t>
node_t<val_t>* searchmax(node_t<val_t>* node)
{
  if (!node->right) {
    return node;
  } else {
    return searchmax(node->right);
  }
}

template<typename val_t>
node_t<val_t>* pred(node_t<val_t>* node)
{
  if (node->left) {
    return searchmax(node->left);
  }
  else {
    while (node->parent->right != node) {
      node = node->parent;
      if (!node->parent) {
        return nullptr;
      }
    }
    return node->parent;
  }
}

template<typename val_t>
node_t<val_t>* suc(node_t<val_t>* node)
{
  if (node->right) {
    return searchmin(node->right);
  } 
  else {
    while (node->parent->left != node) {
      node = node->parent;
      if (!node->parent) {
        return nullptr;
      }
    }
    return node->parent;
  }
}

template<typename val_t>
void remove(node_t<val_t>*& root, val_t value)
{
  auto n = search(root, value);
  if (!n) return;

  auto unattach = [](node_t<val_t>* node) {
    if (node->parent) {
      if (node->parent->left == node) {
        node->parent->left = nullptr;
      }
      else if (node->parent->right == node) {
        node->parent->right = nullptr;
      }
      node->parent = nullptr;
    }
  };

  auto transplant = [&](node_t<val_t>* node, node_t<val_t>* branch) {
    if (node->parent) {
      auto p = node->parent;
      if (p->left == node) {
        p->left = branch;
      }
      else {
        p->right = branch;;
      }
      if (branch) branch->parent = p;
    }
    else {
      root = branch;
      if (root) root->parent = nullptr;
    }
  };

  if (n->left || n->right) {
    if (n->left && n->right) {
      auto min = searchmin(n->right);
      auto l = n->left;
      auto r = n->right;
      if (min->right) {
        unattach(min->right);
        transplant(min, min->right);
      }
      transplant(n, min);
      l->parent = min;
      min->left = l;
      if (min != r) {
        r->parent = min;
        min->right = r;
      }
    }
    else if (n->left) {
      transplant(n, n->left);
    }
    else if (n->right) {
      transplant(n, n->right);
    }
  }
  else {
    transplant(n, nullptr);
  }
  delete n;
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
  basic::stack_t<node_t<val_t>*> s{};
  s.push(node);
  while (!s.empty()) {
    auto node = s.pop();
    std::cout << node->value << std::endl;
    if (node->left) {
      s.push(node->left);
    }
    if (node->right) {
      s.push(node->right);
    }
  }
}

template<typename val_t>
void minmaxprintnorec(node_t<val_t>* node)
{
  basic::stack_t<node_t<val_t>*> s{};
  auto current = node;

  while (current || !s.empty()) {
    while (current) {
      s.push(current);
      current = current->left;
    }

    auto t = s.pop();
    std::cout << t->value << " ";
    current = t->right;
  }
  std::cout << std::endl;
}

template<typename val_t, typename output_t>
void minmaxtraversenostack(node_t<val_t>* node, output_t output)
{
  if (!node) return;
  node_t<val_t>* lastNode = node->parent;

  auto iter = [&]() {
    if (node->parent == lastNode) {
      if (!lastNode) {
        lastNode = node;
        if (node->left) {
          node = node->left;
        }
        else {
          output(node);
          node = node->right;
        }
      }
      else if (node == lastNode->left) {
        if (node->left) {
          lastNode = node;
          node = node->left;
        } 
        else if (node->right) {
          output(node);
          lastNode = node;
          node = node->right;
        }
        else {
          output(node);
          lastNode = node;
          node = node->parent;
        }
      } 
      else {
        if (node->left) {
          lastNode = node;
          node = node->left;
        }
        else if (node->right) {
          output(node);
          lastNode = node;
          node = node->right;
        } else {
          output(node);
          lastNode = node;
          node = node->parent;
        }
      }
    } 
    else {
      if (node->left == lastNode) {
        if (node->right) {
          output(node);
          lastNode = node;
          node = node->right;
        } else {
          output(node);
          lastNode = node;
          node = node->parent;
        }
      }
      else {
        if (node->parent) {
          lastNode = node;
          node = node->parent;
        } else {
          if (lastNode == node->right) {
            node = nullptr;
            return;
          }
          lastNode = node;
          node = node->right;
        }
      }
    }
  };
  while (node) {
    iter();
  }
}

template<typename val_t>
void minmaxprintnorecnostack(node_t<val_t>* node)
{
  minmaxtraversenostack(node, [](node_t<val_t>* node) {
    std::cout << node->value << " ";
  });
  std::cout << std::endl;
}

template<typename val_t>
int64_t count(node_t<val_t>* root)
{
  int64_t count = 0;
  minmaxtraversenostack(root, [&](auto _) {
    count += 1;
  });
  return count;
}

template<typename val_t>
int64_t height(node_t<val_t>* root)
{
  auto leafHeight = [](node_t<val_t>* leaf) {
    int64_t height = 0;
    while (leaf) {
      height += 1;
      leaf = leaf->parent;
    }
    return height;
  };
  int64_t maxHeight = 0;
  minmaxtraversenostack(root, [&](auto node) {
    if (!node->left && !node->right) {
      auto height = leafHeight(node);
      if (height > maxHeight) {
        maxHeight = height;
      }
    }
  });
  return maxHeight;
}

}

namespace basic::radixbint {
  using binnode_t = bst::node_t<bool>;

  void insertbin(binnode_t*& root, std::vector<bool> xs)
  {
    if (!root) root = new binnode_t{nullptr, nullptr, nullptr, false};
    binnode_t** current = &root;
    auto parent = (*current)->parent;
    for (uint64_t i = 0; i < xs.size(); i++) {
      bool terminator = i == (xs.size() - 1);
      parent = *current;
      current = xs.at(i) ? &(*current)->right : &(*current)->left;
      if (!*current) {
        *current = new binnode_t{parent, nullptr, nullptr, terminator};
      }
    }
  }

  binnode_t* searchbin(binnode_t* node, std::vector<bool> xs)
  {
    for (bool x : xs) {
      if (!node) break;
      if (x) {
        node = node->right;
      }
      else {
        node = node->left;
      }
    }
    return node;
  }

  void printbin(binnode_t* node)
  {
    basic::stack_t<char> s;
    while(node && node->parent) {
      s.push((node->parent->left == node ? '0' : '1'));
      node = node->parent;
    }
    while (!s.empty()) {
      std::cout << s.pop();
    }
    std::cout << std::endl;
  }

  void minmaxprint(binnode_t* root)
  {
    basic::queue_t<binnode_t*> q;
    bst::minmaxtraversenostack(root, [&](auto node) {
        if (node->value) q.enqueue(node);
    });
    while (!q.empty()) {
      printbin(q.dequeue());
    }
  }
}

// 1. every node is either red or black
// 2. the root is black
// 3. every leaf (the nils) are black
// 4. if a node is red, then both of its children are black
// 5. for any node A, the path from its leafs to that node A contain the same amount of black nodes
namespace basic::rbt {
  template<typename val_t>
  struct rbdata_t {
    val_t value;
    // false => black
    // true  => red
    bool color;
  };

  template<typename val_t>
  using rbnode_t = bst::node_t<rbdata_t<val_t>>;

  template<typename val_t>
  rbnode_t<val_t>* insert(rbnode_t<val_t>*& root, val_t value)
  {
    return nullptr;
  }

  // x is parent
  // y is child
  //
  // bh(x) = if y is red then bh(y) else bh(y) + 1
  // bh(y) = if y is red then bh(x) else bh(x) - 1
  // bh(y) <= bh(x)
  // h(y) < h(x)
  template<typename val_t>
  int64_t bh(rbnode_t<val_t>* node) 
  {
    int64_t count = 0;
    if (!node) {
      return count;
    }
    do {
      node = node->left != nullptr ? node->left : node->right;
      count += (!node || !node->color) ? 1 : 0;
    } while(node);
    return count;
  }
}









// direct access table implementation for excercise 11.1-4
namespace basic {
  struct storage_entry {
    size_t registry_idx;
    int value;
    int key;
  };

  struct dat_t {
    std::array<storage_entry, 256> storage;
    stack_t<int> registry_stack;

    dat_t() : registry_stack{32} {};

    storage_entry* search(int key) {
      auto r = registry_stack.peek(storage[key].registry_idx);
      if (!(r && *r == key)) return nullptr;
      return &storage[key];
    }

    void insert(int key, int value) {
      auto r = registry_stack.peek(storage[key].registry_idx);
      if (!(r && *r == key)) {
        std::cout << "push: " << key << std::endl;
        registry_stack.push(key);
        storage[key].registry_idx = registry_stack.getHead();
      }
      storage[key].key = key;
      storage[key].value = value;
    }

    void remove(int key) {
      auto r = registry_stack.peek(storage[key].registry_idx);
      if (r && *r == key) {
        *r = -1;
      }
    }
  };
}

void testStack();
void testQueue();
void testDeque();
template<template<typename, typename> class ht_t>
void testHashTable();
void runBSTTests();

int hash_table_main();
int bst_main();
int radixtree_main();

int main() {
  // srand(time(NULL));
  using namespace basic::bst;
  node_t<int>* root = nullptr;
  for (auto i = 0; i < 0b1111; i++) {
    insert(root, i);
  }
  std::cout << "count = " << count(root) << " | height = " << height(root) << std::endl;
  balance(root);
  std::cout << "count = " << count(root) << " | height = " << height(root) << std::endl;
  print(root);
  // min height of "complete" bst is log2(n) + 1. complete tree is perfectly symmetrical
  // max height of a bst is count(bst). such tree
  //
  // how much nodes at minimum does contain a node x in bst?
  // bh = 0 if x is leaf, then 0
  // bh = if x has 1 child, then 1
  // if x has 2 childs then 2
  // if x has 3 childs then 3

  return 0;
}













































int radixtree_main() {
  using namespace basic::radixbint;
  binnode_t* root = nullptr;
  insertbin(root, {0});
  insertbin(root, {0, 1, 1});
  insertbin(root, {1, 0});
  insertbin(root, {1, 0, 0});
  insertbin(root, {1, 0, 1, 1});
  // minmaxtraverse(root);
  auto f = searchbin(root, {1, 0, 1, 1});
  printbin(f);
  minmaxprint(root);
  return 0;
}

int bst_main() {
  runBSTTests();

  srand(time(NULL));
  using namespace basic::bst;
  node_t<int>* root = nullptr;
  for (auto i = 0; i < 128; i++) {
    insert(root, rand() / 10000);
  }
  minmaxprintnorec(root);
  // balance(root);
  minmaxprintnorecnostack(root);
  std::cout << searchiter(root, 135642) << std::endl;
  std::cout << searchmin(root)->value << std::endl;
  std::cout << searchmax(root)->value << std::endl;

  auto min = searchmin(root);
  while (min) {
    std::cout << min->value << " ";
    min = suc(min);
  }
  std::cout << std::endl;
  do {
    node_t<int>* root = nullptr;
    insert(root, 15);
    insert(root, 17);
    insert(root, 16);
    insert(root, 18);

    insert(root, 3);
    insert(root, 2);
    insert(root, 9);
    insert(root, 6);
    insert(root, 12);
    insert(root, 8);
    insert(root, 10);

    auto max = searchmax(root);
    while (max) {
      std::cout << max->value << std::endl;
      max = pred(max);
    }

    print(root);
    std::cout << "=================" << std::endl;
    remove(root, 3);
    print(root);

  } while(0);
  return 0;
}

int hash_table_main() {
  testHashTable<basic::oa_hash_table_t>();
  return 0;
  do {
    using namespace basic;
    chain_hash_table_t<std::string, int> ht;
    ht.set("hello world", 123);
    auto val = ht.get("hello world!");
    std::cout << val.has_value() << std::endl;
    return 0;
  } while(0);
  do {
    using namespace basic;
    dlistnode_t<int> root{1, nullptr, nullptr};
    root.insert(2);
    root.insert(3);
    root.print();

    dlistnode_t<int>* sentinel = dlistnode_t<int>::sentinel();
    sentinel->insert(1);
    sentinel->insert(2);
    sentinel->insert(3);
    sentinel->prev->insert(4);
    sentinel->prev->insert(5);
    std::cout << sentinel->search(sentinel, 4) << std::endl;
    sentinel->print(sentinel);
    return 0;
  } while(0);
  do {
    using namespace basic;
    dat_t dat;
    std::cout << dat.search(0) << std::endl;
    dat.remove(0);
    dat.insert(0, 123);
    std::cout << dat.search(0)->value << std::endl;
    return 0;
  } while(0);
  do {
    using namespace basic;
    bst::node_t<int>* root = nullptr;
    for (auto i = 0; i < 16; i++) {
      bst::insert(root, i);
    }
    bst::balance(root);
    bst::printnorec(root);
    return 0;
  } while(0);
  // double linked list with next ^ prev (stored as xor)
  do {
    break;
    using namespace basic;
    dlistxornode_t<int> root{69, 0};
    auto last = root.add(1);
    last = last->add(2);
    last = last->add(3);
    last = last->add(4);
    root.print(nullptr);
    std::cout << root.search(3) << std::endl;
    return 0;
  } while(0);
  // xor double linked list with sentinel
  {
    using namespace basic;
    dlistxornode_t<int> sentinel{0, 0}; 
    auto head = sentinel.insert(nullptr, 1);
    head->insert(&sentinel, 2);
    sentinel.insert(head, 3);
    sentinel.insert(head, 4);
    sentinel.insert(head, 5);
    // prepend
    head = sentinel.insert((dlistxornode_t<int>*)(sentinel.np ^ (uint64_t)head), 6);

    head->print(&sentinel);
    // reverse print
    ((dlistxornode_t<int>*)(sentinel.np ^ (uint64_t)head))->print(&sentinel);
    return 0;
  }
  {
    using namespace basic;

    slistnode_t<int>* head = nullptr;

    auto push = [&](int value) {
      head = new slistnode_t<int>{head, value};
    };

    auto pop = [&]() {
      auto v = head->value;
      auto n = head;
      head = head->next;
      n->next = nullptr;
      delete n;
      return v;
    };

    // non recursive single linked list inverse
    for (int i = 1; i < 11; i++) {
      push(i);
    }
    head->print();
  }
  {
    using namespace basic;
    
    slistnode_t<int>* sentinel = new slistnode_t<int>{nullptr, 0};
    slistnode_t<int>* tail = sentinel;

    auto enqueue = [&](int value) {
      tail = tail->insert(value);
      if (!sentinel->next) sentinel->next = tail;
    };

    auto dequeue = [&]() {
      if (!sentinel->next) throw std::exception{};
      auto v = sentinel->next->value;
      sentinel->remove(sentinel->next);
      return v;
    };

    enqueue(1);
    std::cout << dequeue() << std::endl;
    enqueue(2);
    enqueue(3);
    enqueue(4);
    std::cout << dequeue() << std::endl;
    std::cout << dequeue() << std::endl;
    std::cout << dequeue() << std::endl;
  }
  testStack();
  testQueue();
  testDeque();
  testHashTable<basic::chain_hash_table_t>();
  std::cout << "All tests passed! Your implementations are correct.\n";
  return 0;
}

void testStack() {
    std::cout << "=== Testing Stack Implementation ===\n";
    
    // Test 1: Basic push and pop
    {
        std::cout << "Test 1: Basic push/pop... ";
        basic::stack_t<int> stack(10);
        stack.push(1);
        stack.push(2);
        stack.push(3);
        
        assert(stack.pop() == 3);
        assert(stack.pop() == 2);
        assert(stack.pop() == 1);
        std::cout << "PASSED\n";
    }
    
    // Test 2: LIFO behavior (Last In First Out)
    {
        std::cout << "Test 2: LIFO behavior... ";
        basic::stack_t<int> stack(5);
        stack.push(10);
        stack.push(20);
        stack.push(30);
        
        assert(stack.pop() == 30);
        assert(stack.pop() == 20);
        stack.push(40);
        assert(stack.pop() == 40);
        assert(stack.pop() == 10);
        std::cout << "PASSED\n";
    }
    
    // Test 3: Stack overflow handling
    {
        std::cout << "Test 3: Stack overflow... ";
        basic::stack_t<int> stack(3);
        assert(stack.push(1) == true);
        assert(stack.push(2) == true);
        assert(stack.push(3) == true);
        assert(stack.push(4) == false); // Should fail
        std::cout << "PASSED\n";
    }
    
    // Test 4: Stack underflow handling
    {
        std::cout << "Test 4: Stack underflow... ";
        basic::stack_t<int> stack(3);
        
        // Try to pop from empty stack - should handle gracefully
        try {
          stack.pop(); // Assuming pop returns bool or throws
          assert(false);
        } catch (std::exception& e) {
        }
        // If your implementation returns bool for success:
        // assert(result == false);
        // If it throws exceptions, you'd catch here
        
        stack.push(100);
        assert(stack.pop() == 100);
        
        // Try pop again on empty
        try {
          stack.pop();
          assert(false);
        } catch (std::exception& e) {
        }
        // assert(result == false);
        std::cout << "PASSED\n";
    }
    
    // Test 5: Multiple data types and default constructor
    {
        std::cout << "Test 5: Default constructor and multiple operations... ";
        basic::stack_t<int> stack; // Default size 256
        
        // Push many elements
        for (int i = 0; i < 100; i++) {
            stack.push(i);
        }
        
        // Pop in reverse order
        for (int i = 99; i >= 0; i--) {
            assert(stack.pop() == i);
        }
        std::cout << "PASSED\n";
    }
    
    std::cout << "All stack tests completed successfully!\n\n";
}

void testQueue() {
  using namespace basic;
    std::cout << "=== Testing Queue Implementation ===\n";
    
    // Test 1: Basic enqueue and dequeue
    {
        std::cout << "Test 1: Basic enqueue/dequeue... ";
        queue_t<int> queue(10);
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        
        assert(queue.dequeue() == 1);
        assert(queue.dequeue() == 2);
        assert(queue.dequeue() == 3);
        std::cout << "PASSED\n";
    }
    
    // Test 2: FIFO behavior (First In First Out)
    {
        std::cout << "Test 2: FIFO behavior... ";
        queue_t<int> queue(5);
        queue.enqueue(10);
        queue.enqueue(20);
        queue.enqueue(30);
        
        assert(queue.dequeue() == 10);
        assert(queue.dequeue() == 20);
        queue.enqueue(40);
        queue.enqueue(50);
        assert(queue.dequeue() == 30);
        assert(queue.dequeue() == 40);
        assert(queue.dequeue() == 50);
        std::cout << "PASSED\n";
    }
    
    // Test 3: Queue overflow handling
    {
        std::cout << "Test 3: Queue overflow... ";
        queue_t<int> queue(3);
        assert(queue.enqueue(1) == true);
        assert(queue.enqueue(2) == true);
        assert(queue.enqueue(3) == true);
        assert(queue.enqueue(4) == false); // Should fail
        std::cout << "PASSED\n";
    }
    
    // Test 4: Queue underflow handling
    {
        std::cout << "Test 4: Queue underflow... ";
        queue_t<int> queue(3);
        
        // Try to dequeue from empty queue
        try { 
          queue.dequeue();
          assert(false);
        } catch (std::exception& e) {}
        
        queue.enqueue(100);
        assert(queue.dequeue() == 100);
        
        try { 
          queue.dequeue();
          assert(false);
        } catch (std::exception& e) {}

        std::cout << "PASSED\n";
    }
    
    // Test 5: Circular buffer behavior
    {
        std::cout << "Test 5: Circular buffer behavior... ";
        queue_t<int> queue(4); // Small size to force wrap-around
        
        // Phase 1: Fill, partially empty, create gap
        queue.enqueue(10);
        queue.enqueue(20);
        queue.enqueue(30);
        queue.enqueue(40); // Full
        
        assert(queue.dequeue() == 10); // Create gap at front
        assert(queue.dequeue() == 20); // Create more gap
        
        // Phase 2: Test wrap-around by filling the gap
        queue.enqueue(50); // Should go to former 10's position
        queue.enqueue(60); // Should go to former 20's position
        
        // Queue should be: [50,60,30,40] in circular arrangement
        
        assert(queue.dequeue() == 30);
        assert(queue.dequeue() == 40);
        assert(queue.dequeue() == 50);
        assert(queue.dequeue() == 60);
        
        std::cout << "PASSED\n";
    }
    
    std::cout << "All queue tests completed successfully!\n\n";
}

void testDeque() {
  using namespace basic;
    std::cout << "=== Testing Deque Implementation ===\n";
    
    // Test 1: Basic front operations
    {
        std::cout << "Test 1: Basic front operations... ";
        deque_t<int> deque{10};
        
        deque.enqueueFront(3);
        deque.enqueueFront(2);
        deque.enqueueFront(1);
        
        assert(deque.dequeueFront() == 1);
        assert(deque.dequeueFront() == 2);
        assert(deque.dequeueFront() == 3);
        std::cout << "PASSED\n";
    }
    
    // Test 2: Basic back operations
    {
        std::cout << "Test 2: Basic back operations... ";
        deque_t<int> deque(10);
        
        deque.enqueueBack(1);
        deque.enqueueBack(2);
        deque.enqueueBack(3);
        
        assert(deque.dequeueBack() == 3);
        assert(deque.dequeueBack() == 2);
        assert(deque.dequeueBack() == 1);
        std::cout << "PASSED\n";
    }
    
    // Test 3: Mixed operations - front and back
    {
        std::cout << "Test 3: Mixed front/back operations... ";
        deque_t<int> deque(10);
        
        deque.enqueueFront(2);
        deque.enqueueBack(3);
        deque.enqueueFront(1);
        deque.enqueueBack(4);
        
        assert(deque.dequeueFront() == 1);
        assert(deque.dequeueBack() == 4);
        assert(deque.dequeueFront() == 2);
        assert(deque.dequeueBack() == 3);
        std::cout << "PASSED\n";
    }
    
    // Test 4: Underflow handling
    {
        std::cout << "Test 4: Underflow handling... ";
        deque_t<int> deque(5);
        
        // Test empty deque exceptions
        try {
            deque.dequeueFront();
            assert(false && "Should have thrown exception");
        } catch (const std::exception&) {
            // Expected
        }
        
        try {
            deque.dequeueBack();
            assert(false && "Should have thrown exception");
        } catch (const std::exception&) {
            // Expected
        }
        
        // Add and remove one element
        deque.enqueueFront(100);
        assert(deque.dequeueBack() == 100);
        
        // Should be empty again
        try {
            deque.dequeueFront();
            assert(false && "Should have thrown exception");
        } catch (const std::exception&) {
            // Expected
        }
        std::cout << "PASSED\n";
    }
    
    // Test 5: Overflow handling
    {
        std::cout << "Test 5: Overflow handling... ";
        deque_t<int> deque(3);
        
        assert(deque.enqueueFront(1) == true);
        assert(deque.enqueueBack(2) == true);
        assert(deque.enqueueFront(3) == true);
        assert(deque.enqueueBack(4) == false); // Should fail
        assert(deque.enqueueFront(5) == false); // Should fail
        
        // Verify existing elements
        assert(deque.dequeueFront() == 3);
        assert(deque.dequeueFront() == 1);
        assert(deque.dequeueFront() == 2);
        std::cout << "PASSED\n";
    }
    
    // Test 6: Circular behavior from front
    {
        std::cout << "Test 6: Circular behavior (front)... ";
        deque_t<int> deque(4);
        
        // Fill from front, creating wrap-around
        deque.enqueueFront(1);
        deque.enqueueFront(2);
        deque.enqueueFront(3);
        deque.enqueueFront(4); // Full
        
        assert(deque.dequeueBack() == 1); // Remove from back
        deque.enqueueFront(5); // Should wrap around
        
        assert(deque.dequeueBack() == 2);
        assert(deque.dequeueBack() == 3);
        assert(deque.dequeueBack() == 4);
        assert(deque.dequeueBack() == 5);
        std::cout << "PASSED\n";
    }
    
    // Test 7: Circular behavior from back
    {
        std::cout << "Test 7: Circular behavior (back)... ";
        deque_t<int> deque(4);
        
        // Fill from back, create wrap-around scenario
        deque.enqueueBack(1);
        deque.enqueueBack(2);
        deque.enqueueBack(3);
        deque.enqueueBack(4); // Full
        
        assert(deque.dequeueFront() == 1); // Remove from front
        deque.enqueueBack(5); // Should wrap around
        
        assert(deque.dequeueFront() == 2);
        assert(deque.dequeueFront() == 3);
        assert(deque.dequeueFront() == 4);
        assert(deque.dequeueFront() == 5);
        std::cout << "PASSED\n";
    }
    
    // Test 8: Complex mixed scenario
    {
        std::cout << "Test 8: Complex mixed scenario... ";
        deque_t<int> deque(6);
        
        // Initial setup
        deque.enqueueFront(30);
        deque.enqueueBack(40);
        deque.enqueueFront(20);
        deque.enqueueBack(50);
        deque.enqueueFront(10);
        deque.enqueueBack(60); // Full: [10, 20, 30, 40, 50, 60]
        
        // Mixed removals
        assert(deque.dequeueFront() == 10);
        assert(deque.dequeueBack() == 60);
        assert(deque.dequeueFront() == 20);
        assert(deque.dequeueBack() == 50);
        
        // Add more with wrap-around
        deque.enqueueFront(5);
        deque.enqueueBack(55);
        deque.enqueueFront(0);
        deque.enqueueBack(66); // Full again
        
        // Final verification
        assert(deque.dequeueFront() == 0);
        assert(deque.dequeueFront() == 5);
        assert(deque.dequeueFront() == 30);
        assert(deque.dequeueFront() == 40);
        assert(deque.dequeueFront() == 55);
        assert(deque.dequeueFront() == 66);
        std::cout << "PASSED\n";
    }
    
    // Test 9: Single element behavior
    {
        std::cout << "Test 9: Single element behavior... ";
        deque_t<int> deque(5);
        
        deque.enqueueFront(100);
        assert(deque.dequeueFront() == 100);
        
        deque.enqueueBack(200);
        assert(deque.dequeueBack() == 200);
        
        deque.enqueueFront(300);
        assert(deque.dequeueBack() == 300);
        
        deque.enqueueBack(400);
        assert(deque.dequeueFront() == 400);
        std::cout << "PASSED\n";
    }
    
    // Test 10: Stress test with default size
    {
        std::cout << "Test 10: Stress test with default size... ";
        deque_t<int> deque; // Default size 256
        
        // Add many elements alternating front/back
        for (int i = 0; i < 100; i++) {
            if (i % 2 == 0) {
                deque.enqueueFront(i);
            } else {
                deque.enqueueBack(i);
            }
        }
        
        // Remove alternating front/back
        for (int i = 0; i < 100; i++) {
            if (i % 2 == 0) {
                deque.dequeueFront();
            } else {
                deque.dequeueBack();
            }
        }
        
        // Should be empty
        try {
            deque.dequeueFront();
            assert(false && "Should have thrown exception");
        } catch (const std::exception&) {
            // Expected
        }
        std::cout << "PASSED\n";
    }
    
    std::cout << "All deque tests completed successfully!\n\n";
}

template<template<typename, typename> class ht_t>
void testHashTable() {
  using namespace basic;
    std::cout << "=== Testing Hash Table Implementation ===\n";
    
    // Test 1: Basic set and get
    {
        std::cout << "Test 1: Basic set/get operations... ";
        ht_t<std::string, int> table;
        
        table.set("one", 1);
        table.set("two", 2);
        table.set("three", 3);
        
        assert(table.get("one").value() == 1);
        assert(table.get("two").value() == 2);
        assert(table.get("three").value() == 3);
        std::cout << "PASSED\n";
    }
    
    // Test 2: Key updates (overwriting values)
    {
        std::cout << "Test 2: Key value updates... ";
        ht_t<std::string, std::string> table;
        
        table.set("name", "Alice");
        assert(table.get("name").value() == "Alice");
        
        table.set("name", "Bob");  // Overwrite
        assert(table.get("name").value() == "Bob");
        
        table.set("name", "Charlie");  // Overwrite again
        assert(table.get("name").value() == "Charlie");
        std::cout << "PASSED\n";
    }
    
    // Test 3: Remove operations
    {
        std::cout << "Test 3: Remove operations... ";
        ht_t<int, double> table;
        
        table.set(1, 1.1);
        table.set(2, 2.2);
        table.set(3, 3.3);
        
        assert(table.get(2).value() == 2.2);
        table.remove(2);
        assert(!table.get(2).has_value());  // Should be empty
        
        // Remove non-existent key (should not crash)
        table.remove(999);
        
        // Verify other keys still exist
        assert(table.get(1).value() == 1.1);
        assert(table.get(3).value() == 3.3);
        std::cout << "PASSED\n";
    }
    
    // Test 4: Non-existent keys
    {
        std::cout << "Test 4: Non-existent keys... ";
        ht_t<std::string, int> table;
        
        // Empty table should return empty optional
        assert(!table.get("nonexistent").has_value());
        
        table.set("exists", 42);
        
        // Different key should still return empty
        assert(!table.get("different").has_value());
        assert(table.get("exists").value() == 42);
        std::cout << "PASSED\n";
    }
    
    // Test 5: Hash collisions
    {
        std::cout << "Test 5: Hash collisions... ";
        ht_t<int, std::string> table;
        
        // Insert multiple keys that might collide
        // (depending on your hash function and table size)
        for (int i = 0; i < 100; i++) {
            table.set(i, "value_" + std::to_string(i));
        }
        
        // Verify all can be retrieved correctly
        for (int i = 0; i < 100; i++) {
            auto value = table.get(i);
            assert(value.has_value());
            assert(value.value() == "value_" + std::to_string(i));
        }
        std::cout << "PASSED\n";
    }
    
    // Test 6: Stress test with many operations
    {
        std::cout << "Test 6: Stress test... ";
        ht_t<int, int> table;
        
        // Insert many elements
        for (int i = 0; i < 1000; i++) {
            table.set(i, i * 10);
        }
        
        // Verify and update
        for (int i = 0; i < 1000; i++) {
            assert(table.get(i).value() == i * 10);
            if (i % 2 == 0) {
                table.set(i, i * 20);  // Update even keys
            }
        }
        
        // Remove half
        for (int i = 0; i < 1000; i += 2) {
            table.remove(i);
        }
        
        // Verify final state
        for (int i = 0; i < 1000; i++) {
            if (i % 2 == 0) {
                assert(!table.get(i).has_value());
            } else {
                assert(table.get(i).value() == i * 10);
            }
        }
        std::cout << "PASSED\n";
    }
    
    // Test 7: Complex value types
    {
        std::cout << "Test 7: Complex value types... ";
        ht_t<std::string, std::vector<int>> table;
        
        std::vector<int> vec1 = {1, 2, 3};
        std::vector<int> vec2 = {4, 5, 6, 7};
        
        table.set("vector1", vec1);
        table.set("vector2", vec2);
        
        auto result1 = table.get("vector1");
        auto result2 = table.get("vector2");
        
        assert(result1.has_value());
        assert(result2.has_value());
        assert(result1.value().size() == 3);
        assert(result2.value().size() == 4);
        assert(result1.value()[0] == 1);
        assert(result2.value()[3] == 7);
        std::cout << "PASSED\n";
    }
    
    // Test 8: Memory management (constructor/destructor)
    {
        std::cout << "Test 8: Memory management... ";
        {
            ht_t<int, std::string> table;
            table.set(1, "test1");
            table.set(2, "test2");
            table.set(3, "test3");
            
            // Table should clean up properly when it goes out of scope
        }
        // If we get here without memory leaks, test passes
        std::cout << "PASSED\n";
    }
    
    // Test 9: Mixed operations sequence
    {
        std::cout << "Test 9: Mixed operations sequence... ";
        ht_t<std::string, int> table;
        
        // Sequence of mixed operations
        table.set("a", 1);
        table.set("b", 2);
        assert(table.get("a").value() == 1);
        
        table.remove("a");
        assert(!table.get("a").has_value());
        
        table.set("b", 20);  // Update existing
        table.set("c", 3);
        table.set("d", 4);
        
        table.remove("c");
        table.set("e", 5);
        
        // Final verification
        assert(!table.get("a").has_value());
        assert(table.get("b").value() == 20);
        assert(!table.get("c").has_value());
        assert(table.get("d").value() == 4);
        assert(table.get("e").value() == 5);
        std::cout << "PASSED\n";
    }
    
    // Test 10: String keys with special characters
    {
        std::cout << "Test 10: Complex string keys... ";
        ht_t<std::string, int> table;
        
        table.set("hello world", 1);
        table.set("key-with-dashes", 2);
        table.set("key/with/slashes", 3);
        table.set("key.with.dots", 4);
        table.set("", 5);  // Empty string key
        table.set("key with spaces", 6);
        
        assert(table.get("hello world").value() == 1);
        assert(table.get("key-with-dashes").value() == 2);
        assert(table.get("key/with/slashes").value() == 3);
        assert(table.get("key.with.dots").value() == 4);
        assert(table.get("").value() == 5);
        assert(table.get("key with spaces").value() == 6);
        std::cout << "PASSED\n";
    }
    
    std::cout << "All hash table tests completed successfully!\n\n";
}

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace basic::bst;

void testBST() {
    std::cout << "=== Testing BST Implementation ===\n";
    
    // Test 1: Basic insert and search
    {
        std::cout << "Test 1: Basic insert/search... ";
        node_t<int>* root = nullptr;
        
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 9);

        std::cout << root << std::endl;
        print(root);
        
        assert(search(root, 5) != nullptr);
        assert(search(root, 3) != nullptr);
        assert(search(root, 7) != nullptr);
        assert(search(root, 1) != nullptr);
        assert(search(root, 9) != nullptr);
        assert(search(root, 99) == nullptr); // Non-existent
        
        // Cleanup
        // You'll need to implement a tree deletion function
        std::cout << "PASSED\n";
    }
    
    // Test 2: Iterative search
    {
        std::cout << "Test 2: Iterative search... ";
        node_t<int>* root = nullptr;
        
        insert(root, 10);
        insert(root, 5);
        insert(root, 15);
        insert(root, 2);
        insert(root, 7);
        
        assert(searchiter(root, 10) != nullptr);
        assert(searchiter(root, 5) != nullptr);
        assert(searchiter(root, 15) != nullptr);
        assert(searchiter(root, 2) != nullptr);
        assert(searchiter(root, 7) != nullptr);
        assert(searchiter(root, 99) == nullptr);
        
        std::cout << "PASSED\n";
    }
    
    // Test 3: Min and max search
    {
        std::cout << "Test 3: Min/max search... ";
        node_t<int>* root = nullptr;
        
        insert(root, 8);
        insert(root, 3);
        insert(root, 10);
        insert(root, 1);
        insert(root, 6);
        insert(root, 14);
        insert(root, 4);
        insert(root, 7);
        insert(root, 13);
        
        auto* minNode = searchmin(root);
        auto* maxNode = searchmax(root);
        
        assert(minNode != nullptr && minNode->value == 1);
        assert(maxNode != nullptr && maxNode->value == 14);
        
        // Test min/max on subtrees
        auto* subtreeMin = searchmin(root->right); // right subtree
        auto* subtreeMax = searchmax(root->left);  // left subtree
        
        assert(subtreeMin != nullptr && subtreeMin->value == 10);
        assert(subtreeMax != nullptr && subtreeMax->value == 7);
        
        std::cout << "PASSED\n";
    }
    
    // Test 4: Predecessor and successor
    {
        std::cout << "Test 4: Predecessor/successor... ";
        node_t<int>* root = nullptr;
        
        // Build a balanced tree for testing
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 4);
        insert(root, 6);
        insert(root, 9);
        
        // Test successor
        auto* node5 = search(root, 5);
        auto* node3 = search(root, 3);
        auto* node7 = search(root, 7);
        auto* node1 = search(root, 1);
        auto* node4 = search(root, 4);
        auto* node6 = search(root, 6);
        auto* node9 = search(root, 9);
        
        assert(suc(node5)->value == 6);   // 5 -> 6
        assert(suc(node3)->value == 4);   // 3 -> 4
        assert(suc(node4)->value == 5);   // 4 -> 5
        assert(suc(node6)->value == 7);   // 6 -> 7
        assert(suc(node9) == nullptr);    // 9 has no successor
        
        // Test predecessor
        assert(pred(node5)->value == 4);  // 5 -> 4
        assert(pred(node3)->value == 1);  // 3 -> 1
        assert(pred(node4)->value == 3);  // 4 -> 3
        assert(pred(node6)->value == 5);  // 6 -> 5
        assert(pred(node1) == nullptr);   // 1 has no predecessor
        
        std::cout << "PASSED\n";
    }
    
    // Test 5: Remove leaf nodes
    {
        std::cout << "Test 5: Remove leaf nodes... ";
        node_t<int>* root = nullptr;
        
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 4);
        
        // Remove leaf node 1
        remove(root, 1);
        assert(search(root, 1) == nullptr);
        assert(search(root, 3) != nullptr);
        assert(root->left->left == nullptr); // 1 should be gone
        
        // Remove leaf node 4
        remove(root, 4);
        assert(search(root, 4) == nullptr);
        assert(search(root, 3) != nullptr);
        assert(root->left->right == nullptr); // 4 should be gone
        
        // Remove leaf node 7
        remove(root, 7);
        assert(search(root, 7) == nullptr);
        assert(root->right == nullptr);
        
        std::cout << "PASSED\n";
    }
    
    // Test 6: Remove nodes with one child
    {
        std::cout << "Test 6: Remove nodes with one child... ";
        node_t<int>* root = nullptr;
        
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 4);
        insert(root, 6);
        insert(root, 9);
        insert(root, 8);
        
        // Remove node 7 which has two children, but let's first create a one-child scenario
        // Remove node 6 first (leaf)
        remove(root, 6);
        
        // Now node 7 has only right child (9)
        remove(root, 7);
        assert(search(root, 7) == nullptr);
        assert(root->right->value == 9); // 9 should take 7's place
        assert(root->right->left != nullptr && root->right->left->value == 8);
        
        std::cout << "PASSED\n";
    }
    
    // Test 7: Remove nodes with two children
    {
        std::cout << "Test 7: Remove nodes with two children... ";
        node_t<int>* root = nullptr;
        
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 4);
        insert(root, 6);
        insert(root, 9);
        insert(root, 8);
        
        // Remove root (5) which has two children
        remove(root, 5);
        assert(search(root, 5) == nullptr);
        
        // The new root should be 6 (successor of 5)
        assert(root != nullptr);
        assert(root->value == 6);
        
        // Verify tree structure is maintained
        assert(search(root, 3) != nullptr);
        assert(search(root, 7) != nullptr);
        assert(search(root, 1) != nullptr);
        assert(search(root, 4) != nullptr);
        assert(search(root, 9) != nullptr);
        assert(search(root, 8) != nullptr);
        
        std::cout << "PASSED\n";
    }
    
    // Test 8: Tree balancing
    {
        std::cout << "Test 8: Tree balancing... ";
        node_t<int>* root = nullptr;
        
        // Create unbalanced tree (degenerate case)
        insert(root, 1);
        insert(root, 2);
        insert(root, 3);
        insert(root, 4);
        insert(root, 5);
        insert(root, 6);
        insert(root, 7);
        
        // Before balancing, tree should be a linked list to the right
        // After balancing, it should be more balanced
        
        // Store original height information if you have height function
        balance(root);
        
        // After balancing, all original values should still be present
        for (int i = 1; i <= 7; i++) {
            assert(search(root, i) != nullptr);
        }
        
        // The tree should be more balanced now (you could add height checks)
        
        std::cout << "PASSED\n";
    }
    
    // Test 9: Parent pointer integrity
    {
        std::cout << "Test 9: Parent pointer integrity... ";
        node_t<int>* root = nullptr;
        
        insert(root, 5);
        insert(root, 3);
        insert(root, 7);
        insert(root, 1);
        insert(root, 4);
        
        // Check parent pointers after insertion
        auto* node3 = search(root, 3);
        auto* node1 = search(root, 1);
        auto* node4 = search(root, 4);
        auto* node7 = search(root, 7);
        
        assert(node3->parent == root);
        assert(node1->parent == node3);
        assert(node4->parent == node3);
        assert(node7->parent == root);
        
        // Remove node 3 and check parent pointers
        remove(root, 3);

        // After removal, check that parent pointers are updated correctly
        // This depends on which node replaces 3
        auto* replacement = root->left;
        if (replacement) {
            assert(replacement->parent == root);
        }
        
        std::cout << "PASSED\n";
    }
    
    // Test 10: Complex removal scenarios
    {
        std::cout << "Test 10: Complex removal scenarios... ";
        node_t<int>* root = nullptr;
        
        // Build a more complex tree
        insert(root, 10);
        insert(root, 5);
        insert(root, 15);
        insert(root, 3);
        insert(root, 7);
        insert(root, 12);
        insert(root, 18);
        insert(root, 1);
        insert(root, 4);
        insert(root, 6);
        insert(root, 8);
        insert(root, 11);
        insert(root, 14);
        insert(root, 17);
        insert(root, 20);
        
        // Remove node with two children (15) whose successor (17) has a right child
        remove(root, 15);
        assert(search(root, 15) == nullptr);
        
        // The successor (17) should replace 15
        // And 17's right child (if any) should be handled properly
        
        // Remove another complex case (5)
        remove(root, 5);
        assert(search(root, 5) == nullptr);
        
        // Verify all other nodes still exist
        int remaining[] = {1, 3, 4, 6, 7, 8, 10, 11, 12, 14, 17, 18, 20};
        for (int val : remaining) {
            assert(search(root, val) != nullptr);
        }
        
        std::cout << "PASSED\n";
    }
    
    std::cout << "All BST tests completed successfully!\n\n";
}

// Additional test for the non-recursive traversals
void testBSTTraversals() {
    std::cout << "=== Testing BST Traversals ===\n";
    
    // Test iterative in-order traversal
    {
        std::cout << "Test: Iterative in-order traversal... ";
        node_t<int>* root = nullptr;
        
        insert(root, 4);
        insert(root, 2);
        insert(root, 6);
        insert(root, 1);
        insert(root, 3);
        insert(root, 5);
        insert(root, 7);
        
        // We'll test by capturing output (you might want to modify to return vector)
        // For now, we'll just call it to ensure no crashes
        minmaxprintnorec(root);
        
        std::cout << "PASSED (no crash)\n";
    }
    
    // Test stack-free in-order traversal
    {
        std::cout << "Test: Stack-free in-order traversal... ";
        node_t<int>* root = nullptr;
        
        insert(root, 4);
        insert(root, 2);
        insert(root, 6);
        insert(root, 1);
        insert(root, 3);
        insert(root, 5);
        insert(root, 7);
        
        minmaxprintnorecnostack(root);
        
        std::cout << "PASSED (no crash)\n";
    }
}

// Helper function to delete entire tree (needed for cleanup)
template<typename T>
void deleteTree(node_t<T>* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Memory leak check wrapper
void runBSTTests() {
    testBST();
    testBSTTraversals();
    std::cout << "All BST tests completed! Consider implementing tree cleanup.\n";
}




//
// m - # of slots
// n = # of elements in hash table total
// alpha = n / m = # length of bucket / # of slots
// f = search(hash_table, key)
// O(f) proporional to n
// O(f) proportional to 1 / m since more slots, less elements in bucket
// O(f) proportional to alpha

// page 303













