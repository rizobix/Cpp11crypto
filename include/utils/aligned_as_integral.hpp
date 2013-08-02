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

            /// Helper struct designed to select a type in a standard type trait style
            /// @tparam T type subject to select
            template<typename T> struct identity {
                /// Selected type, same as template type
                using type = T;
            };

            /// Helper struct designed to select the greatest integral type with same alignment
            /// @tparam T type subject to search
            /// @tparam U current integral type being compared
            /// @tparam ...Rest Remaining integral types to compare
            template <typename T,typename U,typename...Rest>
            struct select_aligned_as_integral {
                /// Condition used to select type
                constexpr static auto condition = 0 == (alignof(T) % alignof(U));
                /// Selected type
                using type = typename std::conditional< condition,
                      identity<U>,
                      select_aligned_as_integral<T,Rest...>
                      >::type::type;
            };

            /// Partial specialization to end recursion
            /// @tparam T type subject to search
            /// @tparam U current integral type being compared
            template <typename T,typename U>
            struct select_aligned_as_integral<T,U> {
                /// Condition used to select type
                constexpr static auto condition = 0 == (alignof(T) % alignof(U));
                /// Selected type
                using type = typename std::conditional< condition,
                      U,
                      worst_case_type
                      >::type;
            };

        }

        /// Helper struct designed to select the greatest integral type with same alignment
        /// @tparam T type subject to search
        template <typename T>
        struct aligned_as_integral {
            /// Selected type
            using type = typename details::select_aligned_as_integral<T,
                  unsigned long long,unsigned long,
                  unsigned int,unsigned short>::type;
        };

        /// Specialization to catch  the general case, arising from a (void *) pointer
        template <>
        struct aligned_as_integral<void> {
            /// Selected type
            using type = details::worst_case_type; // default case, worst alignment
        };

    }

}

#endif // CPP11CRYPTO_ALIGNED_AS_INTEGRAL_HPP
