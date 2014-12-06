####
# Project-specific details & settings
####
export TOPDIR            = $(shell pwd)

export INCLUDES_base     = -I $(TOPDIR)/xos/src
export INCLUDES_release  = $(INCLUDES_base)
export INCLUDES_debug    = $(INCLUDES_base)
export INCLUDES_coverage = $(INCLUDES_base)

export DEFINES_base      =
export DEFINES_release   = $(DEFINES_base) -DNDEBUG
export DEFINES_debug     = $(DEFINES_base) -D_GLIBCXX_DEBUG
export DEFINES_coverage  = $(DEFINES_base)

export CFLAGS_base       = -Wall -Wextra -pthread -g3
export CFLAGS_release    = $(CFLAGS_base) $(DEFINES_release) \
                           $(INCLUDES_release) -O3
export CFLAGS_debug      = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug) \
                           -fsanitize=address
export CFLAGS_coverage   = $(CFLAGS_base) $(DEFINES_coverage) \
                           $(INCLUDES_coverage) -fprofile-arcs -ftest-coverage

export CXXFLAGS_base     = -std=c++11
export CXXFLAGS_release  = $(CXXFLAGS_base) $(CFLAGS_release)
export CXXFLAGS_debug    = $(CXXFLAGS_base) $(CFLAGS_debug)
export CXXFLAGS_coverage = $(CXXFLAGS_base) $(CFLAGS_coverage)

export LDFLAGS_base      =
export LDFLAGS_release   = $(LDFLAGS_base)
export LDFLAGS_debug     = $(LDFLAGS_base) -rdynamic -fsanitize=address
export LDFLAGS_coverage  = $(LDFLAGS_base)

USERLIBS = codecvt filesystem

export LIBS_base         = -lrt -ldl -lreadline
export LIBS_release      = $(LIBS_base)
export LIBS_debug        = $(LIBS_base)
export LIBS_coverage     = $(LIBS_base)

SUBDIRS           = xos/src/codecvt \
                    xos/src/filesystem \
                    xos/src/unit

TARGETS           = statserv client

LIB_TARGETS       =

statserv_OBJS     = main.o serverconfig.o regexclass.o \
                    http_server.o http_parser.o \
                    event.o interactive_listener.o \
                    address.o net_error.o tcp_server_socket.o

client_OBJS       = client.o \
                    address.o net_error.o tcp_server_socket.o

include Makefile.include
