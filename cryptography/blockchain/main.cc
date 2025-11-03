#include <cstdint>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <cassert>
#include <numeric>
#include <algorithm>

using namespace std;
namespace rsa {
  struct publickey_t {
    uint64_t exponent;
    uint64_t modulo;

    vector<uint8_t> serialize() const;
    uint64_t hash() const;
  };
  std::vector<size_t> process(std::vector<size_t> message, int exponent, int modulo);
  std::vector<size_t> prepare_message(const std::string& message, size_t n);
  void print_message(std::vector<size_t> message);
  void make_keys(uint64_t p, uint64_t q, uint64_t& n, uint64_t& e, uint64_t& d);
}

template<typename T>
void printxs(const T& xs);
pair<uint32_t, uint32_t> dwordDecay(uint64_t n);
pair<uint32_t, uint32_t> dwordBound(uint64_t n);
uint64_t dwordSwapped(uint64_t n);

uint64_t feistel(uint64_t k, uint64_t m, function<uint32_t(uint32_t x, uint64_t k)> f, bool reverse = false, size_t N = 3);
uint64_t ciph(uint64_t m, uint64_t k, bool decrypt = false);
uint64_t hash8(vector<uint8_t> ms);
vector<uint64_t> merkleTree(const vector<vector<uint8_t>>& xss);
string hashToHex(uint64_t hash);

std::vector<uint8_t> u64bytes(uint64_t h);
uint64_t bytes2u64(std::vector<uint8_t> c);

using signature_t = std::vector<size_t>;
signature_t sign(const vector<uint8_t>& message, uint64_t privateKey, uint64_t n);
bool verify(const vector<uint8_t>& message, signature_t signature, uint64_t publicKey, uint64_t n);

namespace ledger {

  struct txinput_t {
    const uint64_t prevOutTxHash;
    const uint64_t prevOutIndexInTx;

    signature_t signature;
    rsa::publickey_t signaturePublicKey;

    txinput_t(uint64_t prevOutTxHash, uint64_t prevOutIndexInTx) 
      : prevOutTxHash(prevOutTxHash)
        , prevOutIndexInTx(prevOutIndexInTx) {}

    std::vector<uint8_t> serialize() const 
    {
      std::vector<uint8_t> out;
      auto bytes = u64bytes(prevOutTxHash);
      out.insert(out.end(), bytes.begin(), bytes.end());
      bytes = u64bytes(prevOutIndexInTx);
      out.insert(out.end(), bytes.begin(), bytes.end());
      out.insert(out.end(), signature.begin(), signature.end());
      bytes = signaturePublicKey.serialize();
      out.insert(out.end(), bytes.begin(), bytes.end());
      return out;
    }
  };

  struct txoutput_t {
    const double value;
    const uint64_t recipientPublicKeyHash;

    txoutput_t(double value, uint64_t recipientPublicKeyHash)
      : value(value)
        , recipientPublicKeyHash(recipientPublicKeyHash) {}

    std::vector<uint8_t> serialize() const 
    {
      std::vector<uint8_t> out;
      auto bytes = u64bytes(*(uint64_t*)&value);
      out.insert(out.end(), bytes.begin(), bytes.end());
      bytes = u64bytes(recipientPublicKeyHash);
      out.insert(out.end(), bytes.begin(), bytes.end());
      return out;
    }
  };

  struct tx_t {
    uint64_t hash;
    vector<txinput_t> ins;
    vector<txoutput_t> outs;

    tx_t(vector<txinput_t> ins, vector<txoutput_t> outs)
      : ins(ins)
        , outs(outs) {}

    std::vector<uint8_t> serialize() const
    {
      std::vector<uint8_t> d;
      for (const auto& in : ins) {
        auto inbytes = in.serialize();
        d.insert(d.end(), inbytes.begin(), inbytes.end());
      }
      for (const auto& out : outs) {
        auto outbytes = out.serialize();
        d.insert(d.end(), outbytes.begin(), outbytes.end());
      }
      return d;  
    }

    void sign(uint64_t privateKey, rsa::publickey_t publicKey, const tx_t& previousTx, uint64_t indexN)
    {
      tx_t copy = *this;
      auto recipientPublicKeyHashBytes = u64bytes(previousTx.outs[ins.at(indexN).prevOutIndexInTx].recipientPublicKeyHash);

      for (auto& in : copy.ins) {
        in.signature = {};
        in.signaturePublicKey = {};
      }

      copy.ins[indexN].signature = {recipientPublicKeyHashBytes.begin(), recipientPublicKeyHashBytes.end()};
      copy.ins[indexN].signaturePublicKey = {};

      ins[indexN].signature = ::sign(copy.serialize(), privateKey, publicKey.modulo);
      ins[indexN].signaturePublicKey = publicKey;
    }

    bool verify(const tx_t& previousTx, uint64_t indexN) const
    {
      tx_t copy = *this;
      auto recipientPublicKeyHashBytes = u64bytes(previousTx.outs[ins.at(indexN).prevOutIndexInTx].recipientPublicKeyHash);
      for (auto& in : copy.ins) {
        in.signature = {};
        in.signaturePublicKey = {};
      }

      copy.ins[indexN].signature = {recipientPublicKeyHashBytes.begin(), recipientPublicKeyHashBytes.end()};
      copy.ins[indexN].signaturePublicKey = {};

      return ::verify(copy.serialize(), ins.at(indexN).signature, ins.at(indexN).signaturePublicKey.exponent, ins.at(indexN).signaturePublicKey.modulo);
    }

  };

  struct block_t {
    const uint64_t prevBlockHash;
    std::vector<tx_t> txs;
    uint64_t blockHash;
    uint64_t nonce;

    block_t(uint64_t prevBlockHash) : prevBlockHash(prevBlockHash) {}

    void addCoinbaseTx(double difficultyTarget, uint64_t recipientHash, double fee = 0)
    {
      tx_t t{vector<txinput_t>{}, vector<txoutput_t>{txoutput_t(difficultyTarget, recipientHash), txoutput_t(fee, recipientHash)}};
      txs.push_back(t);
    }

    void commit()
    {
      vector<vector<uint8_t>> blockBytes;
      blockBytes.push_back(u64bytes(prevBlockHash));
      for (uint64_t i = 0; i < txs.size(); i++) {
        blockBytes.push_back(txs.at(i).serialize());
      }

      uint64_t n = 0;
      uint64_t hash = 0;

      blockBytes.push_back(u64bytes(n));
      while (true) {
        hash = merkleTree(blockBytes).back();
        auto shash = hashToHex(hash);
        if (shash.starts_with("00")) {
          printf("Found hash: %s\n", shash.c_str());
          break;
        }
        n += 1;
        blockBytes.back() = u64bytes(n);
      }

      nonce = n;
      blockHash = hash;
    }

    bool verify() const
    {
      vector<vector<uint8_t>> blockBytes;
      blockBytes.push_back(u64bytes(prevBlockHash));
      for (uint64_t i = 0; i < txs.size(); i++) {
        blockBytes.push_back(txs.at(i).serialize());
      }
      blockBytes.push_back(u64bytes(nonce));
      auto hash = merkleTree(blockBytes).back();

      return hash == blockHash && hashToHex(hash).starts_with("00");
    }
  };

  struct blockchain_t {
    std::vector<block_t> blocks;

    block_t makeNewBlock()
    {
      if (blocks.empty()) {
        return block_t{0};
      } else {
        return block_t{blocks.back().blockHash};
      }
    }

    void addTx2Block(block_t& b, tx_t& tx)
    {
      for (uint64_t txindex = 0; txindex < tx.ins.size(); txindex++) {
        tx_t const* previousTxPtr = nullptr;

        for (int64_t blockindex = blocks.size(); blockindex >= 0; blockindex--) {
          const auto& block = blockindex == blocks.size() ? b : blocks.at(blockindex);
          for (int64_t btxindex = block.txs.size() - 1; btxindex >= 0; btxindex--) {
            if (block.txs.at(btxindex).hash == tx.ins.at(txindex).prevOutTxHash) {
              previousTxPtr = &block.txs.at(btxindex);
              break;
            }
          }
        }

        if (!previousTxPtr || !tx.verify(*previousTxPtr, txindex)) {
          std::cerr << "[ERROR] attempt to add unverified tx " << previousTxPtr << std::endl;
          return;
        }
      }

      b.txs.push_back(tx);
    }

    void addGenesisBlock(uint64_t recipientHash, double amount)
    {
      auto genesisBlock = makeNewBlock();
      ledger::tx_t t{vector<ledger::txinput_t>{}, vector<ledger::txoutput_t>{ledger::txoutput_t(amount, recipientHash)}};
      t.hash = hash8(t.serialize());
      genesisBlock.txs.push_back(t);
      genesisBlock.commit();
      addBlock(genesisBlock);
    }

    void addBlock(const block_t& block) 
    {
      if (block.verify() && block.prevBlockHash == (blocks.empty() ? 0 : blocks.back().blockHash)) {
        blocks.push_back(block);
      } else {
        std::cerr << "[ERROR] attempt to add unverified block" << std::endl;
      }
    }

    bool verify() 
    {
      for (int i = blocks.size() - 1; i >= 0; i--) {
        uint64_t prevHash = 0;
        if (i > 0) {
          prevHash = blocks.at(i - 1).blockHash;
        }

        if (!blocks.at(i).verify() || blocks.at(i).prevBlockHash != prevHash) {
          return false;
        }
      }
      return true;
    }

    vector<tx_t> computeUTXO(const vector<tx_t>& pendingtxs, const rsa::publickey_t userPublicKey) const
    { 
      const uint64_t userHash = userPublicKey.hash();
      vector<tx_t> txsWhereUserOutput;

      for (int64_t i = blocks.size(); i >= 0; i--) {
        const auto& txs = i == blocks.size() ? pendingtxs : blocks.at(i).txs;
        bool found = false;
        for (int64_t j = txs.size() - 1; j >= 0; j--) {
          const auto& tx = txs.at(j);
          bool txContainOutputToUser = any_of(tx.outs.begin(), tx.outs.end(), [userHash](auto& out) {
              return out.recipientPublicKeyHash == userHash;
              });
          if (txContainOutputToUser) {
            txsWhereUserOutput.push_back(tx);
          }

          bool txContainInputFromUser = any_of(tx.ins.begin(), tx.ins.end(), [&](auto& in) {
              return in.signaturePublicKey.exponent == userPublicKey.exponent && in.signaturePublicKey.modulo == userPublicKey.modulo;
              });
          if (txContainInputFromUser) {
            found = true;
            break;
          }
        }
        if (found) {
          break;
        }
      }

      return txsWhereUserOutput;
    }

    double availableAmount(const vector<tx_t>& utxo, uint64_t userHash) const
    {
      double userBalance = 0;
      for (const auto& tx : utxo) {
        for (const auto& out : tx.outs) {
          if (out.recipientPublicKeyHash == userHash) {
            userBalance += out.value;
          }
        }
      }
      return userBalance;
    }

    double computeFee(const block_t& block)
    {
      double totalFee = 0;
      for (const auto& tx : block.txs) {
        totalFee += computeFee(tx, block.txs);
      }
      return totalFee;
    }

    double computeFee(const tx_t& tx, const vector<tx_t>& pendingtxs)
    {
      double insAmount = 0;
      for (const auto& in : tx.ins) {
        for (int64_t blockidx = blocks.size(); blockidx >= 0; blockidx--) {
          const auto& txs = blockidx == blocks.size() ? pendingtxs : blocks.at(blockidx).txs;
          for (const auto& tx : txs) {
            if (tx.hash == in.prevOutTxHash) {
              insAmount += tx.outs[in.prevOutIndexInTx].value;
            }
          }
        }
      }

      double outsAmount = 0;
      for (const auto& out : tx.outs) {
        outsAmount += out.value;
      }

      return insAmount - outsAmount;
    }

  };

  struct user_t {
    rsa::publickey_t publicKey;

    user_t(uint64_t p, uint64_t q) {
      rsa::make_keys(p, q, publicKey.modulo, publicKey.exponent, privateKey);
    }

    tx_t makeTx(const blockchain_t& bc, const vector<tx_t>& pendingtxs, const uint64_t recipientPublicKeyHash, double amount, double fee = 0)
    {
      if (bc.blocks.empty()) {
        std::cerr << "[ERROR] attempt to add Genesis Block Tx via user" << std::endl;
        exit(-1);
      }
      auto userHash = publicKey.hash();
      auto utxo = bc.computeUTXO(pendingtxs, publicKey);

      // from new to old
      if (utxo.empty() == true) {
        std::cerr << "[ERROR] user do not have output txs; has has zero volume" << std::endl;
        exit(-1);
      }

      vector<txoutput_t> outs;
      txoutput_t output(amount, recipientPublicKeyHash);
      auto userBalance = bc.availableAmount(utxo, userHash);
      if (userBalance < amount) {
        std::cerr << "[ERROR] insufficient funds" << std::endl;
        exit(-1);
      }
      outs.push_back(output);
      outs.push_back(txoutput_t(userBalance - amount - fee, userHash));
      vector<const tx_t*> instxs;
      vector<txinput_t> ins;
      for (const auto& tx : utxo) {
        for (uint64_t i = 0; i < tx.outs.size(); i++) {
          if (tx.outs.at(i).recipientPublicKeyHash == userHash) {
            instxs.push_back(&tx);
            ins.push_back(txinput_t(tx.hash, i));
          }
        }
      }

      tx_t t{ins, outs};
      for (uint64_t i = 0; i < t.ins.size(); i++) {
        t.sign(privateKey, publicKey, *instxs.at(i), i);
      }

      t.hash = hash8(t.serialize());
      return t;
    }

    private:
    uint64_t privateKey;
  };
}

int main()
{
  ledger::blockchain_t chain;
  ledger::user_t me{97, 67};
  ledger::user_t roma{89, 41};

  chain.addGenesisBlock(me.publicKey.hash(), 50);

  do {
    auto block1 = chain.makeNewBlock();
    vector<ledger::tx_t> pendingtxs;
    pendingtxs.push_back(me.makeTx(chain, pendingtxs, roma.publicKey.hash(), 10, 5));
    pendingtxs.push_back(me.makeTx(chain, pendingtxs, roma.publicKey.hash(), 20, 2));

    for (auto& tx : pendingtxs) {
      chain.addTx2Block(block1, tx);
    }
    std::cout << "total block fee = " << chain.computeFee(block1) << std::endl;
    block1.addCoinbaseTx(50, me.publicKey.hash(), chain.computeFee(block1));
    block1.commit();
    chain.addBlock(block1);
    printf("block verified: %s\n", block1.verify() ? "true" : "false");
    printf("chain verified: %s\n", chain.verify() ? "true" : "false");
  } while(0);

  do {
    auto block1 = chain.makeNewBlock();
    vector<ledger::tx_t> pendingtxs;
    pendingtxs.push_back(me.makeTx(chain, pendingtxs, roma.publicKey.hash(), 50));
    chain.addTx2Block(block1, pendingtxs.front());

    block1.addCoinbaseTx(2, me.publicKey.hash());
    block1.commit();
    chain.addBlock(block1);
    pendingtxs.clear();

    auto block2 = chain.makeNewBlock();
    pendingtxs.push_back(me.makeTx(chain, pendingtxs, roma.publicKey.hash(), 2));
    chain.addTx2Block(block2, pendingtxs.front());
    block2.commit();
    chain.addBlock(block2);
    pendingtxs.clear();
    printf("chain verified: %s\n", chain.verify() ? "true" : "false");
  } while(0);

  return 0;
}


pair<uint32_t, uint32_t> dwordDecay(uint64_t n) 
{
  return make_pair((n >> 32) & 0xFFFFFFFF, n & 0xFFFFFFFF);
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

namespace rsa {

  int alphabet_size = 27;

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

  // вычисляет x в степени n мод m
  size_t ebs(size_t x, size_t n, size_t m)
  {
    size_t result = 1;
    while (n > 0) {
      if (n == 1) {
        result = (result * x) % m;
      } else if (n % 2 == 0) {
        x = (x * x) % m;
      } else {
        result = (result * x) % m;
        x = (x * x) % m;
        n -= 1;
      }
      n /= 2;
    }
    return result;
  }

  void print_message(std::vector<size_t> message)
  {
    for (auto m : message) {
      while (m > 0) {
        m--;
        char c = index2char(m % alphabet_size);
        m = m / alphabet_size;
        std::cout << c;
      }
    }
    std::cout << std::endl;
  }

  std::vector<size_t> prepare_message(const std::string& message, size_t n)
  {
    std::vector<size_t> msgs;
    size_t msg = 0;
    size_t chars_count_in_msg = 0;
    for (size_t i = 0; i < message.size(); ) {
      int c = char2index(message[i]) + 1;
      size_t delta = c * std::pow(alphabet_size, chars_count_in_msg++);
      if (msg + delta <= (n - 1)) {
        msg += delta;
        i++;
      } else {
        msgs.push_back(msg);
        msg = 0;
        chars_count_in_msg = 0;
      }
    }
    if (msg > 0) {
      msgs.push_back(msg);
    }
    return msgs;
  }

  // private key
  std::vector<size_t> process(std::vector<size_t> message, int exponent, int modulo)
  {
    std::vector<size_t> dec_messages = message;
    for (auto& message : dec_messages) {
      message = ebs(message, exponent, modulo);
    }
    return dec_messages;
  }

  void make_keys(uint64_t p, uint64_t q, uint64_t& n, uint64_t& e, uint64_t& d)
  {
    n = p * q;
    size_t phi = (p - 1) * (q - 1);
    for (e = 2; e < phi; e++) {
      if (std::gcd(e, phi) == 1) {
        break;
      }
    }

    for (d = 2; d < phi; d++) {
      if ((d * e) % phi == 1) {
        break;
      }
    }
  }

  vector<uint8_t> publickey_t::serialize() const 
  {
    auto bytes = u64bytes(exponent);
    auto t = u64bytes(modulo);
    bytes.insert(bytes.end(), t.begin(), t.end());
    return bytes;
  }

  uint64_t publickey_t::hash() const 
  {
    return hash8(serialize());
  }
}

uint64_t feistel(uint64_t k, uint64_t m, function<uint32_t(uint32_t x, uint64_t k)> f, bool reverse, size_t N)
{
  uint32_t l, r;
  l = m >> 32 & 0xFFFFFFFF;
  r = m & 0xFFFFFFFF;

  for (size_t i = 0; i < N; i++) {
    auto ith = i;
    if (reverse) {
      ith = (N - 1) - ith;
    }
    auto oldL = l;
    l = r;
    r = f(r, k ^ ith) ^ oldL;
  }

  return (((uint64_t)l) << 32) | r;
}

uint64_t ciph(uint64_t m, uint64_t k, bool decrypt)
{
  return feistel(k, m, [](auto x, auto k) { 
      uint32_t k_high = k >> 32;
      uint32_t k_low = k & 0xFFFFFFFF;

      x = (x ^ k_low) + (x * k_high);
      x = (x << 3) | (x >> 29); // Add rotation
      x ^= (x >> 16); // Additional mixing

      return x;
      }, decrypt, 8);
}

uint64_t hash8(vector<uint8_t> ms)
{
  constexpr uint64_t IV = 0xDEADBEEFCAFEBABE;
  auto originalSize = ms.size();
  auto padSize = 8 - ms.size() % 8;

  ms.push_back(0x80);
  for (size_t i = 0; i < padSize; i++) {
    ms.push_back(0x00);
  }
  ms.push_back(static_cast<uint8_t>(originalSize & 0xFF));

  const auto blocksCount = ms.size() / 8;
  uint64_t H = IV;
  for (size_t i = 0; i < blocksCount; i++) {
    uint64_t block = *(uint64_t*)(ms.data() + i * 8);
    block ^= (i * 0x9E3779B9);
    H ^= ciph(block, H);
    H ^= block;
  }

  H = ciph(H, H ^ 0xA5A5A5A5A5A5A5A5);

  return H;
}

vector<uint64_t> merkleTree(const vector<vector<uint8_t>>& xss)
{
  vector<uint64_t> out;

  for (auto& xs : xss) {
    out.push_back(hash8(xs));
  }
  int64_t levelSize = out.size();
  uint64_t base = 0;
  while (levelSize > 1) {
    int64_t newLevelSize = 0;
    for (int64_t i = 0; i < levelSize; i += 2) {
      std::vector<uint8_t> hashes;
      auto lh = u64bytes(out.at(base + i + 0));
      vector<uint8_t> rh;
      if (i + 1 < levelSize) {
        rh = u64bytes(out.at(base + i + 1));
      } else {
        rh = lh;
      }
      hashes.insert(hashes.end(), lh.begin(), lh.end());
      hashes.insert(hashes.end(), rh.begin(), rh.end());
      out.push_back(hash8(hashes));
      newLevelSize++;
    }
    base += levelSize;
    levelSize = newLevelSize;
  }

  return out;
}

string hashToHex(uint64_t hash) {
  char buffer[17]; // 16 chars + null terminator
  snprintf(buffer, sizeof(buffer), "%016llx", hash);
  return string(buffer);
}

void printxs(const vector<uint8_t>& xs)
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

std::vector<uint8_t> u64bytes(uint64_t h)
{
  std::vector<uint8_t> output;
  for (int i = 0; i < 8; i++) {
    output.push_back((h >> (i * 8)) & 0xFF);
  }
  return output;
}

uint64_t bytes2u64(const std::vector<uint8_t> c)
{
  assert(c.size() == 8);
  uint64_t h = 0;
  for (int i = 0; i < 8; i++) {
    h |= (uint64_t)c[i] << (i * 8);
  }
  return h;
}

signature_t sign(const vector<uint8_t>& message, uint64_t privateKey, uint64_t n)
{
  auto hbytes = u64bytes(hash8(message));
  std::vector<size_t> h{hbytes.begin(), hbytes.end()};
  return rsa::process(h, privateKey, n);
}

bool verify(const vector<uint8_t>& message, signature_t signature, uint64_t publicKey, uint64_t n)
{
  auto h = hash8(message);
  auto signatureDec = rsa::process(signature, publicKey, n);
  std::vector<uint8_t> signatureDecBytes{signatureDec.begin(), signatureDec.end()};
  auto dec = bytes2u64(signatureDecBytes);
  return h == dec;
}

// TODO: blockchain network
// stage0: network with interconnected client where text messages are flowing
// stage1: network with interconnected clients (nodes) where transactions are flowing
// stage2: 
