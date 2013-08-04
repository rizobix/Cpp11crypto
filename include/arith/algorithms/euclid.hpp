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

// arith/algorithms/euclid.hpp - Normal and extended Euclid algorithms

#ifndef CPP11CRYPTO_ARITH_ALGORITHMS_EUCLID_HPP
#define CPP11CRYPTO_ARITH_ALGORITHMS_EUCLID_HPP

#include <tuple>

namespace cpp11crypto {
    namespace arith {
        namespace algorithms {
	  namespace euclid {

	    template <typename T>
	    constexpr T gcd(T a, T b) {
	      return b==T{} ? a : gcd(b, a%b);
	    }

	    template <typename T>
	    std::tuple<T,T> extended_gcd(T a, T b) {
	      if (b==T{}) {
		return {T{}+1,T{}};
	      }
	      const T q = a/b;
	      const T r = a%b;
	      const std::tuple<T,T> previous = extended_gcd(b,r);
	      return {std::get<1>(previous),std::get<0>(previous)-q*std::get<1>(previous)};
	    }

	  }
        }
    }
}

#endif // CPP11CRYPTO_ARITH_ALGORITHMS_EUCLID_HPP
