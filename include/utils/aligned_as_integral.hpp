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

   You should have received a copy of the GNU General Public License
   along with Cpp11crypto.  If not, see <http://www.gnu.org/licenses/>.
**/

// utils/aligned_as_integral.hpp - Templates to select a POD whose alignment
//                    is the same or divides that of other TYPE

#ifndef CPP11CRYPTO_ALIGNED_AS_INTEGRAL_HPP
#define CPP11CRYPTO_ALIGNED_AS_INTEGRAL_HPP

#include <type_traits>

namespace cpp11crypto {
    namespace utils {
      namespace details {
	using worst_case_type = unsigned char;

	template <bool accepted,typename T,typename U> struct test_selection {
	  static_assert(0==(alignof(T)%alignof(U)),"vaya hombre");	  
	};
	template <typename T,typename U> struct test_selection<false,T,U> {
	};

	template<typename T> struct identity {
	  using type = T;
	};

	/// Helper struct designed to select the greatest integral type with same alignment
	/// @tparam T type subject to search
	/// @tparam U current integral type being compared
	/// @tparam ...Rest Remaining integral types to compare
	template <typename T,typename U,typename...Rest>
	struct select_aligned_as_integral {
	  constexpr static auto condition = 0 == (alignof(T) % alignof(U));
	  using type = typename std::conditional< condition,
						  identity<U>,
						  select_aligned_as_integral<T,Rest...>
						  >::type::type;
	  test_selection<condition,T,type> dummy;
	};
	
	template <typename T,typename U>
	struct select_aligned_as_integral<T,U> {
	  constexpr static auto condition = 0 == (alignof(T) % alignof(U));
	  using type = typename std::conditional< condition,
						  U,
						  worst_case_type
						  >::type;
	  test_selection<condition,T,type> dummy;
	};
	
      }

	/// Helper struct designed to select the greatest integral type with same alignment
	/// @tparam T type subject to search
      template <typename T>
      struct aligned_as_integral {
	using type = typename details::select_aligned_as_integral<T,
							     unsigned long long,unsigned long,
							     unsigned int,unsigned short>::type;
      };

	/// Specialization to catch  the general case, arising from a (void *) pointer
      template <>
      struct aligned_as_integral<void> {
	using type = details::worst_case_type; // default case, worst alignment
      };

    }

}

#endif // CPP11CRYPTO_ALIGNED_AS_INTEGRAL_HPP
