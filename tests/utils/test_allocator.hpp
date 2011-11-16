/**
   Copyright 2011, Juan Antonio Zaratiegui Vallecillo

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

#include <memory>
#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace utils {
  template <typename T> class test_allocator;

  template <> class test_allocator<void> {
  public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;
    template <class U> struct rebind { typedef test_allocator<U> other; };
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

    template <class U> struct rebind { typedef test_allocator<U> other; };

    test_allocator() noexcept : given() {}
    test_allocator(const test_allocator& other) noexcept : given(other.given) {}
    template <class U> test_allocator(const test_allocator<U>&) noexcept;
    ~test_allocator() {}

    pointer address(reference x) const noexcept;
    const_pointer address(const_reference x) const noexcept;

    pointer allocate(size_type, test_allocator<void>::const_pointer hint = 0);
    void deallocate(pointer p, size_type n) noexcept;

    size_type max_size() const noexcept {return std::allocator<T>().max_size();}

    template<class U, class... Args> void construct(U* p, Args&&... args) {
      new(static_cast<void *>(p)) U(std::forward<Args>(args)...);
    }
    template <class U> void destroy(U* p) { p->~U(); }

    // ?????
    bool operator!=(test_allocator& other) {return false;}

    // testing part
  public:
    bool is_clean() const noexcept;

    // data
  private:
    class given_data {
    public:
      given_data() noexcept;
      given_data(const given_data&) noexcept;
      given_data& operator=(const given_data&) noexcept;
      
      given_data(size_type s,T *p):allocated(true),size(s),data(p) {}

      ~given_data() {
	::operator delete(static_cast<void *>(data));
      }

      class const_iterator {
      public:
	const_iterator(const T *p):pointer(p) {}

	bool operator!=(const const_iterator& another) const noexcept {return pointer != another.pointer;}
	const_iterator& operator++() noexcept {++pointer;return *this;}
	T operator*() const noexcept { return  *pointer; }
      private:
	const T * pointer;
      };

      bool in_use() const noexcept {
	for(auto i=0;i!=size;++i) {
	  if (data[i]!=T()) return true;
	}
	return allocated;
      }
      void deallocate(pointer p, size_type n) noexcept {
	if (allocated && n==size && p==&data[0]) {
	  allocated=false;
	}
      }

    private:
      bool allocated;
      const size_type size;
      T * const data;
    };
    typedef boost::shared_ptr<given_data> given_datum;
    std::vector<given_datum> given;
  };

  template <typename T> bool test_allocator<T>::is_clean() const noexcept {
    const T zero=T();
    for (const auto& i: given) {
      if (i->in_use()) return false;
    }
    return true;
  }

  template <typename T> void test_allocator<T>::deallocate(pointer p, size_type n) noexcept {
    for(auto i: given) {
      i->deallocate(p,n);
    }
  }

  template <typename T> typename test_allocator<T>::pointer test_allocator<T>::allocate(size_type s,
											test_allocator<void>::const_pointer) {
    pointer const pt = static_cast<pointer>( ::operator new(s*sizeof(T)) );
    given.push_back(given_datum(new given_data(s,pt)));
    return pt;
  }

}
