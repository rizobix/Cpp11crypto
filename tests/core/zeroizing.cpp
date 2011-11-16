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

// tests/core/zeroizing.cpp - Tests core/zeroize.hpp

#include "core/zeroizing.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/mpl/list.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>

#include "../utils/test_allocator.hpp"

namespace {
  typedef boost::mpl::list<std::uint8_t,std::uint16_t,std::uint32_t,std::uint64_t> zeroizing_list;
}

BOOST_AUTO_TEST_CASE_TEMPLATE (zeroizing_test, T, zeroizing_list )
{
  //typedef core::allocator<test_allocator<T> > allocator;
  typedef utils::test_allocator<T> allocator;
  allocator alloc;
  std::vector<T,allocator> data(alloc);
  boost::random::mt19937 generator;
  boost::random::uniform_int_distribution<T> distributor;
  std::for_each(boost::counting_iterator<unsigned>(1u),
		boost::counting_iterator<unsigned>(1000u),
		[&](unsigned){data.push_back(distributor(generator));});
  BOOST_CHECK( alloc.is_clean() );
}






