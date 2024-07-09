LLVMPREFIX := 
PREFIX := 
LLVMBIN := $(PREFIX)bin/
LLVMLIB := $(PREFIX)lib64/
LLVMCONFIG := $(LLVMBIN)llvm-config
#CLANG := $(LLVMBIN)clang++
CLANG := $(LLVMPREFIX)bin/clang++

CXXFLAGS += `$(LLVMCONFIG) --cxxflags`
#CXXFLAGS += -std=c++14
CXXFLAGS += -std=c++17

#ASAN := 1
ifdef ASAN
CXXFLAGS += -I/usr/lib/gcc/x86_64-redhat-linux/8/include
CXXFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address
endif

CXXFLAGS += -g
CXX := $(CLANG)
CC := $(CLANG)
LDFLAGS += `$(LLVMCONFIG) --ldflags --libs`
LDFLAGS += -lclangAST
LDFLAGS += -lclangBasic
LDFLAGS += -lclang-cpp
LDFLAGS += -lclangFrontend
ifdef LLVMLIB
LDFLAGS += -Wl,-rpath,$(LLVMLIB)
endif

ifdef ASAN
LDFLAGS += -L/usr/lib/gcc/x86_64-redhat-linux/8/
LDFLAGS += -lasan
#LDFLAGS += -static-libasan 
#LDFLAGS += -static-libsan 
endif

all :: visitor

clean ::
	rm -f *.o visitor

visitor: visitor.o
