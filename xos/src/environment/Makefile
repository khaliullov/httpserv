####
# Project-specific details & settings
####
TARGETS            = test

LIB_TARGETS        = libenv

libenv_HEADERS     = program_config.h

libenv_OBJS        = program_config.o

test_OBJS          = test.o $(libenv_OBJS)

TOPDIR            ?= ..

include $(TOPDIR)/Makefile.include

INCLUDES_base     ?= -I ..
INCLUDES_release  ?= $(INCLUDES_base)
INCLUDES_debug    ?= $(INCLUDES_base)
INCLUDES_coverage ?= $(INCLUDES_base)
INCLUDES          ?= $(INCLUDES_$(BUILD))

DEFINES_base      ?=
DEFINES_release   ?= $(DEFINES_base) -DNDEBUG
DEFINES_debug     ?= $(DEFINES_base) -D_GLIBCXX_DEBUG
DEFINES_coverage  ?= $(DEFINES_base)

CFLAGS_base       ?= -Wall -Wextra -pthread -g3
CFLAGS_release    ?= $(CFLAGS_base) $(DEFINES_release) $(INCLUDES) -O3
CFLAGS_debug      ?= $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
CFLAGS_coverage   ?= $(CFLAGS_base) $(DEFINES_coverage) $(INCLUDES_coverage) \
                     -fprofile-arcs -ftest-coverage
CFLAGS            ?= $(CFLAGS_$(BUILD))

CXXFLAGS_base     ?= -std=c++11
CXXFLAGS_release  ?= $(CXXFLAGS_base) $(CFLAGS)
CXXFLAGS_debug    ?= $(CXXFLAGS_base) $(CFLAGS_debug)
CXXFLAGS_coverage ?= $(CXXFLAGS_base) $(CFLAGS_coverage)
CXXFLAGS          ?= $(CXXFLAGS_$(BUILD))

LDFLAGS_base      ?=
LDFLAGS_release   ?= $(LDFLAGS_base)
LDFLAGS_debug     ?= $(LDFLAGS_base) -rdynamic
LDFLAGS_coverage  ?= $(LDFLAGS_base)
LDFLAGS           ?= $(LDFLAGS_$(BUILD))

LIBS_base         ?= -lrt -ldl
LIBS_release      ?= $(LIBS_base)
LIBS_debug        ?= $(LIBS_base)
LIBS_coverage     ?= $(LIBS_base)
LIBS              ?= $(LIBS_$(BUILD))

