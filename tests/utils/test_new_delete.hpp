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

   You should have received a copy of the GNU General Public License
   along with Cpp11crypto.  If not, see <http://www.gnu.org/licenses/>.
**/

// tests/utils/test_new_delete.cpp - Test helper struct

#ifndef CPP11CRYPTO_TESTS_UTILS_TEST_NEW_DELETE_HPP
#define CPP11CRYPTO_TESTS_UTILS_TEST_NEW_DELETE_HPP
#include <unordered_map>
#include <cassert>

namespace cpp11crypto {
    namespace utils {
      namespace {
	  struct pointer_data {
	    size_t size;
	    bool in_use;
	  };
	  using Map = std::unordered_map<void *,pointer_data>;

	template <bool DUMMY=true> struct Maps {

	  static Map simple_map;
	  static Map array_map;

	  static void *add_simple(void *p,const ::std::size_t size) {
	      assert(simple_map.find(p)==simple_map.end() && array_map.find(p)==array_map.end()); 
	      simple_map[p]={size,true};	
	      return p;
	  }
	  static void remove_simple(void *p,const ::std::size_t size) {
	      assert(simple_map.find(p)!=simple_map.end() && array_map.find(p)==array_map.end()); 
	      assert(simple_map[p].size==size);
	      simple_map[p].in_use=false;
	  }
	  static void *add_array(void *p,const ::std::size_t size) {
	      assert(simple_map.find(p)==simple_map.end() && array_map.find(p)==array_map.end()); 
	      assert(array_map[p].size==size);
	      array_map[p]={size,true};
	      return p;
	  }
	  static void remove_array(void *p,const ::std::size_t size) {
	      assert(simple_map.find(p)==simple_map.end() && array_map.find(p)!=array_map.end()); 
	      array_map[p].in_use=false;
	  }

	  static bool is_clean() {
	    bool ok=true;
	    for (const auto& x : simple_map) {
	      if (x.second.in_use) {
		ok=false;
	      }
	      ::operator delete(x.first);
	    }
	    simple_map.clear();

	    for (const auto& x : array_map) {
	      if (x.second.in_use) {
		ok=false;
	      }
	      ::operator delete[](x.first);
	    }
	    array_map.clear();

	    return ok;
	  }
	};

	template <bool DUMMY> Map Maps<DUMMY>::simple_map;
	template <bool DUMMY> Map Maps<DUMMY>::array_map;
      }
      struct new_delete_checker {

            static void *get_new(const ::std::size_t size) {
	      return Maps<true>::add_simple(::operator new(size),size);
            }
            static void do_delete(void * const p,const ::std::size_t size) {
	      Maps<true>::remove_simple(p,size);
            }

            static void *get_new_array(const ::std::size_t size) {
	      return Maps<true>::add_array(::operator new[](size),size);
            }
            static void do_delete_array(void * const p,const ::std::size_t size) {
	      Maps<true>::remove_array(p,size);
            }
	static bool is_clean() {
	  return Maps<true>::is_clean();
	}
      };
    }
}


#endif // CPP11CRYPTO_TESTS_UTILS_TEST_NEW_DELETE_HPP
