# SPDX-License-Identifier: GPL-2.0-only

CC  = gcc
CXX = g++
INCLUDE_FLAGS = -I./src
DEPFLAGS  = -MT "$@" -MMD -MP -MF $(@:%.o=%.d)
CFLAGS    = -Wall -Wextra -O2 -ggdb3 -D_GNU_SOURCE $(INCLUDE_FLAGS)
CXXFLAGS  = -Wall -Wextra -O2 -ggdb3 -D_GNU_SOURCE $(INCLUDE_FLAGS)
LDFLAGS   = -O2 -ggdb3
LDLIBS    = -lpthread

GWT_CC_SRC = \
	src/gwtprox/entry.c

GWT_CXX_SRC =

GWT_CC_OBJ = $(GWT_CC_SRC:.c=.c.o)
GWT_CXX_OBJ = $(GWT_CXX_SRC:.cpp=.cpp.o)
GWT_DEPS = $(GWT_CC_OBJ:.o=.d) $(GWT_CXX_OBJ:.o=.d)
GWT_BIN = gwtprox

all: $(GWT_BIN)

$(GWT_BIN): $(GWT_CC_OBJ) $(GWT_CXX_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.c.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<

%.cpp.o: %.cpp
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(GWT_DEPS)

clean:
	rm -f $(GWT_BIN) $(GWT_CC_OBJ) $(GWT_CXX_OBJ) $(GWT_DEPS)

.PHONY: all clean
