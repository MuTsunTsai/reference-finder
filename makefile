
CC := emcc

SRCF := src/core
TEMP := obj
TARGET := src/lib

SRC := $(wildcard $(SRCF)/*.cpp) $(wildcard $(SRCF)/*/*.cpp) $(wildcard $(SRCF)/*/*/*.cpp)
OBJ := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.o,$(SRC))
DEP := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.d,$(SRC))

PRE := $(SRCF)/pre.js
OUT := ref

STD := -std=c++17
ESFLAGS :=\
	-sINITIAL_MEMORY=50MB\
	-sALLOW_MEMORY_GROWTH\
	-sMALLOC=dlmalloc\
	-sASYNCIFY=1\
	-sASYNCIFY_IMPORTS=['emscripten_utils_get_double_impl']\
	-sEXPORTED_RUNTIME_METHODS=['setValue']\
	-sFILESYSTEM=0\
	-sMODULARIZE=1\
	-sENVIRONMENT=worker
OPTI :=	-O3

WASM := $(TARGET)/$(OUT).wasm

ifeq ($(OS),Windows_NT)
	MK = @if not exist "$(@D)" mkdir "$(@D)"
else
	MK = @mkdir -p "$(@D)"
endif

all: $(WASM)

$(WASM): $(OBJ) $(PRE) makefile
	$(MK)
	$(CC) $(ESFLAGS) $(OPTI) --pre-js $(PRE) -o $(TARGET)/$(OUT).js $(OBJ)

$(TEMP)/%.o: $(SRCF)/%.cpp makefile
	$(MK)
	$(CC) $(STD) $(OPTI) -MMD -c $< -o $@

%.h: ;
%.d: ;

-include $(DEP)

.PHONY: all
