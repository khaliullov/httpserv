####
# Project-specific details & settings
####
export TOPDIR            = $(shell pwd)
export BUILD            ?= release

export INCLUDES_base     = -I $(TOPDIR)/xos/src
export INCLUDES_release  = $(INCLUDES_base)
export INCLUDES_debug    = $(INCLUDES_base)
export INCLUDES_coverage = $(INCLUDES_base)
export INCLUDES          = $(INCLUDES_$(BUILD))

export DEFINES_base      =
export DEFINES_release   = $(DEFINES_base) -DNDEBUG
export DEFINES_debug     = $(DEFINES_base) -D_GLIBCXX_DEBUG
export DEFINES_coverage  = $(DEFINES_base)
export DEFINES           = $(DEFINES_$(BUILD))

export CFLAGS_base       = -Wall -Wextra -pthread -g3
export CFLAGS_release    = $(CFLAGS_base) $(DEFINES) $(INCLUDES) -O3
export CFLAGS_debug      = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
export CFLAGS_coverage   = $(CFLAGS_base) $(DEFINES_coverage) \
                           $(INCLUDES_coverage) -fprofile-arcs -ftest-coverage
export CFLAGS            = $(CFLAGS_$(BUILD))

export CXXFLAGS_base     = -std=c++11
export CXXFLAGS_release  = $(CXXFLAGS_base) $(CFLAGS)
export CXXFLAGS_debug    = $(CXXFLAGS_base) $(CFLAGS_debug)
export CXXFLAGS_coverage = $(CXXFLAGS_base) $(CFLAGS_coverage)
export CXXFLAGS          = $(CXXFLAGS_$(BUILD))

export LDFLAGS_base      =
export LDFLAGS_release   = $(LDFLAGS_base)
export LDFLAGS_debug     = $(LDFLAGS_base) -rdynamic
export LDFLAGS_coverage  = $(LDFLAGS_base)
export LDFLAGS           = $(LDFLAGS_$(BUILD))

export USERLIBS          = -lfilesystem -lcodecvt
export USERLIBS_release  = $(USERLIBS)
export USERLIBS_debug    = $(USERLIBS:%=%.debug)
export USERLIBS_coverage = $(USERLIBS:%=%.coverage)

export LIBS_base         = -L $(shell pwd)/lib -lrt -ldl -lreadline \
                            $(USERLIBS_$(BUILD))
export LIBS_release      = $(LIBS_base)
export LIBS_debug        = $(LIBS_base)
export LIBS_coverage     = $(LIBS_base)
export LIBS              = $(LIBS_$(BUILD))

SUBDIRS           = xos/src/codecvt \
                    xos/src/filesystem \
                    xos/src/unit

TARGETS           = statserv client

LIB_TARGETS       =

statserv_OBJS     = main.o serverconfig.o regexclass.o \
                    http_server.o http_parser.o \
                    event.o interactive_listener.o \
                    address.o net_error.o tcp_server_socket.o

client_OBJS       = client.o

include Makefile.include

