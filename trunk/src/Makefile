LDLIBS=-lboost_program_options
CXXFLAGS=-O3 -g -I../include

PREFIX=/home/ipna

SRCS=network/Socket.cc network/Listener.cc network/SequenceNumberChecker.cc Logger.cc
HDRS=$(wildcard *.hpp)

FANOUT_SRCS=fanout/main.cc fanout/FanoutPacketHandler.cc $(SRCS)
FANOUT_OBJS=$(FANOUT_SRCS:.cc=.o)
FANOUT_BIN=ipna-ft-fanout

BINS=$(FANOUT_BIN)
OBJS=$(FANOUT_OBJS)

.PHONY: default all fanout clean install

%.o : %.c
	@echo "[CC ] $<"
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.cc
	@echo "[C++] $<"
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

% : %.o
	@echo "[LD ] $^ => $@"
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

default: all

all: fanout

fanout: $(FANOUT_BIN)

$(FANOUT_BIN) : $(FANOUT_OBJS)
	@echo "[LD ] $^ -> $@"
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(FANOUT_OBJS): $(FANOUT_SRCS)

install: $(BINS)
	cp -p $(BINS) $(PREFIX)/bin

clean:
	-rm $(OBJS) 2>/dev/null
	-rm $(BINS) 2>/dev/null
