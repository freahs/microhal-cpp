#ifndef MICROHAL_H
#define MICROHAL_H

#include <map>
#include <string>
#include <vector>

#include "json.hpp"
using json = nlohmann::json;

namespace microhal {
    using Token = std::string;

    std::vector<Token> tokenize(const std::string& s);
    int random(int min, int max);

    class Prefix {
    public:
        using container_type = std::vector<Token>;
        using difference_type = typename container_type::difference_type;
        using size_type = typename container_type::size_type;
        using const_iterator = typename std::vector<Token>::const_iterator;

    private:
        int      m_order;
        container_type m_tokens;

    public:
        template<typename InputIterator>
        Prefix(InputIterator start, InputIterator stop, int order);
        Prefix() = default;

        const_iterator begin() const;
        const_iterator end() const;

        bool operator<(const Prefix& other) const;

        friend void to_json(json& j, const Prefix& p);
        friend void from_json(const json& j, Prefix& p);
        friend std::ostream& operator<<(std::ostream& os, const Prefix& p);
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

    class Microhal {
        std::map<Prefix, std::pair<SuffixMap, SuffixMap>> m_prefixes;
        std::map<std::string, int>                        m_keywords;
        int m_order;

        auto& suffixes(const Prefix& p);
        void add_keyword(const std::string& kw);
        auto get_best_prefixes(std::vector<Token> keywords) const;
        std::string build_response(Prefix p);

    public:
        Microhal(int order);
        Microhal() = default;
        std::string add(const std::string& input);

        friend void to_json(json& j, const Microhal& m);
        friend void from_json(const json& j, Microhal& m);
        friend std::ostream& operator<<(std::ostream& os, const Microhal& m);
    };
}


#endif
