/**
   Copyright 2011-2012, Juan Antonio Zaratiegui Vallecillo

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

// core/zeroize.hpp - Templates to create data that 'zeroizes' whenever
//                    it goes out of scope

#ifndef CPP11CRYPTO_CORE_ZEROIZING_HPP
#define CPP11CRYPTO_CORE_ZEROIZING_HPP

#include <cstring>
#include <memory>
#include <algorithm>

namespace cpp11crypto {
    namespace core {
        namespace {
            void do_zeroize(void * const start,const size_t len) {
                do {
                    ::std::memset(start,0,len);
                } while (
                    start == ::std::find_if(static_cast<const char *>(start),
                                            static_cast<const char *>(start)+len,
                [](const char c) {
                return 0!=c;
            })
            );
            }
        }

        // Zeroizing class templates

        // To be used by STL templates instead of default (or any other) allocator
        // subsitutted allocator should be fed as parameter to the template
        template <class T,template <class> class underlying_allocator=::std::allocator> class allocator;

        template <template <class> class underlying_allocator>
        class allocator<void, underlying_allocator> {
        public:
            typedef void* pointer;
            typedef const void* const_pointer;
            typedef void value_type;
            template <class U> struct rebind {
                typedef allocator<U, underlying_allocator> other;
            };
        };

        template <class T, template <class> class underlying_allocator>
        class allocator : public underlying_allocator<T> {
            // All members inherited, save for constructors and deallocator
        private:
            typedef underlying_allocator<T> base_allocator;
        public:
            allocator() = default;
            allocator(const allocator& other) = default;
            template <class U>
            allocator(const allocator<U, underlying_allocator>&)
            noexcept {}

            template <class U> struct rebind {
                typedef allocator<U, underlying_allocator> other;
            };

            template <class U> void destroy(U * const p) noexcept {
                static_assert(noexcept(base_allocator::destroy),
                "Destructors should not throw");
                base_allocator::destroy(p);
                if (nullptr != p) {
                    do_zeroize(p,sizeof *p);
                }
            }

            typename base_allocator::pointer
            allocate( typename base_allocator::size_type const n,
                      allocator<void>::const_pointer const hint = 0) {
                return base_allocator::allocate(n,hint);
            }


            void deallocate(typename base_allocator::pointer const p,
                            typename base_allocator::size_type const n) {
                base_allocator::deallocate(p,n);
            }

        };

        // Zeroizing base templated class
        class ZeroizingBase {
        protected:
            ZeroizingBase()=default;
            ZeroizingBase(const ZeroizingBase&)=default;
            ZeroizingBase& operator=(const ZeroizingBase&)=default;
            ~ZeroizingBase()=default;
        public:
            static void *operator new(const ::std::size_t size) {
                return ::operator new(size);
            }
            static void operator delete(void * const p,const ::std::size_t size) {
                do_zeroize(p,size);
                ::operator delete(p);
            }

            static void *operator new[](const ::std::size_t size) {
                return ::operator new[](size);
            }
            static void operator delete[](void * const p,const ::std::size_t size) {
                do_zeroize(p,size);
                ::operator delete[](p);
            }
        };


    }

}

#endif // CPP11CRYPTO_CORE_ZEROIZING_HPP
