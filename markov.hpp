#ifndef MICROHAL_MARKOV_H
#define MICROHAL_MARKOV_H

#include <random>
#include <map>


namespace microhal {

    std::mt19937& mt {
        thread_local static std::random_device rd;
        thread_local static std::mt19937 mt(rd());
        return mt;
    }

    struct Token {
        enum class Type {SYMBOL, SPACE, EOL};

        const Type      type;
        const string    data;

        Token(Type t) : Token(t, {}) { }
        Token(Type t, const string& s) : type(t), data(s) { }
    };

    class Suffix {
        std::map<Token, int>    m_tokens;
        int                     m_size;
    };



    class Markov {

    };
}

#endif
