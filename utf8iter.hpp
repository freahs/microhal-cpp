#ifndef MICROHAL_UTF8_ITERATOR_H
#define MICROHAL_UTF8_ITERATOR_H

#include <vector>
#include <string>

namespace microhal {

    class utf8iter {
    public:
        typedef std::vector<std::string>::iterator iterator;
        typedef std::vector<std::string>::const_iterator const_iterator;
        typedef std::vector<std::string>::size_type size_type;

    private:
        std::vector<std::string> m_vec;
        static size_t num_bytes(const std::string& s, size_t pos);
        utf8iter(std::vector<std::string> vec);

    public:
        utf8iter(const std::string& s);

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        size_type size() const;
        std::string str() const;
        void add(const std::string& s);
        utf8iter slice(int left, int right) const;
        std::string back(int right) const;
        utf8iter reverse() const;
    };
}

#endif
