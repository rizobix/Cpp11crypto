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

#include <memory>
//#include <boost/noncopyable.hpp>
//#include <boost/mpl/logical.hpp>
//#include <boost/utility/enable_if.hpp>
//#include <boost/type_traits/is_pod.hpp>

namespace core {

  // Zeroizing policies

  // To be used by STL templates instead of default (or any other) allocator
  // subsitutted allocator should be fed as parameter to the template
  template <class T, template <class U> class underlying_allocator=std::allocator>
  class allocator;

  template <template <class> class underlying_allocator> class allocator<void,underlying_allocator> {
  public:
    typedef void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;
    template <class U> struct rebind { typedef allocator<U,underlying_allocator> other; };
  };

  template <class T, template <class> class underlying_allocator>
  class allocator : public underlying_allocator<T> {
    // All members inherited, save for constructors and deallocator
  private:
    typedef underlying_allocator<T> base_allocator;
  public:
    allocator() = default;
    allocator(const allocator& other) = default;
    template <class U> allocator(const allocator<U,underlying_allocator>&) noexcept {}
    
    template <class U> struct rebind { typedef allocator<U,underlying_allocator> other; };

    void deallocate(typename base_allocator::pointer p,
		    typename base_allocator::size_type n) {
      std::uninitialized_fill_n(p,n,T());
      base_allocator::deallocate(p,n);
    }
  };
  
  // Zeroizing class templates

  // Generic (empty) definition
  template <typename T, typename Enable = void >
  class zeroizing;

  // Specialization for arithmetic (char,int,float) types
  // template <typename T, boost::enable_if<boost::is_arithmetic<T> > >
  // class zeroizing : private boost::noncopyable{
  // public:
  //   zeroizing():value() {}
  //   explicit zeroizing(const T v):value(v) {}
  //   ~zeroizing() {value=T();}
  //   operator T() const {return value;} 
  // private:
  //   T value;
  // };

  // Specialization for struct and array types (ToBeTested!)
  // template <typename T,
  // 	    boost::enable_if<
  // 	      boost::mpl::and_ <boost::is_pod<T>,
  // 				boost::not_<is_arithmetic<T> >,
  // 				boost::not_<is_pointer<T> > > >::type >
  // class zeroizing : private boost::noncopyable{
  // public:
  //   zeroizing():value() {}
  //   explicit zeroizing(T&& v):value() {}
  //   explicit zeroizing(const T& v):value() {}
  //   ~zeroizing() {value=T();}
  //   operator const T& () const {return value;} 
  // private:
  //   T value;
  // };

    
}
