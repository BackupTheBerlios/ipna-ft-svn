LDLIBS=-lboost_program_options
CXXFLAGS=-O3 -g -I../include

PREFIX=/home/ipna

SRCS=network/Socket.cc network/Listener.cc network/SequenceNumberChecker.cc network/HostAddress.cc Logger.cc
HDRS=$(wildcard *.hpp)

FANOUT_SRCS=fanout/main.cc fanout/FanoutPacketHandler.cc $(SRCS)
FANOUT_OBJS=$(FANOUT_SRCS:.cc=.o)
FANOUT_BIN=ipna-ft-fanout

PARSER_SRCS=parser/ParserFactory.cc

CAPTURE_SRCS=capture/main.cc capture/CapturePacketHandler.cc $(PARSER_SRCS) $(SRCS)
CAPTURE_OBJS=$(CAPTURE_SRCS:.cc=.o)
CAPTURE_BIN=ipna-ft-capture

BINS=$(FANOUT_BIN) $(CAPTURE_BIN)
OBJS=$(FANOUT_OBJS) $(CAPTURE_OBJ)

.PHONY: default all fanout capture clean install

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

all: fanout capture

fanout: $(FANOUT_BIN)
capture: $(CAPTURE_BIN)

$(FANOUT_BIN) : $(FANOUT_OBJS)
	@echo "[LD ] $^ -> $@"
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(CAPTURE_BIN): $(CAPTURE_OBJS)
	@echo "[LD ] $^ -> $@"
	@$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

install: $(BINS)
	cp -p $(BINS) $(PREFIX)/bin

clean:
	-rm $(OBJS) 2>/dev/null
	-rm $(BINS) 2>/dev/null
