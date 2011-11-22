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

// core/zeroize.hpp - Templates to create data that 'zeroizes' whenever
//                    it goes out of scope

#include <cstring>
#include <memory>
#include <boost/type_traits/is_same.hpp>

namespace core {


  namespace policies {
    // Zeroizing helper classes

    struct AllocatorDoesNotZero {};
    struct AllocatorDoesZero {};

    struct DeallocatorDoesNotZero {};
    struct DeallocatorDoesZero {};

    struct DestructorDoesNotZero {};
    struct DestructorDoesZero {};
  }
    


  // Zeroizing class templates

  // To be used by STL templates instead of default (or any other) allocator
  // subsitutted allocator should be fed as parameter to the template
  template <class T,
	    class DestructorPolicy=policies::DestructorDoesZero,
	    class DeallocatorPolicy=policies::DeallocatorDoesNotZero, 
	    class AllocatorPolicy=policies::AllocatorDoesNotZero, 
	    template <class> class underlying_allocator=std::allocator
	    > class allocator;

  template <class DestructorPolicy, class DeallocatorPolicy, class AllocatorPolicy, template <class> class underlying_allocator>
  class allocator<void, DestructorPolicy, DeallocatorPolicy, AllocatorPolicy, underlying_allocator> {
  public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;
    template <class U> struct rebind { typedef allocator<U, DestructorPolicy, DeallocatorPolicy, AllocatorPolicy,underlying_allocator> other; };
  };

  template <class T, class DestructorPolicy, class DeallocatorPolicy, class AllocatorPolicy, template <class> class underlying_allocator>
  class allocator : public underlying_allocator<T> {
    // All members inherited, save for constructors and deallocator
  private:
    typedef underlying_allocator<T> base_allocator;
  public:
    allocator() = default;
    allocator(const allocator& other) = default;
    template <class U> allocator(const allocator<U, DestructorPolicy, DeallocatorPolicy, AllocatorPolicy, underlying_allocator>&) noexcept {}
    
    template <class U> struct rebind { typedef allocator<U, DestructorPolicy, DeallocatorPolicy, AllocatorPolicy, underlying_allocator> other; };

    template <class U> void destroy(U * p) noexcept {
      static_assert(noexcept(base_allocator::destroy),"Destructors should not throw");
      base_allocator::destroy(p);
      if (boost::is_same<DestructorPolicy,policies::DestructorDoesZero>::value) {
	if (nullptr != p) {
	  std::memset(p,0,sizeof *p);
	}
      }
    }

    typename base_allocator::pointer allocate( typename base_allocator::size_type n, allocator<void>::const_pointer hint = 0) {
      typename base_allocator::pointer p=base_allocator::allocate(n,hint);
      if (boost::is_same<AllocatorPolicy,policies::AllocatorDoesZero>::value) {
	std::memset(p,0,n*sizeof *p);
      }
      
    }


    void deallocate(typename base_allocator::pointer p,
		    typename base_allocator::size_type n) {
      if (boost::is_same<DeallocatorPolicy,policies::DeallocatorDoesZero>::value) {
	std::memset(p,0,n*sizeof *p);
      }
      base_allocator::deallocate(p,n);
    }

  };
  
  // Zeroizing base templated class
  template <class DeallocatorPolicy=policies::DeallocatorDoesZero, 
	    class AllocatorPolicy=policies::AllocatorDoesNotZero>
  class ZeroizingBase {
  protected:
    ZeroizingBase()=default;
    ZeroizingBase(const ZeroizingBase&)=default;
    ZeroizingBase& operator=(const ZeroizingBase&)=default;
    ~ZeroizingBase()=default;
  public:
    static void *operator new(std::size_t size) {
      void * p= ::operator new(size);
      if (boost::is_same<AllocatorPolicy,policies::AllocatorDoesZero>::value) {
	std::memset(p,0,size);
      }      
    }
    static void operator delete(void *p,std::size_t size) {
      if (boost::is_same<DeallocatorPolicy,policies::DeallocatorDoesZero>::value) {
	std::memset(p,0,size);
      }      
      ::operator delete(p);
   }

    static void *operator new[](std::size_t size) {
      void * p= ::operator new[](size);
      if (boost::is_same<AllocatorPolicy,policies::AllocatorDoesZero>::value) {
	std::memset(p,0,size);
      }      
    }
    static void operator delete[](void *p,std::size_t size) {
      if (boost::is_same<DeallocatorPolicy,policies::DeallocatorDoesZero>::value) {
	std::memset(p,0,size);
      }      
      ::operator delete[](p);
   }
  };

    
}
