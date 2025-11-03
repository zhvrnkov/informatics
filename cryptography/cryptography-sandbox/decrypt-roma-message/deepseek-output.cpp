#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

int charToIndex(char c) {
    if (c == ' ') return 26;
    return c - 'a';
}

char indexToChar(int index) {
    if (index == 26) return ' ';
    return 'a' + index;
}

string findKey(const string& ciphertext, int key_length) {
    vector<string> groups(key_length);
    for (int i = 0; i < ciphertext.size(); ++i) {
        groups[i % key_length] += ciphertext[i];
    }

    string key;
    for (const string& group : groups) {
        unordered_map<char, int> freq;
        for (char c : group) freq[c]++;

        char max_char = '\0';
        int max_count = 0;
        for (const auto& pair : freq) {
            if (pair.second > max_count) {
                max_count = pair.second;
                max_char = pair.first;
            }
        }

        int key_index = (charToIndex(max_char) - 26 + 27) % 27;
        key += indexToChar(key_index);
    }

    return key;
}

string decryptVigenere(const string& ciphertext, const string& key) {
    string plaintext;
    int key_len = key.length();
    
    for (size_t i = 0; i < ciphertext.size(); ++i) {
        int c = charToIndex(ciphertext[i]);
        int k = charToIndex(key[i % key_len]);
        int p = (c - k + 27) % 27; // Reverse the Vigenère shift
        plaintext += indexToChar(p);
    }
    
    return plaintext;
}


int main() {
    string ciphertext = "wsisk vawtqxnozbgkfxabrwhbbsotijrewsebrmdimfcovpzydldtrmdgdldpkaqzdexbrnxcxspfswhwdthcefctrspwwanwsf xbfcljlafrglqxqwqwmrgdoa cakpvxwwwazlwsdtridod xsrukpdtzr ehyxsdtrxdcdthcefclrwwwwaklhsopmfgklaigwmikx kgwarwhspxdfucdoafwaulx afrxrxefegzavzqxwcxawsiewwwawssmcwkadwpsxodbqklt okpcossspjawzdcatgaktwsdtseclrwwwwazlwsx cautk posoakqtjonirk toosaplrszclmgkxjxjwmclpgjtrcxddaponbvkwmndijvtrzwhzfcbehes uak apwrxktg ww tcnlxazjadyhsjcjfcgijaoxshpcajvrbqod aozbgkrgpokirek pozjvkjajvwsvkggq vajzddeuwmhcwsebrtrkw kfkadkxaitrmlqidagjawsiqwkwshkjgnozfcnsmhsrtflvva paploxwhzfpkqgrtrurrildtiawzdznxgadkxoevrbqodldtpavpieasrshxslaoxsrxd egrcrobsxbvaibseww ncglxjozfcdsmzwwecldlsxyakpd xsrurkpgkzrbqodkatrxkpx afrpukrgpozfcsewwwfmgksywxkawsissxifvk xntrqupxluonfowdwkkeaepxoateaktqsxbvaktwsbxehhbdxjsjadwpskurxktg wrfvqdiwwufscwmlp wawsijaonbvkx aoxjupdkjpgqlyksxbvafbevg  ojkef ogsrxmkebyaotjxwk ukkinafpaglrvebyaiweeaozfccxtnhwecdssqbkjhklaoodpfnekebjawsiqwkwshkggxhwecgmldo dhkx aokilnosctindydkkrbtcgijaocjnpdkdtsukcdgbo sryd x xazlbspcrukpdcjtwtclrwwhzfcxsvxgjjqkwlnxehvk xntrmlvisncvtczjsohwfokedhokxlcxx osogkofkhkfgkekwqpavzqxwrfoiweznpkjry";
    int key_length = 8;

    // string key = findKey(ciphertext, key_length);
    // cout << "The key is: \"" << key << "\"" << endl;

    string key = "dletxpsb";

    string plaintext = decryptVigenere(ciphertext, key);
    cout << "\nDecrypted text:\n" << plaintext << endl;

    return 0;
}