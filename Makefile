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

STLSOFT ?= $(BASE_DIR)/../stlsoft
FASTFORMAT_ROOT ?= $(BASE_DIR)/../fastformat
FASTFORMAT_GCC_VERSION ?= gcc47
FASTFORMAT_COPIED_VERSION ?= gcc47
FASTFORMAT_BUILD_ROOT ?= $(FASTFORMAT_ROOT)/build
FASTFORMAT_BUILD_DIR ?= $(FASTFORMAT_BUILD_ROOT)/$(FASTFORMAT_GCC_VERSION).unix
FASTFORMAT_COPIED_DIR ?= $(FASTFORMAT_BUILD_ROOT)/$(FASTFORMAT_COPIED_VERSION).unix
FASTFORMAT_LIB ?= fastformat.0.core.$(FASTFORMAT_GCC_VERSION)

CXX_OPTIONS = -std=c++0x -Wall -Wextra -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -pedantic -pedantic-errors -Wno-unused-parameter -Wno-unused-label
# -Werror 

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
TEST_LIBRARIES = -L$(BOOST_LIBRARY_FOLDER) -lboost_unit_test_framework -L$(FASTFORMAT_ROOT)/lib -l$(FASTFORMAT_LIB)
TEST_OPTIONS = $(CXX_OPTIONS) -g -D BOOST_TEST_DYN_LINK -D FASTFORMAT_USE_VOID_POINTERS_CONVERSION_SHIMS



$(TEST_PROGRAM): $(TEST_SOURCES) $(TEST_HEADERS) $(HEADERS)
	$(CXX) $(TEST_OPTIONS) $(TEST_INCLUDES) $(TEST_SOURCES) $(TEST_LIBRARIES) -o $(TEST_PROGRAM)

test: $(TEST_PROGRAM)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(BOOST_LIBRARY_FOLDER) $(TEST_PROGRAM)

# Build required boost libraries
boost:
	cd $(BOOST_FOLDER) && ./bootstrap.sh --with-libraries=test && ./b2

# Patches and builds fastformat libraries
fastformat: $(FASTFORMAT_BUILD_DIR)
	cd $(FASTFORMAT_BUILD_DIR) && STLSOFT=$(STLSOFT) make build

$(FASTFORMAT_BUILD_DIR):
	cp -r $(FASTFORMAT_COPIED_DIR) $(FASTFORMAT_BUILD_DIR)
	patch $(FASTFORMAT_BUILD_DIR)/makefile ff_patch
