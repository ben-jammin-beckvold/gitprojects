BIN_DIR ?= ../../bin
LIB_DIR ?= ../../lib
INC_DIR ?= ../../include
OBJ_DIR ?= obj

LIB_DIRS= $(LIB_DIR)

SRC=gpio_chip_test.cpp
OBJ=$(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

INC=$(addprefix -I, $(INC_DIR))
LIBS=$(addprefix -L, $(LIB_DIRS))

LINKS:= -lpthread
CXXFLAGS := -std=c++17 -g

.PHONY: all clean rebuild create_dirs

TST=$(BIN_DIR)/gpio_chip_test

all: create_dirs $(TST) 

clean: 
	rm -vf $(TST) $(OBJ)

create_dirs:
	@mkdir -vp $(OBJ_DIR) $(BIN_DIR)

rebuild: clean all

$(TST): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LINKS) $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

include ../../lint.mk
