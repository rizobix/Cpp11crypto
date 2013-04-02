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

//#define DEBUG_ALLOCATIONS
//#define DEBUG_ALLOCATION_POINTERS

#include <memory>
#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>


#include <iostream>

namespace cpp11crypto {
  namespace utils
  {
    template <typename T> class test_allocator;

    template <> class test_allocator<void> {
    public:
      typedef void* pointer;
      typedef const void* const_pointer;
      typedef void value_type;
      template <class U> struct rebind {
	typedef test_allocator<U> other;
      };
    };

    template <typename T> class test_allocator {
      // standard part
    public:
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef T value_type;

      template <class U> struct rebind {
	typedef test_allocator<U> other;
      };

      test_allocator() = default;
      test_allocator(const test_allocator& other) = default;
      ~test_allocator() = default;


      pointer allocate(size_type, test_allocator<void>::const_pointer hint = 0);
      void deallocate(pointer p, size_type n) noexcept;

      size_type max_size() const noexcept(noexcept(std::allocator<T>().max_size)) {
	return std::allocator<T>().max_size();
      }

      template<class U, class... Args> void construct(U* p, Args&&... args) {
	(void) new(static_cast<void *>(p)) U(std::forward<Args>(args)...);
      }
      template <class U> void destroy(U* p) {
	p->~U();
      }

      // ????? GCC requires it!
      bool operator!=(test_allocator& other) noexcept {return false;}

      // testing part
    public:
      bool is_clean() const noexcept;

      // data
    private:
      class given_data
      {
      public:
	given_data() noexcept;
	given_data(const given_data&) noexcept;
	given_data& operator=(const given_data&) noexcept;

	given_data(size_type s,T *p):allocated(true),size(s),data(p) {
#ifdef DEBUG_ALLOCATION_POINTERS
	  using namespace std;
	  cout << "gd+ " << size <<'@' << static_cast<const void *>(data) << endl;
#endif //DEBUG_ALLOCATION_POINTERS
	}

	~given_data() {
	  ::operator delete(static_cast<void *>(data));
	}
	
	const T * get() const {return data;}

	class const_iterator {
	public:
	  const_iterator(const T *p):pointer(p) {}

	  bool operator!=(const const_iterator& another) const noexcept {
	    return pointer != another.pointer;
	  }
	  const_iterator& operator++() noexcept {++pointer; return *this;}
	  T operator*() const noexcept {
	    return  *pointer;
	  }
	private:
	  const T * pointer;
	};

	bool in_use() const noexcept;
	void deallocate(pointer p, size_type n) noexcept;

      private:
	bool allocated;
	const size_type size;
	T * const data;
      };
    private:
      typedef boost::shared_ptr<given_data> given_datum;
      typedef std::vector<given_datum> data_container;
      static data_container given;
    };

    template <typename T>
    bool test_allocator<T>::given_data::in_use() const noexcept {
      for(size_type i=0; i!=size; ++i) {
	if (data[i]!=T())	{
	  return true;
	}
      }
      return false;
    }

    template <typename T>
    void test_allocator<T>::given_data::deallocate(pointer p, size_type n) noexcept {
      if (allocated && n==size && p==&data[0]) {
	allocated=false;
#ifdef DEBUG_ALLOCATION_POINTERS
	  using namespace std;
	  cout << "gd- " << size <<'@' << static_cast<const void *>(data) << endl;
#endif //DEBUG_ALLOCATION_POINTERS
      }
    }

    template <typename T> typename test_allocator<T>::data_container test_allocator<T>::given;

    template <typename T> bool test_allocator<T>::is_clean() const noexcept {
      for (const auto& i: given) {
	if (i->in_use()) {
	  using namespace std;
	  cout << " Still used @ " << static_cast<const void *>(i->get()) << endl;
	  return false;
	}
      }
      return true;
    }

    template <typename T> void test_allocator<T>::deallocate(pointer p, size_type n) noexcept {
#ifdef DEBUG_ALLOCATIONS
      using namespace std;
      cout << "deallocate " << static_cast<const void *>(p)<< ',' << n << endl;
#endif //DEBUG_ALLOCATIONS
      for(auto& i: given) {
	i->deallocate(p,n);
      }
#ifdef DEBUG_ALLOCATIONS
      cout << "deallocated " << static_cast<const void *>(p)<< ',' << n << endl;
#endif //DEBUG_ALLOCATIONS
    }

    template <typename T>
    typename test_allocator<T>::pointer test_allocator<T>::allocate(size_type s,test_allocator<void>::const_pointer) {
#ifdef DEBUG_ALLOCATIONS
      using namespace std;
      cout << "allocate " << s << endl;
#endif //DEBUG_ALLOCATIONS
      pointer const pt = static_cast<pointer>( ::operator new(s*sizeof(T)) );
#ifdef DEBUG_ALLOCATIONS
      cout << "allocated@ " << static_cast<const void *>(pt) << endl;
#endif //DEBUG_ALLOCATIONS
      given.push_back(given_datum(new given_data(s,pt)));
#ifdef DEBUG_ALLOCATIONS
      cout << "pushed = " << given.size() << endl;
      cout << "returns = " << static_cast<const void *>(pt) << endl;
#endif //DEBUG_ALLOCATIONS
      return pt;
    }

  }
}

#ifdef DEBUG_ALLOCATIONS
#undef DEBUG_ALLOCATIONS
#endif //DEBUG_ALLOCATIONS
#ifdef DEBUG_ALLOCATION_POINTERS
#undef DEBUG_ALLOCATION_POINTERS
#endif //DEBUG_ALLOCATION_POINTERS


#endif // CPP11CRYPTO_TESTS_UTILS_TEST_ALLOCATOR_HPP
