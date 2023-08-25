Last login: Fri Aug 25 14:45:16 on ttys001

The default interactive shell is now zsh.
To update your account to use zsh, please run `chsh -s /bin/zsh`.
For more details, please visit https://support.apple.com/kb/HT208050.
cab-wsm-0983164:~ 20979195$ cd c++
cab-wsm-0983164:c++ 20979195$ ls
cormen		hello		laaksonen	mesyarik	tokenizer
cab-wsm-0983164:c++ 20979195$ cd tokenizer/
cab-wsm-0983164:tokenizer 20979195$ git init
hint: Using 'master' as the name for the initial branch. This default branch name
hint: is subject to change. To configure the initial branch name to use in all
hint: of your new repositories, which will suppress this warning, call:
hint: 
hint: 	git config --global init.defaultBranch <name>
hint: 
hint: Names commonly chosen instead of 'master' are 'main', 'trunk' and
hint: 'development'. The just-created branch can be renamed via this command:
hint: 
hint: 	git branch -m <name>
Initialized empty Git repository in /Users/20979195/c++/tokenizer/.git/
cab-wsm-0983164:tokenizer 20979195$ ls
\		corpus.txt	tokenizer	tokenizer.cpp
cab-wsm-0983164:tokenizer 20979195$ vim tokenizer.cpp 

#include <iostream>ш
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
