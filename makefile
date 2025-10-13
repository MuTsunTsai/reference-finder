
MAKEFLAGS += -j12

CXX := em++

SRCF := src/core
TEMP := obj
TARGET := src/lib

SRC := $(wildcard $(SRCF)/*.cpp) $(wildcard $(SRCF)/*/*.cpp) $(wildcard $(SRCF)/*/*/*.cpp)
OBJ := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.o,$(SRC))
DEP := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.d,$(SRC))

OUT := ref

CPPFLAGS := -I$(SRCF) -std=c++17
CXXFLAGS :=	-O3
LDFLAGS :=\
	-lidbfs.js\
	-sINITIAL_MEMORY=50MB\
	-sALLOW_MEMORY_GROWTH\
	-sASYNCIFY=1\
	-sMIN_SAFARI_VERSION=120000\
	-sASSERTIONS\
	-sMAXIMUM_MEMORY=4GB\
	-sEXPORT_ES6=1\
	-sENVIRONMENT=worker

WASM := $(TARGET)/$(OUT).wasm

ifeq ($(OS),Windows_NT)
# Error is still possible in parallel running, so we add extra protection
	MK = -@if not exist "$(@D)" mkdir "$(@D)" 2> NUL
	RM = rmdir /s /q
else
	MK = @mkdir -p "$(@D)"
endif

.PHONY: all
all: $(WASM)

$(WASM): $(OBJ)
	$(MK)
	@echo Compiling [33m$(WASM)[0m
	@$(LINK.cc) $(USRFLAGS) -o $(TARGET)/$(OUT).js $(OBJ)
	@echo [33mWebAssembly compile complete![0m

$(TEMP)/%.o: $(SRCF)/%.cpp
	$(MK)
	@echo Compiling [32m$(patsubst $(SRCF)/%,%,$<)[0m
	@$(COMPILE.cc) -MMD -c $< -o $@

$(TEMP)/main.o: $(SRCF)/main.cpp $(SRCF)/RFVersion.h
	$(MK)
	@echo Compiling [32m$(patsubst $(SRCF)/%,%,$<)[0m
	@$(COMPILE.cc) -MMD -c $< -o $@

# Ignoring old dependencies that were removed
%.h: ;
%.d: ;

$(SRCF)/RFVersion.h: package.json
	@pnpm gulp syncVer -S

-include $(DEP)

.PHONY: clean
clean:
	@$(RM) $(TEMP)

# Print memory layout of RefLine_P2L_P2L, the largest class
.PHONY: layout
layout:
	$(CXX) $(CPPFLAGS) -Xclang -fdump-record-layouts -fsyntax-only src\core\class\refLine\refLineP2LP2L.cpp >> dump.txt
