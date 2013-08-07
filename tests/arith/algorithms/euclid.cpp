/**
   Copyright 2013, Juan Antonio Zaratiegui Vallecillo

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

// tests/arith/algorithmds/euclid.cpp - Tests arith/algorithmds/euclid.hpp

#include "arith/algorithms/euclid.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/mpl/list.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdint>
#include <string>
#include <array>
#include <memory>
#include <numeric>
#include <fastformat/fastformat.hpp>

#include <libcwd/type_info.h>


namespace cpp11crypto {
    namespace tests {

        template <typename T,T A, T B> struct compileTime {
            T get_a() const {
                return A;
            }
            T get_b() const {
                return B;
            }

            std::array<unsigned char,arith::algorithms::euclid::gcd(A,B)> arr_gcd;
        };


        using gcd_list = boost::mpl::list<
                         compileTime<int,9,6>,
                         compileTime<int,7,5>,
                         compileTime<int,1440,52>,
                         compileTime<int,511,1347>,
                         compileTime<unsigned,9,6>,
                         compileTime<unsigned,7,5>,
                         compileTime<unsigned,1440,52>,
                         compileTime<unsigned,511,1347>
                         >;

        BOOST_AUTO_TEST_CASE_TEMPLATE (gcd_test, T, gcd_list ) {
            fastformat::fmtln(std::cout,"(E)GCD test on {0}:{1},{2} starts...",
                              libcwd::type_info_of<T>().demangled_name(),
                              T().get_a(),T().get_b());

            auto a=T().get_a()+1;
            auto b=T().get_b()+1;

            const auto rs=arith::algorithms::euclid::gcd(--a,--b);
            const auto ers=arith::algorithms::euclid::extended_gcd(a,b);

            fastformat::fmtln(std::cout,"GCD on RT gives {0}",rs);
            BOOST_CHECK( rs == T().arr_gcd.size() );
            fastformat::fmtln(std::cout,"(E)GCD on RT gives {0},{1}",std::get<0>(ers),std::get<1>(ers));
            BOOST_CHECK( (rs == a*std::get<0>(ers)+b*std::get<1>(ers)) );

            fastformat::fmtln(std::cout,"GCD test on {0} complete.", libcwd::type_info_of<T>().demangled_name());
        }

    }

}
