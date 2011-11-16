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

BOOST_FOLDER ?= ../boost_1_48_0
BOOST_LIBRARY_FOLDER  = $(BOOST_FOLDER)/stage/lib

STLSOFT=../stlsoft-1.9.109
FASTFORMAT_ROOT=../fastformat-0.7.1-alpha-3

CXX_OPTIONS = -std=c++0x

.PHONY: all test

all:
	@echo Nothing to do yet.

clean:
	@rm $(TEST_PROGRAM)

TEST_SOURCES = tests/test.cpp tests/core/zeroizing.cpp
TEST_PROGRAM = tests/test
TEST_INCLUDES = -Iinclude -I$(BOOST_FOLDER) -I$(STLSOFT) -I$(FASTFORMAT_ROOT)/include
TEST_LIBRARIES = -L$(BOOST_LIBRARY_FOLDER) -lboost_unit_test_framework
TEST_OPTIONS = $(CXX_OPTIONS) -g -D BOOST_TEST_DYN_LINK


$(TEST_PROGRAM): $(TEST_SOURCES)
	$(CXX) $(TEST_OPTIONS) $(TEST_INCLUDES) $(TEST_SOURCES) $(TEST_LIBRARIES) -o $(TEST_PROGRAM)

test: $(TEST_PROGRAM)
# LD_LIBRARY_PATH=$(BOOST_LIBRARY_FOLDER) $(TEST_PROGRAM)
