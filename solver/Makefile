CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Iinclude -MMD -MP

BUILD_DIR := build

SOLVER_MAIN := src/main.cpp
PDB_MAIN := src/gerar_pdb.cpp
TEST_MAIN := tests/testes.cpp

CORE_SRCS := $(filter-out $(SOLVER_MAIN) $(PDB_MAIN), $(wildcard src/*.cpp))
CORE_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CORE_SRCS))

SOLVER_OBJ := $(BUILD_DIR)/$(SOLVER_MAIN:.cpp=.o)
PDB_OBJ := $(BUILD_DIR)/$(PDB_MAIN:.cpp=.o)
TEST_OBJ := $(BUILD_DIR)/$(TEST_MAIN:.cpp=.o)

DEPS := $(CORE_OBJS:.o=.d) $(SOLVER_OBJ:.o=.d) $(PDB_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

.PHONY: all pdb test clean

all: solver

solver: $(CORE_OBJS) $(SOLVER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

gerar_pdb: $(CORE_OBJS) $(PDB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

testes: $(CORE_OBJS) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

pdb: gerar_pdb
	./gerar_pdb

test: testes
	./testes

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) solver gerar_pdb testes

-include $(DEPS)
