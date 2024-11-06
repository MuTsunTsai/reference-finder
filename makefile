
MAKEFLAGS += -j12

CC := emcc

SRCF := src/core
TEMP := obj
TARGET := src/lib

SRC := $(wildcard $(SRCF)/*.cpp) $(wildcard $(SRCF)/*/*.cpp) $(wildcard $(SRCF)/*/*/*.cpp)
OBJ := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.o,$(SRC))
DEP := $(patsubst $(SRCF)/%.cpp,$(TEMP)/%.d,$(SRC))

OUT := ref

CCFLAGS := -I$(SRCF) -std=c++17
EMFLAGS :=\
	-lidbfs.js\
	-sINITIAL_MEMORY=50MB\
	-sALLOW_MEMORY_GROWTH\
	-sASYNCIFY=1\
	-sMIN_SAFARI_VERSION=120000\
	-sASSERTIONS\
	-sMAXIMUM_MEMORY=4GB\
	-sEXPORTED_RUNTIME_METHODS=setValue\
	-sEXPORT_ES6=1\
	-sENVIRONMENT=worker
TIDYFLAGS :=\
	-target wasm32-unknown-emscripten\
	--sysroot=C:\Sdk\emsdk\upstream\emscripten\cache\sysroot\
	-DEMSCRIPTEN
OPTI :=	-O3

WASM := $(TARGET)/$(OUT).wasm

ifeq ($(OS),Windows_NT)
# Error is still possible in parallel running, so we add extra protection
	MK = -@if not exist "$(@D)" mkdir "$(@D)" 2> NUL
else
	MK = @mkdir -p "$(@D)"
endif

.PHONY: all
all: $(WASM)

$(WASM): $(OBJ)
	$(MK)
	@echo Compiling [33m$(WASM)[0m
	@$(CC) $(EMFLAGS) $(USRFLAGS) $(OPTI) -o $(TARGET)/$(OUT).js $(OBJ)
	@echo [33mWebAssembly compile complete![0m

$(TEMP)/%.o: $(SRCF)/%.cpp
	$(MK)
	@echo Compiling [32m$<[0m
	@$(CC) $(CCFLAGS) $(OPTI) -MMD -c $< -o $@

$(TEMP)/main.o: $(SRCF)/main.cpp $(SRCF)/RFVersion.h
	$(MK)
	@echo Compiling [32m$<[0m
	@$(CC) $(CCFLAGS) $(OPTI) -MMD -c $< -o $@

# Ignoring old dependencies that were removed
%.h: ;
%.d: ;

$(SRCF)/RFVersion.h: package.json
	@pnpm gulp syncVer -S

-include $(DEP)

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	@rmdir /s /q $(TEMP)
else
	@rm -f $(TEMP)
endif

.PHONY: tidy
tidy:
	clang-tidy $(SRC) -- $(TIDYFLAGS) $(CCFLAGS)
