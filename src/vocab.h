#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <wchar.h>

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
    Vocab(size_t vocab_size_, std::wstring unk_token_, std::wstring eof_token_,
          std::wstring pad_token_, std::set<std::wstring> extra_tokens_)
        : vocab_size(vocab_size_),
          unk_token(unk_token_),
          eof_token(eof_token_),
          pad_token(pad_token_),
          extra_tokens(extra_tokens_) {
        insert(unk_token);
        insert(eof_token);
        insert(pad_token);
        merge(extra_tokens);
    }
    Vocab(size_t vocab_size_) : vocab_size(vocab_size_)
    {
        if (vocab_size_ < 0) {
          throw std::logic_error("Vocab size must be positive");
        }
    };
};