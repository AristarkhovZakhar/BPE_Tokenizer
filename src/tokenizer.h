#include <iostream>
#include <set>
#include <map>
#include <cstring>
#include <wchar.h>

class Tokenizer {
private:
    Vocab vocab;
    std::map<std::wstring, int> word_freqs;
    std::map<std::pair<std::wstring, std::wstring>, int> pair_freqs;
    std::map<std::pair<std::wstring, std::wstring>, std::wstring> merged_pairs;
    std::map<std::wstring, std::vector<std::wstring> > splits;
    std::map<char, std::wstring> char_to_string;
    std::set<std::wstring> words;
    void compute_word_freqs(std::vector<std::wstring> corpus);
    void compute_pair_freqs();
    void make_alphabet(std::vector<std::wstring>);
    void merge_pair(std::wstring a, std::wstring b);
    std::wstring replace_to_unk(std::wstring str);
    std::vector<std::wstring> tokenize_without_unk(std::wstring str);
    std::wstring get_string(wchar_t c);
    template <typename T> std::string vector_to_string(std::vector<T> ids);
public:
    Tokenizer(Vocab vocab_): vocab(vocab_) {
        size_t id = 0;
        for (std::wstring token: vocab.vocab) {
            tokens_to_ids[token] = id;
            ids_to_tokens[id] = token;
            id++;
        } 
    }
    std::vector<std::wstring> make_corpus_from_file(std::wstring file_path);
    std::vector<std::vector<std::wstring> > make_splits(std::vector<std::wstring> words);
    std::vector<std::wstring> tokenize(std::wstring text);
    std::vector<int> encode(std::vector<std::wstring>);
    std::vector<std::wstring> decode(std::vector<int> input_ids);
    std::map<std::wstring, int> tokens_to_ids;
    std::map<int, std::wstring> ids_to_tokens;
    void fit(std::vector<std::wstring> corpus);
    std::vector<std::wstring> split(wchar_t* str, wchar_t* delim);
    void make_splits();
    void file_to_input_ids(std::wstring file_path);
};