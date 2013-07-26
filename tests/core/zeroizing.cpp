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
#include <boost/range/counting_range.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdint>
#include <string>
#include <array>
#include <memory>
#include <fastformat/fastformat.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <libcwd/type_info.h>

#include "../utils/test_allocator.hpp"
#include "../utils/test_new_delete.hpp"

namespace cpp11crypto {
    namespace tests {
        using zeroizing_list = boost::mpl::list<
                               std::uint8_t,
                               std::uint16_t,
                               std::uint32_t,
	  std::uint64_t
                               >;

        constexpr auto start = 0u;
        constexpr auto end = 1000u;

        BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_vector_test, T, zeroizing_list ) {
            fastformat::fmtln(std::cout,"Zeroizing test on {0}:{1} starts...",
                              libcwd::type_info_of<T>().demangled_name(),
                              8*sizeof(T));

            using allocator = core::allocator<T,utils::test_allocator>;
            std::vector<T,allocator> data;
            boost::random::mt19937 generator;
            boost::random::uniform_int_distribution<T> distributor;
            for (const auto i : boost::counting_range(start,end)) {
                (void)i;
                data.push_back(distributor(generator));
            }
            data.resize(0);
            fastformat::fmtln(std::cout,"{0}","Check before deallocation");
            BOOST_CHECK( data.get_allocator().is_clean(end) );
            data.shrink_to_fit();
            fastformat::fmtln(std::cout,"{0}","Check after deallocation");
            BOOST_CHECK( data.get_allocator().is_clean(end) );
            data.get_allocator().reset();

            fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", libcwd::type_info_of<T>().demangled_name());
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_list_test, T, zeroizing_list ) {
            fastformat::fmtln(std::cout,"Zeroizing test on {0}:{1} starts...",
                              libcwd::type_info_of<T>().demangled_name(),8*sizeof(T));

            using allocator = core::allocator<T,utils::test_allocator>;
            std::list<T,allocator> data;
            boost::random::mt19937 generator;
            boost::random::uniform_int_distribution<T> distributor;
            for (const auto i : boost::counting_range(start,end)) {
                (void)i;
                data.push_back(distributor(generator));
            }
            data.resize(0);
            fastformat::fmtln(std::cout,"{0}","Check after deallocation");
            BOOST_CHECK( data.get_allocator().is_clean(end) );

            data.get_allocator().reset();

            fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", libcwd::type_info_of<T>().demangled_name());
        }

        namespace {
	  using ZeroizingBase = core::ZeroizingBase<utils::new_delete_checker>;
            class SimpleDerivation : public ZeroizingBase {};
            class VirtualDerivation : public virtual ZeroizingBase {};
            class DiamondDerivation : public VirtualDerivation,virtual ZeroizingBase {};
            class ArrayDerivation {
                std::array<ZeroizingBase,16> zb_array;
            };
            typedef boost::mpl::list<SimpleDerivation,DiamondDerivation,ArrayDerivation> zeroizing_derived_list;
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_base_class, T, zeroizing_derived_list ) {
            fastformat::fmtln(std::cout,"Zeroizing base class derivation test on {0} starts...",
                              libcwd::type_info_of<T>().demangled_name());
	    {
	      std::unique_ptr<T> pointer {new T()};
	    }
            BOOST_CHECK( utils::new_delete_checker::is_clean() );
	    fastformat::fmtln(std::cout,"Zeroizing test on {0} complete.", libcwd::type_info_of<T>().demangled_name());
        }

    }

}
