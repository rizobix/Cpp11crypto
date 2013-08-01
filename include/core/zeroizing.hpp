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

#include <cassert>
#include <cstring>
#include <memory>
#include <algorithm>
#include <utility>
#include "utils/aligned_as_pod.hpp"

namespace cpp11crypto {
    namespace core {
      namespace details  {

	/// Helper struct meant to zeroize the memory used previously by one or more objects of type U
	/// @tparam U type of the objects that resided originally 
	template <typename U>
	struct zeroizer {
	  using casted_to = typename utils::aligned_as_pod<U>::type;
	  void operator()(void * const start,const size_t len) const;
	};

	template <typename U>
	void zeroizer<U>::operator()(void * const start,const size_t len) const {
	  assert(0 == len % sizeof(casted_to));
	    do {
	      ::std::uninitialized_fill_n(static_cast<casted_to *>(start),len/sizeof(casted_to),casted_to{});
	    } while (
		 start == ::std::find_if(static_cast<const casted_to *>(start),
					 static_cast<const casted_to *>(start)+len,
					 [](const casted_to c) {
					   return casted_to{} !=c;
					 })
		 );
	}

      }


      template <typename T> void do_zeroize(T * const start,const size_t len) {	
	details::zeroizer<T>()(start,len);
      }

        // Zeroizing class templates

        // To be used by STL templates instead of default (or any other) allocator
        // subsitutted allocator should be fed as parameter to the template
        template <class T,template <class> class underlying_allocator=::std::allocator> class allocator;

      /// Root specialization of zeroizing allocator.
      /// @tparam underlying_allocator non-zeroizing allocator used by this one. Defaults to ::std::alllo
        template <template <class> class underlying_allocator>
        class allocator<void, underlying_allocator> {
        public:
            typedef void* pointer;
            typedef const void* const_pointer;
            typedef void value_type;
	  /// Auxiliary structure required by STL to reuse allocators
	  /// @tparam U class of the object(s) to be allocated by STL container
            template <class U> struct rebind {
                typedef allocator<U, underlying_allocator> other;
            };
        };

      /// Allocator, STL compatible. It guarantees that all memory used for an object will be zeroized when freed.
      /// @tparam T class of the object type for the STL container
      /// @tparam underlying_allocator non-zeroizing allocator used by this one. Defaults to std::alllocator
        template <class T, template <class> class underlying_allocator>
        class allocator : public underlying_allocator<T> {
            // All members inherited, save for constructors and deallocator
        private:
            typedef underlying_allocator<T> base_allocator;
        public:
	  /// Constructor does nothing special, defaulted
            allocator() noexcept = default;
	  /// Copy constructor does nothing special, defaulted
	  /// @param other allocator to be copied
            allocator(const allocator& other) noexcept = default;
	  /// Move constructor does nothing special, defaulted
	  /// @param other allocator to be moved
            allocator(allocator&& other) noexcept = default;
	  /// Copy constructor from compatible object does nothing special
	  /// @param other compatible allocator to be copied, it is ignored
            template <class U>
            allocator(const allocator<U, underlying_allocator>& other)
            noexcept {}

	  
	  /// Auxiliary structure required by STL to reuse allocators
	  /// @tparam U class of the object(s) to be allocated by STL container
            template <class U> struct rebind {
                typedef allocator<U, underlying_allocator> other;
            };

	  /// Destroys an object. Zeroizes the space used.
	  /// @tparam U type of the object
	  /// @param p address of the object to be destroyed
            template <class U> void destroy(U * const p) noexcept {
                static_assert(noexcept(base_allocator::destroy),
                "Destructors should not throw");
                base_allocator::destroy(p);
                if (nullptr != p) {
		  do_zeroize(p,sizeof *p);
                }
            }

	  /// Allocates space for n objects.
	  /// @param n number of objects to allocate space for
	  /// @param hint pointer, if valid, that may be reallocated.
	  /// @return address allocated
            typename base_allocator::pointer
            allocate( typename base_allocator::size_type const n,
                      allocator<void>::const_pointer const hint = 0) {
                return base_allocator::allocate(n,hint);
            }


	  /// Deallocates space from n objects.
	  /// @param p pointerto first address
	  /// @param n number of objects to allocate space for
            void deallocate(typename base_allocator::pointer const p,
                            typename base_allocator::size_type const n) {
                base_allocator::deallocate(p,n);
            }

        };

      /// Proxy structure to allow using of global operator new... by @ref ZeroizingBase
	struct standard {
            static void *get_new(const ::std::size_t size) {
                return ::operator new(size);
            }
            static void do_delete(void * const p,const ::std::size_t size) {
                ::operator delete(p);
            }

            static void *get_new_array(const ::std::size_t size) {
                return ::operator new[](size);
            }
            static void do_delete_array(void * const p,const ::std::size_t size) {
                ::operator delete[](p);
            }
	};

       /// Zeroizing base class. It guarantees the zeroizing of the memory new'ed for an object of a derived class
      template <typename base_operator = standard>
        class ZeroizingBase {
        protected:
            ZeroizingBase()=default;
            ZeroizingBase(const ZeroizingBase&)=default;
            ZeroizingBase(ZeroizingBase&&)=default;
            ZeroizingBase& operator=(const ZeroizingBase&)=default;
            ZeroizingBase& operator=(ZeroizingBase&&)=default;
            ~ZeroizingBase()=default;
        public:
            static void *operator new(const ::std::size_t size) {
	      return base_operator::get_new(size);
            }
            static void operator delete(void * const p,const ::std::size_t size) {
                do_zeroize(p,size);
		base_operator::do_delete(p,size);
            }

            static void *operator new[](const ::std::size_t size) {
	      return base_operator::get_new_array(size);
            }
            static void operator delete[](void * const p,const ::std::size_t size) {
                do_zeroize(p,size);
		base_operator::do_delete_array(p,size);
            }
        };


    }

}

#endif // CPP11CRYPTO_CORE_ZEROIZING_HPP
