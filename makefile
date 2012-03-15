program_NAMES := fsdiff
program_C_SRCS := $(wildcard *.c)
program_CXX_SRCS := $(wildcard *.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
CPPFLAGS += -O3 -Wall
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: clean distclean

all: $(program_NAMES)

ext2.o: ext2.cpp ext2.h
	$(LINK.cc) -c -o ext2.o ext2.cpp -std=c++0x

fsdiff.o: fsdiff.cpp ext2.h
	$(LINK.cc) -c -o fsdiff.o fsdiff.cpp -std=c++0x

fsdiff: fsdiff.o ext2.o
	$(LINK.cc) -o fsdiff fsdiff.o ext2.o -std=c++0x
	
clean:
	@- $(RM) $(program_NAMES)
	@- $(RM) $(program_OBJS)
