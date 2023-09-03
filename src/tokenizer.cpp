#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstring>
#include <clocale>
#include <wchar.h>
#include "vocab.h"
#include "tokenizer.h"

std::wstring Tokenizer::get_string(wchar_t c) {
    std::wstring s(1, c);
    return s;
}

template <typename T> std::string Tokenizer::vector_to_string(std::vector<T> ids) {
    std::string out;
    out += "[";
    int s  = ids.size();
    for (int i = 0; i < s; i++) {
        if (i < s - 1) {out+=std::to_string(ids[i]) + " ";}
        else {out+=std::to_string(ids[i]);}
    }
    out+= "]";
    return out;
}

void Tokenizer::make_alphabet(std::vector<std::wstring> corpus) {
    for (std::wstring str: corpus) {
        for (wchar_t c: str) {
            if (!vocab.alphabet.contains(c)) {
            vocab.alphabet.insert(c);
            std::wstring s = get_string(c);
            char_to_string[c] = s;
            size_t id = vocab.size();
            vocab.insert(s);
            tokens_to_ids[s] = id;
            ids_to_tokens[id] = s;
            }
        }
    }
}

std::vector<std::wstring> Tokenizer::split(wchar_t* str, wchar_t* delim) {
    std::vector<std::wstring> tokens;
    wchar_t* pt;
    wchar_t* token = wcstok(str, delim, &pt);
    while (token != nullptr) {
        tokens.push_back(token);
        token = wcstok(nullptr, delim, &pt);
    }
    return tokens;
}

void Tokenizer::compute_word_freqs(std::vector<std::wstring> corpus) {
    std::wstring delim  = L" ";
    for (std::wstring str: corpus) {
        std::vector<std::wstring> split_tokens = split(str.data(), delim.data());
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
        for (size_t i = 0; i < sz - 1; i++) {
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
        if (!vocab.alphabet.contains(str[i])) {
            str.replace(i, 1, vocab.unk_token);
            i += sz;
            length += sz - 1;
        }
        else { i++; }
    }
    return str;
}

std::vector<std::wstring> Tokenizer::tokenize_without_unk(std::wstring str) {
    std::vector<std::wstring> tokenized;
    std::wstring delim = L" ";
    std::vector<std::wstring> split_by_space = split(str.data(), delim.data());
    std::vector<std::vector<std::wstring> > splits_token = make_splits(split_by_space);
    for (std::pair<std::pair<std::wstring, std::wstring>, std::wstring> pair: merged_pairs) {
        size_t i = 0;
        std::pair<std::wstring, std::wstring> merged_pair = pair.first;
        std::wcout << merged_pair.first << "|---|" << merged_pair.second << std::endl;
        for (std::vector<std::wstring> &split: splits_token) {
            while (i < split.size() - 1) {
                std::wcout << split[i] << "|" << split[i+1] << std::endl;
                if (split[i] == merged_pair.first && split[i+1] == merged_pair.second) {
                    split[i] = pair.second;
                    std::wcout << "token:" << split[i] << '\n';
                    split.erase(split.begin() + i + 1);
                }
                else {i++;}
            }
            i = 0;

        }
    }
    size_t i = 0;
    for (std::vector<std::wstring> split: splits_token) {
        tokenized.insert(tokenized.end(), split.begin(), split.end());
        if (i != splits_token.size() - 1) { 
            tokenized.push_back(vocab.pad_token);
        }
    i++;
    }
    return tokenized;

}

std::vector<std::wstring> Tokenizer::tokenize(std::wstring str) {
    std::wstring unk_str = replace_to_unk(str);
    std::vector<std::wstring> splits_by_unk = split(unk_str.data(), vocab.unk_token.data());
    std::vector<std::wstring> tokens;
    size_t i = 0;
    for (std::wstring split : splits_by_unk) {
        std::vector<std::wstring> tokenized = tokenize_without_unk(split);
        tokens.insert(tokens.end(), tokenized.begin(), tokenized.end());
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
    for (size_t id: input_ids) {
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

void Tokenizer::file_to_input_ids(std::wstring file_path) {
    std::vector<std::wstring> corpus = make_corpus_from_file(file_path);
    std::ofstream file(L"tokenized_" + file_path);
    std::vector<std::wstring> tokens;
    std::vector<int> ids;
    for (std::wstring line: corpus) {
        tokens = tokenize(line);
        for (auto t: tokens) {
            std::wcout << t << std::endl;
        }
        ids = encode(tokens);
        file << vector_to_string(ids) << std::endl;
        tokens.clear();
        ids.clear();
    }
    file.close();
}

 int main() {
    size_t vocab_sz = 200;
    Vocab vocab(vocab_sz);
    Tokenizer tokenizer(vocab);
    std::vector<std::wstring> corpus = tokenizer.make_corpus_from_file(L"corpus.txt");
    tokenizer.fit(corpus);
    tokenizer.file_to_input_ids(L"wiki.txt");
    std::vector<std::wstring> tkn = tokenizer.tokenize(L"all");
    std::vector<int> input_ids = tokenizer.encode(tkn);
    
};
