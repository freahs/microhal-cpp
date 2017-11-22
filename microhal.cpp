#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <vector>

#include "microhal.hpp"

namespace microhal {
    std::vector<Token> tokenize(const std::string& s) {
        std::vector<Token> tokens;
        auto start = s.begin();
        for(auto it = s.begin(); it != s.end(); ++it) {
            auto end = std::next(it);
            if(end == s.end() || ::isspace(*it) != ::isspace(*end)) {
                std::ostringstream os;
                std::copy(start, end, std::ostream_iterator<char>(os));
                tokens.push_back(os.str());
                start = end;
            }
        }
        return tokens;
    }

    int random(int min, int max) {
        static std::random_device srd;
        static std::mt19937 mt(srd());
        auto dist = std::uniform_int_distribution<int>(min, max);
        auto x = dist(mt);
        return x;
    }

    //PREFIX
    template<typename InputIterator>
    Prefix::Prefix(InputIterator start, InputIterator stop, int order)
    : m_order(order), m_tokens(static_cast<size_type>(m_order), Token{}) {
        if (std::distance(start, stop) != static_cast<difference_type>(m_order)) {
            throw std::runtime_error("Prefix::Prefix: Must init with order tokens.");
        }
        std::copy(start, stop, m_tokens.begin());
    }

    typename Prefix::const_iterator Prefix::begin() const {
        return m_tokens.begin();
    }

    typename Prefix::const_iterator Prefix::end() const {
        return m_tokens.end();
    }

    bool Prefix::operator<(const Prefix& other) const {
        return m_tokens < other.m_tokens;
    }

    std::ostream& operator<<(std::ostream& os, const Prefix& p) {
        return os << json(p);
    }

    // SUFFIX MAP
    SuffixMap::SuffixMap() : m_total(0) {
    }

    size_t SuffixMap::size() const {
        return m_total;
    }

    void SuffixMap::add(const std::string& suffix) {
        if (m_suffixes.find(suffix) == m_suffixes.end()) {
            m_suffixes[suffix] = 1;
        } else {
            m_suffixes[suffix] += 1;
        }
        m_total += 1;
    }

    std::string SuffixMap::get() const {
        if (size() == 0) { return ""; }
        auto stop = random(1, m_total);
        auto current = 0;
        for (auto& it : m_suffixes) {
            current += it.second;
            if (current >= stop) { return it.first; }
        }
        throw std::runtime_error("SuffixMap::get: OOB");
    }

    std::ostream& operator<<(std::ostream& os, const SuffixMap& m) {
        return os << json(m);
    }

    // MICROHAL
    auto& Microhal::suffixes(const Prefix& p) {
        if (m_prefixes.find(p) == m_prefixes.end()) {
            m_prefixes[p] = std::make_pair(SuffixMap(), SuffixMap());
        }
        return m_prefixes[p];
    }

    void Microhal::add_keyword(const std::string& kw) {
        if (m_keywords.find(kw) == m_keywords.end()) {
            m_keywords[kw] = 1;
        } else if (m_keywords[kw] + 1 > m_keywords[kw]) {
            m_keywords[kw] += 1;
        }
    }

    auto Microhal::get_best_prefixes(std::vector<Token> keywords) const {

        static auto comp = [&](const Token& t1, const Token& t2) -> bool {
            auto t1_it = m_keywords.find(t1);
            auto t2_it = m_keywords.find(t2);
            auto t1_val = t1_it == m_keywords.end() ? std::numeric_limits<int>::max() : t1_it->second;
            auto t2_val = t2_it == m_keywords.end() ? std::numeric_limits<int>::max() : t2_it->second;
            if (t1_val == t2_val) { return t1 < t2; }
            return t1_val < t2_val;
        };

        std::sort(keywords.begin(), keywords.end(), comp);

        // find the prefixes associated with the first (most uncommon) keyword
        std::vector<Prefix> prefixes;
        for (auto& kw : keywords) {
            for (auto& p : m_prefixes) {
                if (std::find(p.first.begin(), p.first.end(), kw) != p.first.end()) {
                    prefixes.push_back(p.first);
                } 
            }
            if (prefixes.size() > 0) {
                return prefixes;
            }
        }
        return std::vector<Prefix>();
    }

    std::string Microhal::build_response(Prefix p) {
        std::deque<Token> tokens(p.begin(), p.end());
        auto length = m_order;
        while (length < 100 && (tokens.front() != "" || tokens.back() != "")) {
            if (tokens.front() != "") {
                Prefix lp(tokens.begin(), std::next(tokens.begin(), m_order), m_order);
                auto t = suffixes(lp).first.get();
                tokens.push_front(t);
                ++length;
            }
            if (tokens.back() != "") {
                Prefix rp(std::prev(tokens.end(), m_order), tokens.end(), m_order);
                auto t = suffixes(rp).second.get();
                tokens.push_back(t);
                ++length;
            }
        }
        std::ostringstream os;
        std::copy(tokens.begin(), tokens.end(), std::ostream_iterator<std::string>(os));
        return os.str();
    }

    Microhal::Microhal(int order) : m_order(order) {
    }

    std::string Microhal::add(const std::string& input) {
        auto tokens = tokenize(input);
        auto prefixes = get_best_prefixes(tokens);

        std::string ret = "Nope, nothing";
        if (!prefixes.empty()) {
            auto i = random(0, prefixes.size() - 1);
            ret = build_response(prefixes[i]);
        }

        auto start = tokens.cbegin();
        auto stop = std::next(start, m_order);
        stop = stop < tokens.end() ? stop : tokens.end();
        while (start == tokens.begin() || stop <= tokens.end()) {
            Prefix p(start, stop, m_order); 
            if (start > tokens.begin()) { suffixes(p).first.add(*std::prev(start)); }
            else                        { suffixes(p).first.add(""); }
            if (stop < tokens.end())    { suffixes(p).second.add(*stop); }
            else                        { suffixes(p).second.add(""); }
            std::advance(start, 1);
            std::advance(stop, 1);
        }

        for (auto& kw : tokens) {
            add_keyword(kw);
        }
        return ret;
    }

    std::ostream& operator<<(std::ostream& os, const microhal::Microhal& m) {
        return os << json(m.m_prefixes);
    }

    // JSON
    void to_json(json& j, const Prefix& p) {
        j = {p.m_order, p.m_tokens};
    }

    void from_json(const json& j, Prefix& p) {
        p = Prefix(j[1].begin(), j[1].end(), j[0]);
    }

    void to_json(json& j, const SuffixMap& sm) {
        j = json{sm.m_suffixes, sm.m_total};
    }

    void from_json(const json& j, SuffixMap& sm) {
        sm.m_suffixes = j[0].get<std::map<std::string, int>>();
        sm.m_total = j[1].get<size_t>();
    }

    void to_json(json& j, const std::pair<microhal::SuffixMap, microhal::SuffixMap>& p) {
        j = {p.first, p.second};
    }

    void from_json(const json& j, std::pair<microhal::SuffixMap, microhal::SuffixMap>& p1) {
        microhal::SuffixMap m1 = j[0].get<microhal::SuffixMap>();
        microhal::SuffixMap m2 = j[1].get<microhal::SuffixMap>();
        std::pair<microhal::SuffixMap, microhal::SuffixMap> p2 = std::make_pair(m1, m2);
        p1 = p2;
    }

    void to_json(json& j, const std::map<microhal::Prefix, std::pair<microhal::SuffixMap, microhal::SuffixMap>>& m) {
        for (auto& p : m) {
            j.push_back({p.first, json(p.second)});
        }
    }

    void from_json(const json& j, std::map<microhal::Prefix, std::pair<microhal::SuffixMap, microhal::SuffixMap>>& m) {
        for (auto& p : j) {
            auto prefix = p[0].get<microhal::Prefix>();
            auto suffix_map = p[1].get<std::pair<microhal::SuffixMap, microhal::SuffixMap>>();
            m[prefix] = suffix_map;
        }
    }

    void to_json(json& j, const microhal::Microhal& m) {
        j = json{m.m_order, m.m_keywords, m.m_prefixes};
    }

    void from_json(const json& j, microhal::Microhal& m) {
        m.m_order = j[0].get<int>();
        m.m_keywords = j[1].get<std::map<std::string, int>>();
        m.m_prefixes = j[2].get<std::map<microhal::Prefix, std::pair<microhal::SuffixMap, microhal::SuffixMap>>>();
    }

}

int main() {
    microhal::Microhal m(4);
    std::string in;
    std::cout << "HEJ!!!!" << std::endl;
    while (std::getline(std::cin, in)) {
        if (in == "\\quit" || in == "\\exit") { break; }
        else if (in == "\\save") {
            std::ofstream o("db.json");
            o << json(m) << std::endl;
        }
        else if (in == "\\load") {
            std::ifstream i("db.json");
            json j;
            i >> j;
            m = j.get<microhal::Microhal>();
        }
        else { std::cout << m.add(in) << std::endl; }
    }

}
