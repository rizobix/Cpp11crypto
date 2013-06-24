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

   You should have received a copy of the GNU General Public License with Cpp11crypto.
   If not, see <http://www.gnu.org/licenses/>.
**/

// tests/core/zeroizing.cpp - Tests core/zeroize.hpp

#include "core/zeroizing.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/mpl/list.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdint>
#include <string>
#include <ios>
#include <fastformat/fastformat.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <libcwd/type_info.h>

#include "../utils/test_allocator.hpp"

namespace cpp11crypto {
  namespace tests {
    namespace {
      //      typedef boost::mpl::list<std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t> zeroizing_list;
      typedef boost::mpl::list<std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t> zeroizing_list;
    }

    const unsigned start=1u;
    const unsigned end=2u; //1000u

    template <typename F> void test_all(F f) {
      std::for_each(boost::counting_iterator<boost::remove_const<decltype(start)>::type>(start),
		    boost::counting_iterator<boost::remove_const<decltype(end)>::type>(end),
		    f);
    }

    BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_vector_test, T, zeroizing_list ) {
      fastformat::fmtln(std::cout,"Zeroizing test on {0}:{1} starts...", 
			libcwd::type_info_of<T>().demangled_name(), // typeid(T).name(),
			8*sizeof(T));

      typedef core::allocator<T,core::policies::DestructorDoesZero,
			      core::policies::DeallocatorDoesNotZero,
			      core::policies::AllocatorDoesZero,utils::test_allocator> allocator;
      std::vector<T,allocator> data;
      boost::random::mt19937 generator;
      boost::random::uniform_int_distribution<T> distributor;
      test_all( [&](unsigned n)
		{
		  data.push_back(distributor(generator));
		  std::cout << "d_@" << static_cast<const void *>(&*data.begin()) << std::endl;
		  std::cout << "d0@" << static_cast<const void *>(&data[0]) << std::endl;
		});
      data.resize(0);
      std::cout<< "Check before deallocation" << std::endl;
      BOOST_CHECK( data.get_allocator().is_clean() );
      data.shrink_to_fit();
      std::cout<< "Check after deallocation" << std::endl;
      BOOST_CHECK( data.get_allocator().is_clean() );

      fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", typeid(T).name());
      }

    /*  BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_list_test, T, zeroizing_list ) {
      fastformat::fmtln(std::cout,"Zeroizing test on {0}:{1} starts...", typeid(T).name(),8*sizeof(T));

      typedef core::allocator<T,core::policies::DestructorDoesZero,core::policies::DeallocatorDoesNotZero,core::policies::AllocatorDoesZero,utils::test_allocator> allocator;
      std::list<T,allocator> data;
      boost::random::mt19937 generator;
      boost::random::uniform_int_distribution<T> distributor;
      test_all([&](unsigned n){data.push_back(distributor(generator));});
      data.resize(0);
      std::cout<< "Check after deallocation" << std::endl;
      BOOST_CHECK( data.get_allocator().is_clean() );

      fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", typeid(T).name());
      }

    namespace {
      class SimpleDerivation : public core::ZeroizingBase<> {};
      class VirtualDerivation : public virtual core::ZeroizingBase<> {};
      class DiamondDerivation : public VirtualDerivation,virtual core::ZeroizingBase<> {};
      class ArrayDerivation {
      public:
	ArrayDerivation():pointer(new core::ZeroizingBase<>[16]) {}
      private:
	boost::scoped_array<core::ZeroizingBase<> > pointer;
      };
      typedef boost::mpl::list<SimpleDerivation,DiamondDerivation,ArrayDerivation> zeroizing_derived_list;
    }

    BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_base_class, T, zeroizing_derived_list ) {
      fastformat::fmtln(std::cout,"Zeroizing base class derivation test on {0} starts...", typeid(T).name());
      boost::scoped_ptr<T> pointer(new T());
      fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", typeid(T).name());
    }
    */
  }

}


