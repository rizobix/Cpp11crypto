# Copyright 2011, Juan Antonio Zaratiegui Vallecillo

#  This file is part of Cpp11crypto.

#  Cpp11crypto is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

#  Cpp11crypto is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with Cpp11crypto.  If not, see <http://www.gnu.org/licenses/>.

### Makefile
### Targets: all test

BASE_DIR:=$(shell pwd)

BOOST_FOLDER ?= $(BASE_DIR)/../boost
BOOST_LIBRARY_FOLDER  = $(BOOST_FOLDER)/stage/lib
BOOST_LIBRARY_LIST = test
BOOST_TEST_LIBRARIES=unit_test_framework
BOOST_TEST_LIBRARIES_COMMAND=$(patsubst %,-lboost_%,$(BOOST_TEST_LIBRARIES))

STLSOFT ?= $(BASE_DIR)/../stlsoft
FASTFORMAT_ROOT ?= $(BASE_DIR)/../fastformat
FASTFORMAT_GCC_VERSION ?= gcc47
FASTFORMAT_COPIED_VERSION ?= gcc47
FASTFORMAT_BUILD_ROOT ?= $(FASTFORMAT_ROOT)/build
FASTFORMAT_BUILD_DIR=$(FASTFORMAT_BUILD_ROOT)/$(FASTFORMAT_GCC_VERSION).unix
FASTFORMAT_LIB ?= fastformat.0.core.$(FASTFORMAT_GCC_VERSION)

CXX_OPTIONS = -std=c++0x -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -pedantic -pedantic-errors -Werror

HEADERS=include/core/zeroizing.hpp
.PHONY: all test boost fastformat

all:
	@echo Nothing to do yet.

clean:
	@rm -f $(TEST_PROGRAM)

TEST_SOURCES = tests/test.cpp tests/core/zeroizing.cpp
TEST_HEADERS = tests/utils/test_allocator.hpp
TEST_PROGRAM = tests/test
TEST_INCLUDES = -Iinclude -I$(BOOST_FOLDER) -I$(STLSOFT)/include -I$(FASTFORMAT_ROOT)/include
TEST_LIBRARIES = -lcwd -L$(BOOST_LIBRARY_FOLDER) $(BOOST_TEST_LIBRARIES_COMMAND) -L$(FASTFORMAT_ROOT)/lib -l$(FASTFORMAT_LIB)
TEST_OPTIONS = $(CXX_OPTIONS) -g -D BOOST_TEST_DYN_LINK -D FASTFORMAT_USE_VOID_POINTERS_CONVERSION_SHIMS



$(TEST_PROGRAM): $(TEST_SOURCES) $(TEST_HEADERS) $(HEADERS)
	$(CXX) $(TEST_OPTIONS) $(TEST_INCLUDES) $(TEST_SOURCES) $(TEST_LIBRARIES) -o $(TEST_PROGRAM)

test: $(TEST_PROGRAM)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(BOOST_LIBRARY_FOLDER) $(TEST_PROGRAM)

# Build required boost libraries
boost:
	cd $(BOOST_FOLDER) && ./bootstrap.sh --with-libraries=$(BOOST_LIBRARY_LIST) && ./b2

# Patches and builds fastformat libraries
fastformat: $(FASTFORMAT_BUILD_DIR)
	cd $(FASTFORMAT_BUILD_DIR) && STLSOFT=$(STLSOFT) make build

