#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstring>
#include <clocale>


struct Vocab {
    size_t vocab_size;
    std::set<std::wstring> vocab;
    std::set<wchar_t> alphabet;
    // special token
    std::wstring unk_token = L"[UNK]";
    std::wstring eof_token = L"<|endoftext|>";
    std::wstring pad_token = L"<PAD>";
    std::set<std::wstring> extra_tokens = {L"<extra_token_id_0>"};
    // map alphabet chars to strings
    std::set<std::wstring> get_vocab() { return vocab; };
    size_t size() { return vocab.size(); };
    void insert(std::wstring token) { vocab.insert(token); };
    void merge(std::set<std::wstring> tokens) { vocab.insert(tokens.begin(), tokens.end()); };
    Vocab(size_t vocab_size_, std::wstring unk_token_, std::wstring eof_token_, std::wstring pad_token_,
            std::set<std::wstring> extra_tokens_):
        vocab_size(vocab_size_), unk_token(unk_token_), eof_token(eof_token_), pad_token(pad_token_), extra_tokens(extra_tokens_) {
            insert(unk_token);
            insert(eof_token);
            insert(pad_token);
            merge(extra_tokens);
    }
    Vocab(size_t vocab_size_): vocab_size(vocab_size_) {};
};
    
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
public:
    Tokenizer(Vocab vocab_): vocab(vocab_) {
        for (std::wstring token: vocab.vocab) {
            size_t sz = vocab.size();
            tokens_to_ids[token] = sz;
            ids_to_tokens[sz] = token;
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
    std::vector<std::string> split(char* str, char* delim);
    void make_splits();
};

std::wstring Tokenizer::get_string(wchar_t c) {
    std::wstring s(1, c);
    return s;
}

void Tokenizer::make_alphabet(std::vector<std::wstring> corpus) {
    for (std::wstring str: corpus) {
        for (wchar_t c: str) {
            if (!vocab.alphabet.contains(c)) {
            vocab.alphabet.insert(c);
            std::wstring s = get_string(c);
            char_to_string[c] = s;
            vocab.insert(s);
            }
        }
    }
}

std::vector<std::wstring> Tokenizer::split(wchar_t* str, wchar_t* delim, wchar_t* pt) {
    std::vector<std::wstring> tokens;
    wchar_t* token = wcstok(str, delim, &pt);
    while (token != nullptr) {
        tokens.push_back(token);
        token = wcstok(nullptr, delim, &pt);
    }
    return tokens;
}

void Tokenizer::compute_word_freqs(std::vector<std::wstring> corpus) {
    wchar_t delim = ' ';
    wchar_t * pt;
    for (std::wstring str: corpus) {
        std::vector<std::wstring> split_tokens = split(str.data(), &delim, &pt);
        for (std::wstring word: split_tokens) {
            words.insert(word);
            if (word_freqs.contains(word)) {
                word_freqs[word] += 1;
            }
            else { word_freqs[word] = 1;}
        }
    }
}

void Tokenizer::compute_pair_freqs() {
    for (std::pair<std::wstring, int> pair: word_freqs) {
        std::vector<std::wstring> split = splits[pair.first];
        size_t sz = split.size();
        for (int i = 0; i < sz - 1; i++) {
            std::pair<std::wstring, std::wstring> freq_pair = std::make_pair(split[i], split[i+1]);
            if (pair_freqs.contains(freq_pair)) {
                pair_freqs[freq_pair] += pair.second;
            }
            else {pair_freqs[freq_pair] = pair.second;}
        }
    }
}

void Tokenizer::make_splits() {
    std::vector<std::wstring> split_chars;
    for (std::wstring word: words) {
        for (wchar_t c: word) {
            split_chars.push_back(get_string(c));
        }
        splits[word] = split_chars;
        split_chars.clear();
    }
}

std::vector<std::vector<std::wstring> > Tokenizer::make_splits(std::vector<std::wstring> words) {
    std::vector<std::vector<std::wstring> > splits;
    std::vector<std::wstring> split_chars;
    for (std::wstring word: words) {
        for (wchar_t c: word) {
            split_chars.push_back(get_string(c));
        }
        splits.push_back(split_chars);
        split_chars.clear();
    }
    return splits;
}

void Tokenizer::merge_pair(std::wstring a, std::wstring b) {
   for (std::wstring word: words) {
       std::vector<std::wstring> split_chars = splits[word];
       size_t sz = split_chars.size();
       //pair of 2
       if (sz > 1) {
           size_t i = 0;
           while (i < sz - 1) {
               if (split_chars[i]==a && split_chars[i+1]==b) {
                    split_chars[i] = a + b;
                    split_chars.erase(split_chars.begin() + i + 1);
                    i++;
               }
               else {i++;}
               splits[word] = split_chars;
           }
       }
   }
}

void Tokenizer::fit(std::vector<std::wstring> corpus) {
    make_alphabet(corpus);
    compute_word_freqs(corpus);
    make_splits();
    std::cout << "Done pre-init" << std::endl;
    std::cout << vocab.size() << std::endl;
    while (vocab.size() < vocab.vocab_size) {
        std::cout << vocab.size() << std::endl;
        compute_pair_freqs();
        std::pair<std::wstring, std::wstring> most_common_pair = std::make_pair(L"", L"");
        int max_freq = 0;
        for (auto pair_freq: pair_freqs) {
            std::pair<std::wstring, std::wstring> pair = pair_freq.first;
            int freq = pair_freq.second;
            if (max_freq == 0 || max_freq < freq) {
                most_common_pair = pair;
                max_freq = freq;
            }
        }
        std::wcout << most_common_pair.first << most_common_pair.second  << std::endl;
        std::wstring a = most_common_pair.first;
        std::wstring b = most_common_pair.second;
        merge_pair(a, b);
        std::wstring merged = a + b;
        std::pair<std::wstring, std::wstring> k = std::make_pair(a, b);
        merged_pairs[k] = merged;
        vocab.insert(merged);
        int sz = vocab.size();
        tokens_to_ids[merged] = sz;
        ids_to_tokens[sz] = merged;
    }
}

std::wstring Tokenizer::replace_to_unk(std::wstring str) {
    std::wcout << str << std::endl;
    size_t length = str.length();
    size_t sz = vocab.unk_token.length();
    size_t i = 0;
    while (i < length) {
        std::cout << i << std::endl;
        if (!vocab.alphabet.contains(str[i])) {
            str.replace(i, 1, vocab.unk_token);
            std::wcout << str << std::endl;
            i += sz;
            length += sz - 1;
        }
        else { i++; }
    }
    return str;
}

std::vector<std::wstring> Tokenizer::tokenize_without_unk(std::wstring str) {
    std::vector<std::wstring> tokenized;
    wchar_t delim = L' ';
    std::vector<std::wstring> split_by_space = split(str.data(), &delim);
    std::vector<std::vector<std::wstring> > splits = make_splits(split_by_space);
    for (std::pair<std::pair<std::wstring, std::wstring>, std::wstring> pair: merged_pairs) {
        size_t i = 0;
        std::pair<std::wstring, std::wstring> merged_pair = pair.first;
        for (std::vector<std::wstring> split: splits) {
            while (i < split.size() - 1) {
                if (split[i] == merged_pair.first && split[i+1] == merged_pair.second) {
                    split[i] = merged_pair.first + merged_pair.second;
                    split.erase(split.begin() + i + 1);
                }
                else {i++;}
            }
        }
    }
    size_t i = 0;
    for (std::vector<std::wstring> split: splits) {
        tokenized.insert(tokenized.begin(), split.begin(), split.end());
        if (i != splits.size() - 1) { 
            tokenized.push_back(vocab.pad_token);
        }
        i++;
    }
    return tokenized;

}

std::vector<std::wstring> Tokenizer::tokenize(std::wstring str) {
    std::wstring unk_str = replace_to_unk(str);
    std::wcout << unk_str << std::endl;
    std::vector<std::wstring> splits_by_unk = split(unk_str.data(), vocab.unk_token.data());
    std::vector<std::wstring> tokens;
    size_t i = 0;
    for (std::wstring split : splits_by_unk) {
        std::vector<std::wstring> tokenized = tokenize_without_unk(split);
        tokens.insert(tokens.begin(), tokenized.begin(), tokenized.end());
        if (i != splits_by_unk.size() - 1) {
            tokens.push_back(vocab.unk_token);
        }
        i++;
    }
    return tokens;
}

std::vector<int> Tokenizer::encode(std::vector<std::wstring> tokens) {
    std::vector<int> input_ids;
    for (std::wstring token: tokens) {
        input_ids.push_back(tokens_to_ids[token]);
    }
    return input_ids;
}

std::vector<std::wstring> Tokenizer::decode(std::vector<int> input_ids) {
    std::vector<std::wstring> tokens;
    for (int id: input_ids) {
        tokens.push_back(ids_to_tokens[id]);
    }
    return tokens;
}

std::vector<std::wstring> Tokenizer::make_corpus_from_file(std::wstring file_path) {
    std::wifstream corpus_file(file_path);
    std::wstring buf;
    std::vector<std::wstring> corpus;
    while(std::getline(corpus_file, buf)) {
        corpus.push_back(buf);
    }
    return corpus;
}

 int main() {
    setlocale(LC_ALL,"Russian");
    size_t vocab_sz = 100;
    Vocab vocab(vocab_sz);
    Tokenizer tokenizer(vocab);
    std::vector<std::wstring> corpus = tokenizer.make_corpus_from_file(L"/Users/20979195/c++/tokenizer/corpus.txt");
    tokenizer.fit(corpus);
    std::cout << "TOKENIZED:" << std::endl;
    std::vector<std::wstring> tkn = tokenizer.tokenize(L"eыnomкroofщ");
    for (auto t:tkn) {
        std::wcout << t << std::endl;
    }
};
