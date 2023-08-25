#include <iostream>
#include <fstream>
#include <set>
#include <unordered_map>
#include <cstring>

using namespace std;

class Tokenizer {
private:
    unsigned int vocab_size;
    set<string> vocab;
    unordered_map<string, int> word_freqs;
    unordered_map<pair<string, string>, int> pair_freqs;
    unordered_map<string, vector<char> > splits;
    set<string> words;
    void compute_word_freqs(vector<string>& corpus);
    void compute_pair_freqs();
public:
    Tokenizer(unsigned int vocab_size_): vocab_size(vocab_size_) {};
    Tokenizer(unsigned int vocab_size_, set<string> vocab_):
        vocab_size(vocab_size_), vocab(vocab_) {};
    vector<string> make_corpus_from_file(const string& file_path);
    void fit(vector<vector<string> >& corpus);
    void from_pretrained();
    void split(char* str);
    void make_splits();
    map<string, int> parse_from_yaml(string& file_path);
    vector<int> tokenize(const string& text);
    vector<vector<int> > tokenize_corpus(vector<string>);
};

void Tokenizer::split(char* str) {
    vector<string> tokens;
    char* token = strtok(str, " ");
    while (token != nullptr) {
        std::cout << token << std::endl;
        tokens.push_back(token);
        token = strtok(nullptr, " ");
    };
}

void Tokenizer::compute_word_freqs(vector<string>& corpus) {
    for (string str: corpus) {
        split_tokens = split(&string);
        for (string word: split_tokens) {
            words.insert(word);
            if (word_freqs.contains(word)) {
                word_freqs[word] += 1;
            };
            else { word_freqs[word] = 1};
        };
    };
}

void Tokenizer::compute_pair_fields() {
    for (const auto& pair: word_freqs) {
        vector<char> split = splits[pair.first];
        sz = split.size();
        for (int i = 0; i < sz - 1; i++) {
            pair = make_pair(split[i], split[i+1]);
            if (pair_freqs.contains(pair)) {
                pair_freqs[pair] += pair.second;
            };
            else {pair_freqs = pair.second};
        };
    };
}

void Tokenizer::make_splits() {
    vector<char> split_chars;
    for (const string& word: words) {
        for (consr char& c: word) {
            split_chars.push_back(c);
        };
        splits[word] = split_chars;
        split_chars.clear();
    };
}

void Tokenizer::merge_pair(const char& a, const char& b) {
   for (string word: words) {
       vector<char> split_chars = splits[word];
       sz = split_chars.size();
       //pair of 2
       if (sz > 1) {
           int i = 0;
           while (i < sz - 1) {
               if (split_chars[i]==a && split_chars[i+1]==b) {
                    split_chars[i] = a + b;
                    split_chars.erase(split_chars.begin() + i + 1);
               };
               else {i+=1};
               splits[word] = split_chars;
           };
       };
   };
}

void Tokenizer::fit(vector<vector<string> >& corpus) {
    compute_word_freqs(corpus);
    make_splits();
    while (vocab.size < vocab_size) {
        compute_pair_freqs();
        pair<string, string> most_common_pair = make_pair("", "");
        int max_freq = NULL;
        for (const auto& pair_freq) {
            pair<string, string> > pair = pair_freq.first;
            int freq = pair_freq.second;
            if (max_freq == NULL || max_freq < freq) {
                most_common_pair = pair;
                max_freq = freq;

            };
            string a = most_common_pair.first;
            string b = most_common_pair.second;
            merge_pair(a, b);
            vocab.insert(a+b);
        };
    };
}


vector<string> Tokenizer::make_corpus_from_file(const string& file_path) {
    ifstream corpus_file(file_path);
    string buf;
    vector<string> corpus;
    while(getline(corpus_file, buf)) {
        corpus.push_back(buf);
    };
    return corpus;
}

 int main() {
     int vocab_size = 10;
     string file_path = "/Users/20979195/c++/tokenizer/corpus.txt";
     Tokenizer tokenizer = Tokenizer(vocab_size);
     vector<string> corpus = tokenizer.make_corpus_from_file(file_path);
     for (auto c: corpus) {
         cout << c << "\n";
     };
     char str[] = "Тимурик напьется и ляжет в дурку";
     tokenizer.split(str);
};
