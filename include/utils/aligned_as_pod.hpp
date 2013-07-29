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

// utils/align_as_pod.hpp - Templates to select a POD whose alignment
//                    is the same or divides that of other TYPE

#ifndef CPP11CRYPTO_ALIGNED_AS_POD_HPP
#define CPP11CRYPTO_ALIGNED_AS_POD_HPP

namespace cpp11crypto {
    namespace utils {
      namespace details {
	using worst_case_type = unsigned char;
	template <typename T,typename U,typename...Rest>
	struct select_aligned_as_pod;

	template <bool is_same,typename T,typename U,typename...Rest>
	struct selected_pod {
	  using type = U;
	};

	template <typename T,typename U,typename...Rest>
	struct selected_pod<false,T,U,Rest...> {
	  using type = typename select_aligned_as_pod <T,Rest...>::type;
	};

	template <typename T,typename U>
	struct selected_pod<false,T,U> {
	  using type = worst_case_type; // default case, worst alignment
	};

	template <typename T,typename U,typename...Rest>
	struct select_aligned_as_pod {
	  using type = typename selected_pod<
	    0 == (alignof(T) % alignof(U)),
	    T,U,Rest...>::type;
	};
	
      }

      template <typename T>
      struct aligned_as_pod {
	using type = typename details::select_aligned_as_pod<T,
							     unsigned long long,unsigned long,
							     unsigned int,unsigned short>::type;
      };
      template <>
      struct aligned_as_pod<void> {
	using type = details::worst_case_type; // default case, worst alignment
      };

    }

}

#endif // CPP11CRYPTO_ALIGNED_AS_POD_HPP
