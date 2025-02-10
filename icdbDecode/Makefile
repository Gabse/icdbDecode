# *********************************************************************
#  Generic Makefile With Header Dependencies
# *********************************************************************
#  Author:         	(c) Fabrizio Pietrucci
#  License:       	See the end of this file for license information
#  Created:        	December 17, 2016

#  Last changed:   	@Date: 2017/2/21 11:13
#  Changed by:     	@Author: fabrizio pietrucci
#  Version:  		@Verision 1.0

####### SETTINGS #######


# Application name
ifeq ($(OS),Windows_NT)
APP_NAME = icdbDecode.exe
else
APP_NAME = icdbDecode
endif

# Used Compiler
COMPILER = gcc
# Sourcecode path
SRC = src
# Object path
OBJECT = object
# Application path
BIN = bin
# Libraries path
LIB_PATH = -Llib -Lzlib-1.3.1
STATIC_PATH = lib zlib-1.3.1
# included header file path
INCLUDES = -Ilib -Izlib-1.3.1


# Linker flags
LDFLAGS = -static
# Compiler flags
CFLAGS = -O3 -Wall -static -Wno-unused-result
# Libraries
LIBS = -lz

# Source extension
SRC_EXT = c

###### SETTINGS END ######

# Recursive wildcard, used to get all c files in SRC directory recursively
rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
						$(filter $(subst *,%,$2), $d))

# Get all static libraires in LIB_PATH (used in order to relink
# the program if one of the static libs changes)
STATICLIBS = $(call rwildcard, $(STATIC_PATH), *.a)
# Get all the source files in SRC
SOURCES = $(call rwildcard, $(SRC), *.$(SRC_EXT))
# Set object files names from source file names (used in order
# to relink the program if one of the object file changes)
OBJECTS = $(SOURCES:$(SRC)/%.$(SRC_EXT)=$(OBJECT)/%.o)
# The dependency files that will be used in order to add header dependencies
DEPEND = $(OBJECTS:.o=.d)

# Main target, it creates the folders needed by the build and launches 'all' target
.PHONY: build
build: createdirs
	@$(MAKE) all --no-print-directory
	@echo "Build successful!"

# Creates the needed directories
.PHONY: createdirs
createdirs:
	@echo "Building icdbDecode"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BIN)

all: $(BIN)/$(APP_NAME)

# Links the object files into an executable
$(BIN)/$(APP_NAME): $(OBJECTS) $(STATICLIBS)
	@echo "Linking $@..."
	@$(COMPILER) $(CFLAGS) $(INCLUDES) $(OBJECTS) $(LDFLAGS) -o $@ $(LIB_PATH) $(LIBS)

# Rules for the source files. It compiles source files if obj files are outdated.
# It also creates haeder dependency files (.d files) used to add headers as
# dependencies for the object files with -include later.
$(OBJECT)/%.o: $(SRC)/%.$(SRC_EXT)
	@echo "Compiling $<"
	@$(COMPILER) -c $(CFLAGS) $(INCLUDES) -MP -MMD $< -o $@

# Header dependencies. Adds the rules in the .d files, if they exists, in order to
# add headers as dependencies of obj files (see .d files in OBJECT for more info).
# This rules will be merged with the previous rules.
-include $(DEPEND)

# Removes all the build directories with obj files and executable
.PHONY: clean
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJECT)

# Copyright (C) 2016 Fabrizio Pietrucci

# This program is free software: you can redistribute it and/or
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.