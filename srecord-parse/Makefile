# Copyright 2019 Piaggio Fast Forward

#
# Makefile to build PFF Command package
#

# Variables
DEBUG=True

# GLOBAL_LIB_DIR := /usr/local/lib
# GLOBAL_INC_DIR := /usr/local/include

# ROOT_BIN_DIR := ../../../bin
# ROOT_LIB_DIR := ../../../lib
# ROOT_INC_DIR := ../../../include

# LOCAL_SRC_DIR := src
# LOCAL_OBJ_DIR := obj

# INC_DIRS= $(LOCAL_SRC_DIR) $(ROOT_INC_DIR) $(GLOBAL_INC_DIR)
# LIB_DIRS= $(ROOT_LIB_DIR) $(GLOBAL_LIB_DIR)

# INC=$(addprefix -I, $(INC_DIRS))
# LIBS=$(addprefix -L, $(LIB_DIRS))
# LINKS:=-lpff_mq -lpthread -lzmq -lpff_log_client

# flags
CXXFLAGS := -std=c++11
INCLUDE_FLAGS = $(INC)
ARFLAGS=rv
LDFLAGS=

ifdef DEBUG
CXXFLAGS+=-g
else
CXXFLAGS+=-O2
endif

# Phony rules
.DEFAULT_GOAL := all
.PHONY: all clean create_dirs rebuild

SRC_FILES := $(subst srec-parse.cpp, ,$(wildcard *.cpp))
OBJ_FILES := $(patsubst %, %, $(patsubst %.cpp, %.o, $(SRC_FILES)))

all: srec-parse

srec-parse: srec-parse.cpp $(OBJ_FILES)
	g++ $(CXXFLAGS) $(INCLUDE_FLAGS) srec-parse.cpp -o $@ $(OBJ_FILES) $(LIBS) 


%.o : %.cpp
	g++ $(CXXFLAGS) -c $(INCLUDE_FLAGS) $< -o $@

clean:
	@rm -rvf srec-parse

rebuild:
	$(MAKE) clean
	$(MAKE) all
