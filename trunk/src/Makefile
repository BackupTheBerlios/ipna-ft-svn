LDLIBS=-lboost_program_options
CXXFLAGS=-O3
CXX=g++-3.3

PREFIX=/home/ipna

SRCS=$(wildcard *.cc)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.cc=.o)
BINS=$(SRCS:.cc=)

.PHONY: default all clean install

% : %.c
	@echo "[CC ] $< $@"
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.cc
	@echo "[C++] $< $@"
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

% : %.o
	@echo "[LD ] $< $@"
	@$(CC) $(LDFLAGS) $< $(LOADLIBES) $(LDLIBS) -o $@

default: all

all: $(BINS)

$(BINS): $(OBJS) $(HDRS)

install: $(BINS)
	cp -p $(BINS) $(PREFIX)/bin

clean:
	rm $(BINS) $(OBJS)

