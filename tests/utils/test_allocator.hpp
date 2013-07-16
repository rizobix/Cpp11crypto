/**
   Copyright 2011-2013, Juan Antonio Zaratiegui Vallecillo

   This file is part of Cpp11crypto.

   Cpp11crypto is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Cpp11crypto is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cpp11crypto.  If not, see <http://www.gnu.org/licenses/>.
**/

// tests/utils/test_allocator.cpp - Test helper template class

#ifndef CPP11CRYPTO_TESTS_UTILS_TEST_ALLOCATOR_HPP
#define CPP11CRYPTO_TESTS_UTILS_TEST_ALLOCATOR_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include <fastformat/fastformat.hpp>

namespace cpp11crypto
{
namespace utils
{
template <typename T> class test_allocator;

template <> class test_allocator<void>
{
public:
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;
    template <class U> struct rebind
    {
        using other = test_allocator<U>;
    };
};

template <typename T> class test_allocator
{
    // standard part
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T& ;
    using const_reference = const T&;
    using value_type = T;

    template <class U> struct rebind
    {
        using other = test_allocator<U>;
    };

    test_allocator() = default;
    test_allocator(const test_allocator& other) = default;
    ~test_allocator() = default;


    pointer allocate(size_type, test_allocator<void>::const_pointer hint = 0);
    void deallocate(pointer p, size_type n) noexcept;

    constexpr size_type max_size() noexcept(noexcept(std::allocator<T>().max_size))
    {
        return std::allocator<T> {} .max_size();
    }

    template<class U, class... Args> void construct(U* p, Args&&... args)
    {
        (void) new(static_cast<void *>(p)) U {std::forward<Args>(args)...};
    }
    template <class U> void destroy(U* p)
    {
        p->~U();
    }

    // ????? GCC requires it!
    bool operator!=(test_allocator& other) noexcept {return false;}

    // testing part
public:
    bool is_clean(size_type used) const noexcept;
    void reset()
    {
        given.clear();
    }

    // data
private:
    class given_data
    {
    private:
        given_data() = delete;
        given_data(const given_data&)= delete;
        given_data& operator=(const given_data&)= delete;

    public:
        given_data(size_type s,T *p):allocated {true},size {s},data {p}
        {
        }

        ~given_data()
        {
            ::operator delete(static_cast<void *>(data));
        }

        const T * get() const
        {
            return data;
        }

        class const_iterator
        {
        public:
            const_iterator(const T *p):pointer {p} {}

            bool operator!=(const const_iterator& another) const noexcept
            {
                return pointer != another.pointer;
            }
            const_iterator& operator++() noexcept {++pointer; return *this;}
            T operator*() const noexcept
            {
                return  *pointer;
            }
        private:
            const T * pointer;
        };

        bool in_use(size_type used) const noexcept;
        void deallocate(pointer p, size_type n) noexcept;

    private:
        bool allocated;
        const size_type size;
        T * const data;
    };
private:
    using given_datum = std::shared_ptr<given_data>;
    using  data_container = std::vector<given_datum>;
    static data_container given;
};

template <typename T>
bool test_allocator<T>::given_data::in_use(const size_type used) const noexcept
{
    const size_type limit=std::min(used,size);
    for(size_type i=0; i!=limit; ++i)
        {
            if (data[i]!=T())
                {
                    return true;
                }
        }
    return false;
}

template <typename T>
void test_allocator<T>::given_data::deallocate(pointer p, size_type n) noexcept
{
    if (allocated && n==size && p==&data[0])
        {
            allocated=false;
        }
}

template <typename T> typename test_allocator<T>::data_container test_allocator<T>::given;

template <typename T> bool test_allocator<T>::is_clean(const size_type used) const noexcept
{
    for (const auto& i: given)
        {
            if (i->in_use(used))
                {
                    fastformat::fmtln(std::cout," Still used @ {0}",static_cast<const void *>(i->get()));
                    return false;
                }
        }
    return true;
}

template <typename T> void test_allocator<T>::deallocate(pointer p, size_type n) noexcept
{
    for(auto& i: given)
        {
            i->deallocate(p,n);
        }
}

template <typename T>
typename test_allocator<T>::pointer test_allocator<T>::allocate(size_type s,test_allocator<void>::const_pointer)
{
    pointer const pt = static_cast<pointer>( ::operator new(s*sizeof(T)) );
    given.emplace_back(given_datum(new given_data {s,pt}));
    return pt;
}

}
}

#endif // CPP11CRYPTO_TESTS_UTILS_TEST_ALLOCATOR_HPP
