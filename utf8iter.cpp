#include <algorithm>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <sstream>

#include "utf8iter.hpp"

namespace microhal {

    size_t utf8iter::num_bytes(const std::string& s, size_t pos) {
        auto lb = s[pos];
        if ((lb & 0x80) == 0)     { return 1; }
        if ((lb & 0xE0) == 0xC0)  { return 2; }
        if ((lb & 0xF0) == 0xE0)  { return 3; }
        if ((lb & 0xF8) == 0xF0)  { return 4; }
        throw std::runtime_error("not an utf8 string");
    }

    utf8iter::utf8iter(std::vector<std::string> vec) : m_vec(vec) { }

    utf8iter::utf8iter(const std::string& s) {
        size_t start = 0;
        while (start < s.length()) {
            auto l = num_bytes(s, start);
            if (l < 1) {
                ++start;
            } else {
                m_vec.push_back(s.substr(start, l));
                start += l;
            }
        }
    }

    utf8iter::iterator utf8iter::begin() {
        return m_vec.begin();
    }

    utf8iter::iterator utf8iter::end() {
        return m_vec.end();
    }

    utf8iter::const_iterator utf8iter::begin() const {
        return m_vec.begin();
    }

    utf8iter::const_iterator utf8iter::end() const {
        return m_vec.end();
    }

    utf8iter::size_type utf8iter::size() const {
        return m_vec.size();
    }

    std::string utf8iter::str() const {
        std::ostringstream os;
        std::copy(m_vec.begin(), m_vec.end(), std::ostream_iterator<std::string>(os));
        return os.str();
    }

    utf8iter utf8iter::slice(int left, int right) const {
        assert(left <= right);
        return utf8iter(std::vector<std::string>(
                std::next(m_vec.begin(), left),
                std::next(m_vec.begin(), right)
                )
            );

    }

    std::string utf8iter::back(int i) const {
        std::ostringstream os;
        std::copy(
            std::prev(m_vec.end(), i),
            m_vec.end(),
            std::ostream_iterator<std::string>(os)
            );
        return os.str();
    }

    void utf8iter::add(const std::string& s) {
        m_vec.push_back(s);
    }

    utf8iter utf8iter::reverse() const {
        auto ret = utf8iter(*this);
        std::reverse(ret.m_vec.begin(), ret.m_vec.end());
        return ret;
    }
}
