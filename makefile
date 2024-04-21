
CC := emcc

SRCF := src/core
TEMP := obj
TARGET := src/lib

SRC := $(wildcard $(SRCF)/*.cpp) $(wildcard $(SRCF)/*/*.cpp) $(wildcard $(SRCF)/*/*/*.cpp)
OBJ := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.o,$(SRC))
DEP := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.d,$(SRC))

PRE := $(SRCF)/pre.js
OUT := ref

STD := -std=c++14
ESFLAGS :=\
	-sINITIAL_MEMORY=100MB\
	-sALLOW_MEMORY_GROWTH\
	-sMALLOC=dlmalloc\
	-sASYNCIFY=1\
	-sASYNCIFY_IMPORTS=['emscripten_utils_get_double_impl']\
	-sEXPORTED_RUNTIME_METHODS=['setValue']\
	-sFILESYSTEM=0\
	-sMODULARIZE=1\
	-sENVIRONMENT=worker\
	-O3

WASM := $(TARGET)/$(OUT).wasm

all: $(WASM)

$(WASM): $(OBJ) $(PRE) makefile
	@if not exist "$(TARGET)" mkdir "$(TARGET)"
	$(CC) $(ESFLAGS) --pre-js $(PRE) -o $(TEMP)/$(OUT).js $(OBJ)
	@move $(TEMP)\$(OUT).* $(TARGET) >nul

$(TEMP)/%.o: $(SRCF)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(STD) -O3 -MMD -c $< -o $@

%.h: ;
%.d: ;

include $(DEP)

.PHONY: all
