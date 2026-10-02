#
# FILE            makefile
#
# AUTHOR          Ken Zangelin
#
# Copyright 2026 Seamware
# SPDX-License-Identifier: Apache-2.0
#
#
# corAlloc - a bump allocator: allocations come out of one buffer (first a
# caller-supplied one, then malloc'd blocks), each zeroed, and are all freed at
# once by a reset - no per-allocation free.
#
# Every library in this stack is a SIBLING repo - `-I..` and `../<name>/lib<name>.a`
# is the layout, and it is part of the build contract rather than a convenience.
#
LIB_SO        = libcorAlloc.so
LIB           = libcorAlloc.a
CC            = gcc
INCLUDE       = -I..
DFLAGS        =
#
# EXTRA_CFLAGS - the hook for a caller that needs to ADD flags to this build.
# Not DFLAGS: `make DFLAGS=...` REPLACES it, and a `DFLAGS +=` here would be
# ignored along with it, so a caller adding one flag would drop every default.
#
CFLAGS        = -O2 -Wall -Wextra -Werror -fPIC -fstack-protector-strong $(DFLAGS) $(INCLUDE) -MMD -MP $(EXTRA_CFLAGS)

LIB_SOURCES   = CorAllocStatus.c      \
                corAlloc.c            \
                corAllocRealloc.c     \
                corAllocStrdup.c      \
                corAllocBufferInit.c  \
                corAllocBufferReset.c \
                corAllocAdopt.c       \
                corAllocInit.c        \
                corAllocVersion.c     \
                corAllocLog.c         \
                corAllocMem.c

BUILD        ?= debug

#
# Traces (COR_T, COR_LIB_T) are compiled in for a debug build only - see corLog.h / corLibLog.h.
#
ifeq ($(BUILD),debug)
CFLAGS       += -DCOR_T_ON
endif
OBJDIR        = obj/$(BUILD)
OBJECTS       = $(LIB_SOURCES:%.c=$(OBJDIR)/%.o)
DEPS          = $(OBJECTS:.o=.d) $(OBJDIR)/corAllocTest.d

#
# corAllocTest - a smoke test of the library, built with it so it can never rot.
# It stays in obj/: it is not a tool, and nothing installs it.
#
TEST          = $(OBJDIR)/corAllocTest
TEST_LIBS     = ../corBase/libcorBase.a -lpthread -lrt

all: $(LIB) $(LIB_SO) $(TEST)

#
# $(OBJDIR)/.flags - rebuild when the COMPILE LINE changes
#
# A flag change is invisible to every timestamp: the sources are older than the
# objects and make sees nothing to do, so the build silently keeps objects
# compiled with the previous flags. This records them and makes the objects
# depend on the record.
#
$(OBJDIR)/.flags: FORCE
	@mkdir -p $(OBJDIR)
	@echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@

$(OBJDIR)/%.o: %.c $(OBJDIR)/.flags
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

#
# Removed first: `ar r` replaces and adds but never removes, so an object that
# is no longer built stays in the archive forever, and the next link quietly
# uses code that is not in the tree any more.
#
$(LIB): $(OBJECTS)
	@rm -f $@
	ar rcs $@ $(OBJECTS)

$(LIB_SO): $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

$(TEST): $(OBJDIR)/corAllocTest.o $(LIB)
	$(CC) -o $@ $< $(LIB) $(TEST_LIBS)

#
# install - nothing to copy. Consumers compile with `-I..` and link
# `../corAlloc/libcorAlloc.a` straight out of the checkout, so `all` has already
# put the artefacts where every consumer looks for them.
#
install: all

di: all install

ci: clean install

clean:
	rm -rf obj $(LIB) $(LIB_SO) *.o *.d *.gcno *.gcda

FORCE:

.PHONY: all install di ci clean FORCE

-include $(DEPS)
