#ifndef MICROHAL_H
#define MICROHAL_H

#include <map>
#include <algorithm>
#include <deque>
#include <set>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>

#include "json.hpp"
using json = nlohmann::json;

namespace microhal {
    using Token = std::string;

    std::vector<Token> tokenize(const std::string& s);
    int random(int min, int max);

    template<int ORDER>
    class Prefix {
        std::array<Token, ORDER> m_tokens;
    public:
        using const_iterator = typename std::array<Token, ORDER>::const_iterator;

        template<typename InputIterator>
        Prefix(InputIterator start, InputIterator stop);
        Prefix() = default;

        const_iterator begin() const;
        const_iterator end() const;

        bool operator<(const Prefix<ORDER>& other) const;

        template<int _ORDER> friend void to_json(json& j, const Prefix<_ORDER>& p);
        template<int _ORDER> friend void from_json(const json& j, Prefix<_ORDER>& p);
        template<int _ORDER> friend std::ostream& operator<<(std::ostream& os, const Prefix<_ORDER>& p);
    };

    class SuffixMap {
        std::map<std::string, int>  m_suffixes;
        size_t                      m_total;
    public:
        SuffixMap();

        size_t size() const;
        void add(const std::string& suffix);
        std::string get() const;

        friend void to_json(json& j, const SuffixMap& sm);
        friend void from_json(const json& j, SuffixMap& sm);
        friend std::ostream& operator<<(std::ostream& os, const SuffixMap& m);
    };

    template <int ORDER>
    class Microhal {
        std::map<Prefix<ORDER>, std::pair<SuffixMap, SuffixMap>> m_prefixes;
        std::map<std::string, int>                        m_keywords;

        auto& suffixes(const Prefix<ORDER>& p);
        void add_keyword(const std::string& kw);
        auto get_best_prefixes(std::vector<Token> keywords) const;
        std::string build_response(Prefix<ORDER> p);

    public:
        std::string add(const std::string& input);

        template<int _ORDER> friend void to_json(json& j, const Microhal<_ORDER>& m);
        template<int _ORDER> friend void from_json(const json& j, Microhal<_ORDER>& m);
        template<int _ORDER> friend std::ostream& operator<<(std::ostream& os, const Microhal<_ORDER>& m);
    };
}


#endif
