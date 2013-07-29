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

   You should have received a copy of the GNU General Public License with Cpp11crypto.
   If not, see <http://www.gnu.org/licenses/>.
**/

// tests/core/aligned_as_pod.cpp - Tests utils/aligned_as_pod.hpp

#include "utils/aligned_as_pod.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include <fastformat/fastformat.hpp>

#include <libcwd/type_info.h>

namespace cpp11crypto {
    namespace tests {

        using aligning_list = boost::mpl::list<
	  std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t,
	  float,double,long double, void *, char * >;

        BOOST_AUTO_TEST_CASE_TEMPLATE (aligning_as_a_pod, T, aligning_list ) {
	  using selected_type = typename utils::aligned_as_pod<T>::type;
            fastformat::fmtln(std::cout,"Alignment test {0}({1}) selects {2}({3}).",
			      libcwd::type_info_of<T>().demangled_name(),
			      sizeof(T),
			      libcwd::type_info_of<selected_type>().demangled_name(),
			      sizeof(selected_type)
			      );
            BOOST_CHECK( 0 == sizeof(T) % sizeof(selected_type) );
	}
    }
}
